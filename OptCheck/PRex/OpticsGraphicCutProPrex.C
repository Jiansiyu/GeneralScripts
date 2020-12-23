/*
 * OpticsGraphicCutPro
 * author: Siyu Jian sj9va@virginia.edu
 *          Go Hoos
 */
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TString.h>
#include <TChain.h>
#include <TCut.h>
#include <TCutG.h>
#include <TPad.h>
#include <TMath.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <TF1NormSum.h>
#include <TPaveText.h>
#include <map>
#include <vector>
#include <random>
#include <iostream>

#include <TComplex.h>
#include <TVirtualPad.h>

#include <TSpectrum2.h>
#include <TF2.h>
#include <TObject.h>
#include "TMinuit.h"
#include <TFile.h>
#include <fstream>
#include <TLatex.h>
#include <TSystem.h>

#include <TApplication.h>
#include <boost/filesystem.hpp>
#include "TMath.h"
R__LOAD_LIBRARY(/usr/lib/x86_64-linux-gnu/libboost_filesystem.so)

int FoilID=0;

int col=3;
int row=1;
int row_min=0;
int row_count=10;
const UInt_t NSieveCol = 13;
const UInt_t NSieveRow = 7;

//////////////////////////////////////////////////////////////////////////////
// Work Directory
// cut options
// Need to change
//////////////////////////////////////////////////////////////////////////////
TString prepcut;
TString generalcut;
TString generalcutR="R.tr.n==1 && R.vdc.u1.nclust==1&& R.vdc.v1.nclust==1 && R.vdc.u2.nclust==1 && R.vdc.v2.nclust==1";// && fEvtHdr.fEvtType==1";// && R.gold.p > 0.91 && R.gold.p < 0.98";
TString generalcutL="L.tr.n==1 && L.vdc.u1.nclust==1&& L.vdc.v1.nclust==1 && L.vdc.u2.nclust==1 && L.vdc.v2.nclust==1";// && fEvtHdr.fEvtType==1";//  && L.gold.p > 0.91 && L.gold.p < 0.98";

inline Bool_t IsFileExist (const std::string& name) {
//	  struct stat buffer;
//	  return (stat (name.c_str(), &buffer) == 0);
    return !gSystem->AccessPathName(name.c_str());

}

TChain *LoadRootFile(UInt_t runID,UInt_t maxFile=2,TString folder="/home/newdriver/Storage/Research/PRex_Experiment/PRex_Replay/replay/Result"){
    TChain *chain=new TChain("T");
    TString rootDir(folder.Data());
    TString HRS="R";
    if(runID>20000){ //RHRS
        if(IsFileExist(Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID))){
            std::cout<<"Add File::"<<Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
            chain->Add(Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID));

            TString filename;
            int16_t split=1;
            filename=Form("%s/prexRHRS_%d_-1_%d.root",rootDir.Data(),runID,split);
            while (IsFileExist(filename.Data())){
                std::cout<<"Add File::"<<filename.Data()<<std::endl;
                chain->Add(filename.Data());
                split++;
                filename=Form("%s/prexRHRS_%d_-1_%d.root",rootDir.Data(),runID,split);
                if(split > maxFile) break;
            }
        }else{
            std::cout<<"Looking file :"<<Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
        }
    }else{
        HRS="L";
        if(IsFileExist(Form("%s/prexLHRS_%d_-1.root",rootDir.Data(),runID))){
            std::cout<<"Add File::"<<Form("%s/prexLHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
            chain->Add(Form("%s/prexLHRS_%d_-1.root",rootDir.Data(),runID));

            TString filename;
            int16_t split=1;
            filename=Form("%s/prexLHRS_%d_-1_%d.root",rootDir.Data(),runID,split);
            while (IsFileExist(filename.Data())){
                std::cout<<"Add File::"<<filename.Data()<<std::endl;
                chain->Add(filename.Data());
                split++;
                filename=Form("%s/prexLHRS_%d_-1_%d.root",rootDir.Data(),runID,split);
                if(split > maxFile) break;
            }
        }else{
            std::cout<<"Looking file :"<<Form("%s/prexLHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
        }
    }
    return  chain;
}
// does it needed to add another function to predict the position of each peak
// add an global fit function used for the fit

TF1 *SpectroCrystalFitDp_C12(TH1F*momentumSpectro,int fitPeak=4){


	auto CGroundDp=momentumSpectro->GetXaxis()->GetBinCenter(momentumSpectro->GetMaximumBin());
	//start the fit and get the mean ans sigma
	momentumSpectro->Fit("gaus","RQ0","ep",CGroundDp-0.0003,CGroundDp+0.0003);

	double_t fgroundCrystalballPar[5];

	TF1 *fgroundCrystalball = new TF1("fgroundCrystal", "crystalball",
			momentumSpectro->GetFunction("gaus")->GetParameter(1)
					- 5 * momentumSpectro->GetFunction("gaus")->GetParameter(2),
			momentumSpectro->GetFunction("gaus")->GetParameter(1)
					+ 5 * momentumSpectro->GetFunction("gaus")->GetParameter(2));
	fgroundCrystalball->SetParameters(
			momentumSpectro->GetFunction("gaus")->GetParameter(0),
			momentumSpectro->GetFunction("gaus")->GetParameter(1),
			momentumSpectro->GetFunction("gaus")->GetParameter(2), 1.64, 1.1615);

//	momentumSpectro->Fit("fgroundCrystal","RQ0","ep",fgroundCrystalball->GetXmin(),fgroundCrystalball->GetXmax());
    fgroundCrystalball->GetParameters(fgroundCrystalballPar);

	return fgroundCrystalball;
}



TF1 *SpectroCrystalFitP_C12(TH1F*momentumSpectro,int fitPeak=4){


	auto CGroundDp=momentumSpectro->GetXaxis()->GetBinCenter(momentumSpectro->GetMaximumBin());
	//start the fit and get the mean ans sigma
	momentumSpectro->Fit("gaus","RQ0","ep",CGroundDp-0.0006,CGroundDp+0.0003);

	double_t fgroundCrystalballPar[5];

	TF1 *fgroundCrystalball = new TF1("fgroundCrystal", "crystalball",
			momentumSpectro->GetFunction("gaus")->GetParameter(1)
					- 5 * momentumSpectro->GetFunction("gaus")->GetParameter(2),
			momentumSpectro->GetFunction("gaus")->GetParameter(1)
					+ 3 * momentumSpectro->GetFunction("gaus")->GetParameter(2));
	fgroundCrystalball->SetParameters(
			momentumSpectro->GetFunction("gaus")->GetParameter(0),
			momentumSpectro->GetFunction("gaus")->GetParameter(1),
			momentumSpectro->GetFunction("gaus")->GetParameter(2), 1.64, 1.1615);

	momentumSpectro->Fit("fgroundCrystal","R0Q","ep",fgroundCrystalball->GetXmin(),fgroundCrystalball->GetXmax());
	fgroundCrystalball->GetParameters(fgroundCrystalballPar);

	return fgroundCrystalball;
}


Int_t OpticsGraphicCutPro(UInt_t runID,UInt_t maxFile=2,TString folder="/home/newdriver/Storage/Research/PRex_Experiment/PRex_Replay/replay/Result") {
	// prepare the data
	TChain *chain=LoadRootFile(runID,maxFile,folder);
	TString rootDir(folder.Data());
	TString HRS="L";
    generalcut=generalcutL;
	if(runID > 20000){
        HRS="R";
        generalcut=generalcutR;
	}

	TCanvas *mainPatternCanvas=(TCanvas *)gROOT->GetListOfCanvases()->FindObject("cutPro");
	if(!mainPatternCanvas){
		mainPatternCanvas=new TCanvas("cutPro","cutPro",600,600);
	}else{
		mainPatternCanvas->Clear();
	}

//	TCanvas *mainPatternCanvas=new TCanvas("cut","cut",600,600);
	mainPatternCanvas->Draw();
	TH2F *TargetThPhHH=(TH2F *)gROOT->FindObject("th_vs_ph");
	if(TargetThPhHH) TargetThPhHH->Delete();
	TargetThPhHH=new TH2F("th_vs_ph","th_vs_ph",1000,-0.03,0.03,1000,-0.045,0.045);

	chain->Project(TargetThPhHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),generalcut.Data());
	TargetThPhHH->Draw("zcol");

	mainPatternCanvas->Update();
	mainPatternCanvas->ToggleEventStatus();
	mainPatternCanvas->AddExec("ex", "DynamicCanvas()");
	std::cout<<"This is an test point"<<std::endl;
	return 1;
}


double getCentralP(TChain *chain, Bool_t drawFlag=false){

	// get the run Number
	 int runID=(int)chain->GetMaximum("fEvtHdr.fRun");

	 TString HRS="R";
	 if(runID<20000){
	 	HRS="L";
	 }
	 TString Experiment="PRex";

	 //TODO need to check whether this is prex/crex experiment
	 double CentralP;
	 if (Experiment  == "PRex"){
		 // for PRex experiment, both HRS NMR works, get the central P as
		 TString namehh(Form("Hall%s_NMR_run%d",HRS.Data(),runID));
		TH1F *HRSCentralPDetHH=(TH1F *)gROOT->FindObject(namehh.Data());
		if(HRSCentralPDetHH) HRSCentralPDetHH->Clear();
		HRSCentralPDetHH = new TH1F(namehh.Data(),namehh.Data(), 1000, -0.7, 0.9);

		chain->Project(HRSCentralPDetHH->GetName(),Form("Hac%s_D1_NMR_SIG", HRS.Data()), generalcut.Data());

		if (HRSCentralPDetHH->GetEntries()) {
			double Mag = HRSCentralPDetHH->GetMean();
			CentralP = 2.702 * (Mag) - 1.6e-03 * (Mag) * (Mag) * (Mag);
			HRSCentralPDetHH->GetXaxis()->SetRangeUser(HRSCentralPDetHH->GetMean() - 0.01,HRSCentralPDetHH->GetMean() + 0.01);
		} else {
			std::cout << "\033[1;33m [Warning]\033[0m Missing HallR_NMR:"<< std::endl;
		}
	 }
	return CentralP;
}

void DynamicCanvas(){
	//check which button is clicked
	//if the S button clicked, save the current  cut
	//if the the d button clicked, skip the current hole and continue with the next one

	int event = gPad->GetEvent();
	if (event == kNoEvent)
		return;

	TObject *select = gPad->GetSelected();
	if (!select)
		return;
	if (!select->InheritsFrom(TH2::Class())) {
		gPad->SetUniqueID(0);
		return;
	}
	if (event!=kButton1Down) return;


	TFile *f1=new TFile("test_temp.root","recreate");
		assert(f1);

	// link the root tree and check which HRS we are working on
	TChain *chain = (TChain *) gROOT->FindObject("T");
	TString HRS("R");
	TString filename(chain->GetFile()->GetName());
	if (filename.Contains("RHRS")) {
	} else if (filename.Contains("LHRS")) {
		HRS = "L";
	}

	UInt_t runID=chain->GetMaximum("fEvtHdr.fRun");
	double CentralP=getCentralP(chain);

	TH2 *h = (TH2*) select;
	gPad->GetCanvas()->FeedbackMode(kTRUE);

	// if the button is clicked
	// get the mouse click position in histogram
	double_t x = (gPad->PadtoX(gPad->AbsPixeltoX(gPad->GetEventX())));
	double_t y = (gPad->PadtoY(gPad->AbsPixeltoY(gPad->GetEventY())));

	// create new canvas
	TCanvas *SieveRecCanvas = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(
			"SieveRecCanvas");
	if (SieveRecCanvas) {
		SieveRecCanvas->Clear();
//		delete SieveRecCanvas->GetPrimitive("Projection");
	} else
		SieveRecCanvas = new TCanvas("SieveRecCanvas", "Projection Canvas",
				1000, 1000);

	SieveRecCanvas->Divide(1, 2);
	SieveRecCanvas->cd(2)->Divide(4, 1);
	//get the hsitogram and start rec
	SieveRecCanvas->cd(2)->cd(2);

	TH2F *selectedSievehh = (TH2F *) gROOT->FindObject("Sieve_Selected_th_ph");
	if (selectedSievehh) {
		selectedSievehh->Clear();
	}
	selectedSievehh = new TH2F("Sieve_Selected_th_ph", "Sieve_Selected_th_ph",
			100, h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax(), 100,
			h->GetYaxis()->GetXmin(), h->GetYaxis()->GetXmax());
	chain->Project(selectedSievehh->GetName(),
			Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),
			Form("sqrt((%s.gold.th-%f)^2+ (%s.gold.ph-%f)^2)<0.003 && %s",
					HRS.Data(), y, HRS.Data(), x, generalcut.Data()));
	selectedSievehh->SetContour(10);
	selectedSievehh->GetXaxis()->SetTitle(Form("%s.gold.ph", HRS.Data()));
	selectedSievehh->GetYaxis()->SetTitle(Form("%s.gold.th", HRS.Data()));
	selectedSievehh->Draw("CONT LIST");

	SieveRecCanvas->Update(); // update the canvas to let the pattern buffer in root

	// extract the contour
	TObjArray *conts = (TObjArray*) gROOT->GetListOfSpecials()->FindObject(
			"contours");
	if (!conts)
		return;
	TList *lcontour1 = (TList*) conts->At(0);
	if (!lcontour1)
		return;
	TGraph *gc1 = (TGraph*) lcontour1->First();
	if (!gc1)
		return;
	if (gc1->GetN() < 10)
		return;

	//TODO need to change the name of
	TCutG *cutg = new TCutG(Form("hcut_R_%ld",random()),
			gc1->GetN(), gc1->GetX(), gc1->GetY());
	cutg->SetLineWidth(2);
	cutg->SetLineColor(kRed);
	cutg->SetVarX(Form("%s.gold.ph", HRS.Data()));
	cutg->SetVarY(Form("%s.gold.th", HRS.Data()));
	cutg->Draw("same");

	// plot the cut on the canvas
	SieveRecCanvas->cd(2)->cd(1);

	TH2F *patternCheck = (TH2F *) gROOT->FindObject("Sieve_Pattern_Check");
	if (patternCheck) {
		patternCheck->Clear();
	}
	patternCheck = new TH2F("Sieve_Pattern_Check", "Sieve_Pattern_Check",
			h->GetXaxis()->GetNbins(), h->GetXaxis()->GetXmin(),
			h->GetXaxis()->GetXmax(), h->GetYaxis()->GetNbins(),
			h->GetYaxis()->GetXmin(), h->GetYaxis()->GetXmax());
	chain->Project(patternCheck->GetName(),
			Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),generalcut.Data());
	patternCheck->Draw("zcol");
	cutg->Draw("same");

	SieveRecCanvas->cd(1);
	SieveRecCanvas->cd(1)->SetLogy();
	// plot the dp and fit
	TH1F *momentum=new TH1F(Form("C-12 gold.p %d",runID),Form("C-12 gold.p %d",runID),150,0.94,0.96);
	chain->Project(momentum->GetName(),Form("%s.gold.dp*%f+%f",HRS.Data(),CentralP,CentralP),Form("%s && %s",generalcut.Data(),cutg->GetName()));
	momentum->Draw("same");

	SpectroCrystalFitP_C12(momentum,2);

	std::cout<<momentum->GetXaxis()->GetBinCenter(momentum->GetMaximumBin());

	auto func=momentum->GetFunction("fgroundCrystal");
	func->Draw("same");
	TLatex *txt=new TLatex(func->GetParameter(1),func->GetParameter(0),Form("%f",func->GetParameter(1)));
	txt->Draw("same");

//	std::cout<<func->GetParameter(1)<<std::endl;

/*	double_t ffirstCrystalPar[5];
	TF1 *ffirstCrystal=new TF1("ffirstCrystal","crystalball",ffirstGuasPar[1]-0.0025,ffirstGuas->GetXmax());
	ffirstCrystal->SetParameters(ffirstGuasPar[0],ffirstGuasPar[1],ffirstGuasPar[2],1.64,1.1615);
	momentum->Fit("ffirstCrystal","R","ep",ffirstCrystal->GetXmin(),ffirstCrystal->GetXmax());
	ffirstCrystal->GetParameters(ffirstCrystalPar);
	//	ffirstCrystal->Draw("same");

	// fit together
	double_t fCrystalMomentumPar[10];
	TF1 *fCrystalMomentum=new TF1("fCrystalMomentum","crystalball(0)+crystalball(5)",ffirstCrystal->GetXmin(),fgroundCrystalball->GetXmax());
	std::copy(fgroundCrystalballPar,fgroundCrystalballPar+5,fCrystalMomentumPar);
	std::copy(ffirstCrystalPar,ffirstCrystalPar+5,fCrystalMomentumPar+5);
	fCrystalMomentum->SetParameters(fCrystalMomentumPar);
	momentum->Fit("fCrystalMomentum","","",fCrystalMomentum->GetXmin(),fCrystalMomentum->GetXmax());
	fCrystalMomentum->Draw("same");
	fCrystalMomentum->GetParameters(fCrystalMomentumPar);

	SieveRecCanvas->Update();
	// plot the reconstrcution peak
	TLine *groudposLine=new TLine(fCrystalMomentumPar[1],0,fCrystalMomentumPar[1],fgroudGausPar[0]*1.1);
	groudposLine->SetLineColor(3);
	groudposLine->SetLineWidth(2);
	groudposLine->Draw("same");

	TLine *firstposLine=new TLine(fCrystalMomentumPar[6],0,fCrystalMomentumPar[6],ffirstGuasPar[0]*1.1);
	firstposLine->SetLineColor(3);
	firstposLine->SetLineWidth(2);
	firstposLine->Draw("same");

	TPaveText *pt = new TPaveText(0.1,0.8,0.3,0.9,"NDC");
	pt->AddText(Form("%1.3f MeV (%2.2f\%%)",1000.0*(fCrystalMomentumPar[1]-fCrystalMomentumPar[6]),100.0*abs(abs(fCrystalMomentumPar[1]-fCrystalMomentumPar[6])-0.00443891)/0.00443891));
	pt->Draw("same");

	TLatex *t1 = new TLatex(fgroudGausPar[1] + 2 * fgroudGausPar[2],fgroudGausPar[0], Form("P=%2.5fGeV #sigma=%1.2f x 10^{-3}", fCrystalMomentumPar[1],fCrystalMomentumPar[2]*1000));
	t1->SetTextSize(0.055);
	t1->SetTextAlign(12);
	t1->SetTextColor(2);
	t1->Draw("same");

	TLatex *t2 = new TLatex(ffirstGuasPar[1] + 2 * ffirstGuasPar[2],ffirstGuasPar[0], Form("P=%2.5fGeV #sigma=%1.2f x 10^{-3}", fCrystalMomentumPar[6],fCrystalMomentumPar[7]*1000));
	t2->SetTextSize(0.055);
	t2->SetTextAlign(12);
	t2->SetTextColor(2);
	t2->Draw("same");

	//plot the bigger plot for first excited states
	SieveRecCanvas->cd(2)->cd(3);
	TH1F *groundStats=(TH1F *)momentum->Clone("C-12 p.ground");
	groundStats->GetXaxis()->SetRangeUser(fCrystalMomentumPar[1]-0.002,fCrystalMomentumPar[1]+0.002);
	groundStats->Draw();
	groudposLine->Draw("same");

	SieveRecCanvas->cd(2)->cd(4);
	TH1F *firstStats=(TH1F *)momentum->Clone("C-12 p.first");
	firstStats->GetXaxis()->SetRangeUser(fCrystalMomentumPar[6]-0.002,fCrystalMomentumPar[6]+0.002);
	firstStats->Draw();
	firstposLine->Draw("same");

	SieveRecCanvas->Update();

	// Get Main Canvas and plot the pos on the canvas
	TH2F *hSieveHole = (TH2F *) gROOT->FindObject("sieveholeh");
	if (patternCheck) {
		patternCheck->Clear();
	}
	hSieveHole=new TH2F("sieveholeh", "sieveholeh", 1000, h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax(), 1000,
			h->GetYaxis()->GetXmin(), h->GetYaxis()->GetXmax());
	chain->Project(hSieveHole->GetName(),
				Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),
				Form("%s && %s",generalcut.Data(),cutg->GetName()));
	TCanvas *SieveMainCanvas = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(
				"cutPro");

	if(SieveMainCanvas){
		SieveMainCanvas->cd();
		cutg->Draw("same");
		TLatex *t1 = new TLatex(hSieveHole->GetMean(1)-0.001,hSieveHole->GetMean(2)+0.001, Form("%1.4fMeV", fCrystalMomentumPar[1]));
		t1->SetTextSize(0.02);
		t1->Draw("same");
		TLatex *t2 = new TLatex(hSieveHole->GetMean(1)-0.001,hSieveHole->GetMean(2)-0.001, Form("%1.3fMeV", 1000.0*(fCrystalMomentumPar[1]-fCrystalMomentumPar[6])));
		t2->SetTextSize(0.02);
		t2->Draw("same");
	}

	hSieveHole->Delete();*/
    //	f1->Close();
}


// get the scattered angle of the scattered angle
double getScatterAngle(double HRSAngle, double tg_th, double tg_ph){
    if(HRSAngle > 4.0){
        HRSAngle =HRSAngle*TMath::Pi()/180.0;
    }
    return (std::cos(HRSAngle) - tg_ph*std::sin(HRSAngle))/std::sqrt(1+tg_ph*tg_ph+tg_th*tg_th);
}

// when the sieve holes cannot seperate well, you may want to cut the sieve holes manually
Int_t OpticsGraphicCutManual(UInt_t runID,UInt_t maxFile=2,TString folder="/home/newdriver/Storage/Research/PRex_Experiment/PRex_Replay/replay/Result") {

    //
    auto chain=LoadRootFile(runID, maxFile,folder);
//    gStyle->SetOptStat(0);

    TString HRS="R";
    if (runID<20000)HRS="L";

    if(HRS=="L"){
        generalcut=generalcutL;
    }else{
        generalcut=generalcutR;
    }

    TCanvas *mainPatternCanvas=(TCanvas *)gROOT->GetListOfCanvases()->FindObject("cutPro");
    if(!mainPatternCanvas){
        mainPatternCanvas=new TCanvas("cutPro","cutPro",600,600);
    }else{
        mainPatternCanvas->Clear();
    }

    //	TCanvas *mainPatternCanvas=new TCanvas("cut","cut",600,600);
    mainPatternCanvas->Draw();
    TH2F *TargetThPhHH=(TH2F *)gROOT->FindObject(Form("th_vs_ph_run%d",runID));
    if(TargetThPhHH) TargetThPhHH->Delete();
    TargetThPhHH=new TH2F(Form("th_vs_ph_run%d",runID),Form("th_vs_ph_run%d",runID),1000,-0.03,0.03,1000,-0.045,0.045);

    chain->Project(TargetThPhHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),generalcut.Data());
    TargetThPhHH->Draw("zcol");
    mainPatternCanvas->SetGridx(10);
    mainPatternCanvas->SetGridy(10);
    mainPatternCanvas->Update();
    // input how start row and how many holes in this row


    TCutG* cutg = (TCutG*)gROOT->FindObject(Form("hcut_R_%d_%d_%d", FoilID, col, row));
    if(!cutg){
        cutg = (TCutG*)(TVirtualPad::Pad()->WaitPrimitive("CUTG", "CutG")); // making cut, store to CUTG
        mainPatternCanvas->Update();
    }
    cutg->SetLineColor(kRed);
    cutg->SetName(Form("hcut_R_%d_%d_%d", FoilID, col, row));
    cutg->SetVarX(Form("%s.gold.ph", HRS.Data()));
    cutg->SetVarY(Form("%s.gold.th", HRS.Data()));
    cutg->Draw("PL");
    mainPatternCanvas->Update();

    // create new canvas and generate the new information
    TCanvas *diagnoseCanv=(TCanvas *)gROOT->GetListOfCanvases()->FindObject("diagCanv");
    if(!diagnoseCanv){
        diagnoseCanv=new TCanvas("diagCanv","diagCanv",600,600);
    }else{
        diagnoseCanv->Clear();
    }
    diagnoseCanv->Divide(1,3);
    diagnoseCanv->cd(1)->Divide(4,1);
    diagnoseCanv->cd(1)->cd(1);
    TargetThPhHH->Draw("zcol");
    cutg->Draw("same");

    diagnoseCanv->cd(1)->cd(2);
    // create the theta and phi on target
    TH1F *tgThetah=new TH1F(Form("th_tg_run%d",runID),Form("th_tg_run%d",runID),TargetThPhHH->GetYaxis()->GetNbins(),TargetThPhHH->GetYaxis()->GetXmin(),TargetThPhHH->GetYaxis()->GetXmax());
    chain->Project(tgThetah->GetName(),Form("%s.gold.th",HRS.Data()),Form("%s && %s",generalcut.Data(),cutg->GetName()));
    tgThetah->Draw();

    diagnoseCanv->cd(1)->cd(3);
    // create the theta and phi on target
    TH1F *tgPhih=new TH1F(Form("Phi_tg_run%d",runID),Form("Phi_tg_run%d",runID),TargetThPhHH->GetXaxis()->GetNbins(),TargetThPhHH->GetXaxis()->GetXmin(),TargetThPhHH->GetXaxis()->GetXmax());
    chain->Project(tgPhih->GetName(),Form("-%s.gold.ph",HRS.Data()),Form("%s && %s",generalcut.Data(),cutg->GetName()));
    tgPhih->Draw();


    //get the angle

    {
        diagnoseCanv->cd(1)->cd(4);

        double HRSAngle = 4.806;
        if (HRS == "R") HRSAngle=4.764;
        double HRSAngleRad=HRSAngle*TMath::Pi()/180.0;
        TH1F *tgScatterAngle=new TH1F(Form("scatter Angle"),Form("scatter Angle"),1000,3,8);
        chain->Project(tgScatterAngle->GetName(),Form("TMath::ACos((TMath::Cos(%f)-%s.gold.ph*TMath::Sin(%f))/TMath::Sqrt(1+%s.gold.th*%s.gold.th+%s.gold.ph*%s.gold.ph))*180/TMath::Pi()",HRSAngleRad,HRS.Data(),HRSAngleRad,HRS.Data(),HRS.Data(),HRS.Data(),HRS.Data()),Form("%s && %s",generalcut.Data(),cutg->GetName()));
        tgScatterAngle->Draw();
    }
    double CentralP=getCentralP(chain);
    {
        diagnoseCanv->cd(2)->cd(1);
        TH1F *momentum=new TH1F(Form("C-12 gold.p %d",runID),Form("C-12 gold.p %d",runID),150,0.94,0.96);
        chain->Project(momentum->GetName(),Form("%s.gold.dp*%f+%f",HRS.Data(),CentralP,CentralP),Form("%s && %s",generalcut.Data(),cutg->GetName()));
        momentum->Draw();

    }


    return 1;
}
