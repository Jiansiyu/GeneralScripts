/*
 * OpticsGraphicCutPro.C
 *
 *  Created on: Jan 9, 2020
 *      Author: newdriver
 */


#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TString.h>
#include <TChain.h>
#include <TCut.h>
#include <TCutG.h>
#include <TH2F.h>
#include <TH1.h>
#include <TF1.h>
#include <TPaveText.h>
#include <map>
#include <vector>
#include <random>
#include <iostream>

#include <TVirtualPad.h>

#include <TObject.h>
#include <TFile.h>
#include <TLatex.h>
#include <TSystem.h>

#include <TApplication.h>
#include <boost/filesystem.hpp>
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
TString generalcutR="R.tr.n==1 && R.vdc.u1.nclust==1&& R.vdc.v1.nclust==1 && R.vdc.u2.nclust==1 && R.vdc.v2.nclust==1 && R.gold.p > 2.14 && R.gold.p < 2.2  ";
TString generalcutL="L.tr.n==1 && L.vdc.u1.nclust==1&& L.vdc.v1.nclust==1 && L.vdc.u2.nclust==1 && L.vdc.v2.nclust==1  && L.gold.p > 2.14 && L.gold.p < 2.2";//&& fEvtHdr.fEvtType==1

inline Bool_t IsFileExist (const std::string& name) {
    return !gSystem->AccessPathName(name.c_str());
}


//
// fit function for the water cell target
TF1 *SpectroCrystalFit_C12(TH1F*momentumSpectro){

    auto CGroundDp=momentumSpectro->GetXaxis()->GetBinCenter(momentumSpectro->GetMaximumBin());

    //start the fit and get the mean ans sigma
    momentumSpectro->Fit("gaus","RQ0","ep",CGroundDp-0.0003,CGroundDp+0.0003);

    double_t fgroundCrystalballPar[5];

    TF1 *fgroundCrystalball = new TF1("fgroundCrystal", "crystalball",
                                      momentumSpectro->GetFunction("gaus")->GetParameter(1)
                                      - 5 * momentumSpectro->GetFunction("gaus")->GetParameter(2),
                                      momentumSpectro->GetFunction("gaus")->GetParameter(1)
                                      + 2 * momentumSpectro->GetFunction("gaus")->GetParameter(2));
    fgroundCrystalball->SetParameters(
            momentumSpectro->GetFunction("gaus")->GetParameter(0),
            momentumSpectro->GetFunction("gaus")->GetParameter(1),
            momentumSpectro->GetFunction("gaus")->GetParameter(2), 1.64, 1.1615);

    momentumSpectro->Fit("fgroundCrystal","RQ0","ep",fgroundCrystalball->GetXmin(),fgroundCrystalball->GetXmax());
    fgroundCrystalball->GetParameters(fgroundCrystalballPar);


    TH1F *test=(TH1F *)momentumSpectro->Clone("fitTest");
    test->GetXaxis()->SetRangeUser(momentumSpectro->GetXaxis()->GetXmin(),fgroundCrystalballPar[1]-5*fgroundCrystalballPar[2]);

    double_t ffirstGuasPar[3];
    auto C1stp=test->GetXaxis()->GetBinCenter(test->GetMaximumBin());
    test->Delete();
    TF1 *ffirstGuas=new TF1 ("firststatesgaus","gaus",C1stp-2*fgroundCrystalballPar[2],C1stp+3*fgroundCrystalballPar[2]);
    momentumSpectro->Fit("firststatesgaus","R0Q","ep",ffirstGuas->GetXmin(),ffirstGuas->GetXmax());
    ffirstGuas->GetParameters(ffirstGuasPar);

    double_t ffirstCrystalPar[5];
    TF1 *ffirstCrystal=new TF1("ffirstCrystal","crystalball",ffirstGuasPar[1]-0.002,ffirstGuas->GetXmax());
    ffirstCrystal->SetParameters(ffirstGuasPar[0],ffirstGuasPar[1],ffirstGuasPar[2],1.64,1.1615);
    momentumSpectro->Fit("ffirstCrystal","RQ0","ep",ffirstCrystal->GetXmin(),ffirstCrystal->GetXmax());
    ffirstCrystal->GetParameters(ffirstCrystalPar);

    double_t fCrystalMomentumPar[10];
    TF1 *fCrystalMomentum=new TF1("fCrystalMomentum","crystalball(0)+crystalball(5)",ffirstCrystal->GetXmin(),fgroundCrystalball->GetXmax());
    std::copy(fgroundCrystalballPar,fgroundCrystalballPar+5,fCrystalMomentumPar);
    std::copy(ffirstCrystalPar,ffirstCrystalPar+5,fCrystalMomentumPar+5);
    fCrystalMomentum->SetParameters(fCrystalMomentumPar);
    momentumSpectro->Fit("fCrystalMomentum","RQ0","ep",fCrystalMomentum->GetXmin(),fCrystalMomentum->GetXmax());

    return fCrystalMomentum;
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

	momentumSpectro->Fit("fgroundCrystal","RQ0","ep",fgroundCrystalball->GetXmin(),fgroundCrystalball->GetXmax());
	fgroundCrystalball->GetParameters(fgroundCrystalballPar);


	TH1F *test=(TH1F *)momentumSpectro->Clone("fitTest");
	test->GetXaxis()->SetRangeUser(momentumSpectro->GetXaxis()->GetXmin(),fgroundCrystalballPar[1]-5*fgroundCrystalballPar[2]);

	double_t ffirstGuasPar[3];
	auto C1stp=test->GetXaxis()->GetBinCenter(test->GetMaximumBin());
	test->Delete();
	TF1 *ffirstGuas=new TF1 ("firststatesgaus","gaus",C1stp-3*fgroundCrystalballPar[2],C1stp+3*fgroundCrystalballPar[2]);
	momentumSpectro->Fit("firststatesgaus","R0Q","ep",ffirstGuas->GetXmin(),ffirstGuas->GetXmax());
	ffirstGuas->GetParameters(ffirstGuasPar);

	double_t ffirstCrystalPar[5];
	TF1 *ffirstCrystal=new TF1("ffirstCrystal","crystalball",ffirstGuasPar[1]-0.0025,ffirstGuas->GetXmax());
	ffirstCrystal->SetParameters(ffirstGuasPar[0],ffirstGuasPar[1],ffirstGuasPar[2],1.64,1.1615);
	momentumSpectro->Fit("ffirstCrystal","R","ep",ffirstCrystal->GetXmin(),ffirstCrystal->GetXmax());
	ffirstCrystal->GetParameters(ffirstCrystalPar);

	double_t fCrystalMomentumPar[10];
	TF1 *fCrystalMomentum=new TF1("fCrystalMomentum","crystalball(0)+crystalball(5)",ffirstCrystal->GetXmin(),fgroundCrystalball->GetXmax());
	std::copy(fgroundCrystalballPar,fgroundCrystalballPar+5,fCrystalMomentumPar);
	std::copy(ffirstCrystalPar,ffirstCrystalPar+5,fCrystalMomentumPar+5);
	fCrystalMomentum->SetParameters(fCrystalMomentumPar);
	momentumSpectro->Fit("fCrystalMomentum","RQ0","ep",fCrystalMomentum->GetXmin(),fCrystalMomentum->GetXmax());
	fCrystalMomentum->GetParameters(fCrystalMomentumPar);
	// get the Dp seperation for the first and second, and then project to the third and the fouth to fit the second the third excited states


	if(fitPeak>3){
		double c2GausFitPar[3];
		double c2_fitRange_Min=fCrystalMomentum->GetParameter(1)-(fCrystalMomentum->GetParameter(1)-fCrystalMomentum->GetParameter(6))*7.65407/4.43982-2*fCrystalMomentum->GetParameter(7);
		double c2_fitRange_Max=fCrystalMomentum->GetParameter(1)-(fCrystalMomentum->GetParameter(1)-fCrystalMomentum->GetParameter(6))*7.65407/4.43982+2*fCrystalMomentum->GetParameter(7);
		//fit the peak with gaussion
		momentumSpectro->Fit("gaus","R0Q","ep",c2_fitRange_Min,c2_fitRange_Max);
		momentumSpectro->GetFunction("gaus")->GetParameters(c2GausFitPar);

		double c3GausFitPar[3];
		double c3_fitRange_Min=fCrystalMomentum->GetParameter(1)-(fCrystalMomentum->GetParameter(1)-fCrystalMomentum->GetParameter(6))*9.641/4.43982-2*fCrystalMomentum->GetParameter(7);
		double c3_fitRange_Max=fCrystalMomentum->GetParameter(1)-(fCrystalMomentum->GetParameter(1)-fCrystalMomentum->GetParameter(6))*9.641/4.43982+2*fCrystalMomentum->GetParameter(7);
		// get th peak and start the fit
		momentumSpectro->Fit("gaus","R0Q","ep",c3_fitRange_Min,c3_fitRange_Max);
		momentumSpectro->GetFunction("gaus")->GetParameters(c3GausFitPar);

		double_t fCrystalGausMomentumPar[16];
		std::copy(fCrystalMomentumPar,fCrystalMomentumPar+10,fCrystalGausMomentumPar);
		std::copy(c2GausFitPar,c2GausFitPar+3,fCrystalGausMomentumPar+10);
		std::copy(c3GausFitPar,c3GausFitPar+3,fCrystalGausMomentumPar+13);

		TF1 *fCrystalGuasMomentum=new TF1("fCrystalGuasMomentum","crystalball(0)+crystalball(5)+gaus(10)+gaus(13)",c3_fitRange_Min,fgroundCrystalball->GetXmax());
		fCrystalGuasMomentum->SetParameters(fCrystalGausMomentumPar);
		momentumSpectro->Fit("fCrystalGuasMomentum","R0Q","ep",fCrystalGuasMomentum->GetXmin(),fCrystalGuasMomentum->GetXmax());
		return fCrystalGuasMomentum;

	}else if (fitPeak==3) {
		double c2GausFitPar[3];
		double c2_fitRange_Min=fCrystalMomentum->GetParameter(1)-(fCrystalMomentum->GetParameter(1)-fCrystalMomentum->GetParameter(6))*7.65407/4.43982-3*fCrystalMomentum->GetParameter(7);
		double c2_fitRange_Max=fCrystalMomentum->GetParameter(1)-(fCrystalMomentum->GetParameter(1)-fCrystalMomentum->GetParameter(6))*7.65407/4.43982+3*fCrystalMomentum->GetParameter(7);
		//fit the peak with gaussion
		momentumSpectro->Fit("gaus","","",c2_fitRange_Min,c2_fitRange_Max);
		momentumSpectro->GetFunction("gaus")->GetParameters(c2GausFitPar);

		double_t fCrystalGausMomentumPar[13];
		std::copy(fCrystalMomentumPar,fCrystalMomentumPar+10,fCrystalGausMomentumPar);
		std::copy(c2GausFitPar,c2GausFitPar+3,fCrystalGausMomentumPar+10);

		TF1 *fCrystalGuasMomentum=new TF1("fCrystalGuasMomentum","crystalball(0)+crystalball(5)+gaus(10)",c2_fitRange_Min,fgroundCrystalball->GetXmax());
		fCrystalGuasMomentum->SetParameters(fCrystalGausMomentumPar);
		momentumSpectro->Fit("fCrystalGuasMomentum","","",fCrystalGuasMomentum->GetXmin(),fCrystalGuasMomentum->GetXmax());
		return fCrystalGuasMomentum;
	}
	return fCrystalMomentum;
}





Int_t OpticsGraphicCutPro(UInt_t runID,TString folder="/home/newdriver/Storage/Research/CRex_Experiment/RasterReplay/Replay/Result") {
	// prepare the data
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
			}
		}else{
			std::cout<<"Looking file :"<<Form("%s/prexLHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
		}
	}

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
	TH2F *TargetThPhHH=(TH2F *)gROOT->FindObject("th_vs_ph");
	if(TargetThPhHH) TargetThPhHH->Delete();
	TargetThPhHH=new TH2F("th_vs_ph","th_vs_ph",1000,-0.03,0.03,1000,-0.045,0.045);

	chain->Project(TargetThPhHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),generalcut.Data());
	TargetThPhHH->Draw("zcol");

	mainPatternCanvas->Update();
	mainPatternCanvas->ToggleEventStatus();
//	mainPatternCanvas->AddExec("ex", "DynamicCoordinates()");
	mainPatternCanvas->AddExec("ex", "DynamicCanvas()");
	std::cout<<"This is an test point"<<std::endl;
	return 1;
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


	TFile *f1=new TFile("test_temp.root","UPDATE");
		assert(f1);

	// link the root tree and check which HRS we are working on
	TChain *chain = (TChain *) gROOT->FindObject("T");
	TString HRS("R");
	TString filename(chain->GetFile()->GetName());
	if (filename.Contains("RHRS")) {
	} else if (filename.Contains("LHRS")) {
		HRS = "L";
	}

    // try to extract the hall prob if this is LHRS
    double CentralP;
    if (HRS == "L") {
        TH1F *HallProbHH = new TH1F("HallLProb", "HallLProb", 1000, -1, 0);
        chain->Project(HallProbHH->GetName(), "HacL_D_LS450_FLD_DATA",
                       generalcut.Data());
        CentralP = std::abs((HallProbHH->GetMean()) * 0.95282 / 0.33930);
        std::cout << "CentralMomentum is (LHRS) for Hall Probe::" << (CentralP)
                  << std::endl;
    } else {
        //HacR_D1_NMR_SIG
        TH1F *HallR_NMR = new TH1F("HallR_NMR", "HallR_NMR", 1000, 0.7, 0.9);
        chain->Project(HallR_NMR->GetName(), "HacR_D1_NMR_SIG",
                       generalcut.Data());
        if (HallR_NMR->GetEntries()) {
            double Mag = HallR_NMR->GetMean();
            CentralP = 2.702 * (Mag) - 1.6e-03 * (Mag) * (Mag) * (Mag);
            std::cout << "CentralMomentum is (RHRS) from NMR::" << CentralP
                      << std::endl;
        } else {

            std::cout << "\033[1;33m [Warning]\033[0m Missing HallR_NMR:"
                      << std::endl;
        }
    }


	TH2 *h = (TH2*) select;
	gPad->GetCanvas()->FeedbackMode(kTRUE);

	// if the button is clicked
	// get the mouse click position in histogram
	double_t x = (gPad->PadtoX(gPad->AbsPixeltoX(gPad->GetEventX())));
	double_t y = (gPad->PadtoY(gPad->AbsPixeltoY(gPad->GetEventY())));


	int runID=(int)chain->GetMaximum("fEvtHdr.fRun");
	// create new canvas
	TCanvas *SieveRecCanvas = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(
			"SieveRecCanvas");
	if (SieveRecCanvas) {
		SieveRecCanvas->Clear();
//		delete SieveRecCanvas->GetPrimitive("Projection");
	} else
		SieveRecCanvas = new TCanvas("SieveRecCanvas", "Projection Canvas",
				1960, 1000);

	SieveRecCanvas->Divide(1, 3);
	SieveRecCanvas->cd(2)->Divide(4, 1);
    SieveRecCanvas->cd(3)->Divide(4, 1);
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
	TH1F *momentum=new TH1F(Form("C-12 gold.p run_%d",runID),Form("C-12 gold.p run_%d",runID),800,2.15,2.20);
	chain->Project(momentum->GetName(),Form("%s.gold.dp*%f+%f",HRS.Data(),CentralP,CentralP),Form("%s && %s",generalcut.Data(),cutg->GetName()));
	// get the maximum bin, this should be the first excited states
	auto CGroundp=momentum->GetXaxis()->GetBinCenter(momentum->GetMaximumBin());
	auto C1stp=CGroundp-0.00443891;
	momentum->GetXaxis()->SetRangeUser(CGroundp-0.0044*3,CGroundp+0.0044*2);
	momentum->GetXaxis()->SetTitle(Form("%s.gold.dp*%f+%f",HRS.Data(),CentralP,CentralP));
	momentum->GetYaxis()->SetTitle("#");
	momentum->Draw();

    auto fCrystalMomentum=SpectroCrystalFit_C12(momentum);
    fCrystalMomentum->Draw("same");

    double_t fCrystalMomentumPar[10];
    fCrystalMomentum->GetParameters(fCrystalMomentumPar);

	SieveRecCanvas->Update();
	// plot the reconstrcution peak
	TLine *groudposLine=new TLine(fCrystalMomentumPar[1],0,fCrystalMomentumPar[1],fCrystalMomentumPar[0]*1.1);
	groudposLine->SetLineColor(3);
	groudposLine->SetLineWidth(2);
	groudposLine->Draw("same");

	TLine *firstposLine=new TLine(fCrystalMomentumPar[6],0,fCrystalMomentumPar[6],fCrystalMomentumPar[5]*1.1);
	firstposLine->SetLineColor(3);
	firstposLine->SetLineWidth(2);
	firstposLine->Draw("same");

	TPaveText *pt = new TPaveText(0.1,0.8,0.3,0.9,"NDC");
	pt->AddText(Form("%1.3f MeV (%2.2f\%%)",1000.0*(fCrystalMomentumPar[1]-fCrystalMomentumPar[6]),100.0*abs(abs(fCrystalMomentumPar[1]-fCrystalMomentumPar[6])-0.00443891)/0.00443891));
	pt->Draw("same");

	TLatex *t1 = new TLatex(fCrystalMomentumPar[1] + 2 * fCrystalMomentumPar[2],fCrystalMomentumPar[0], Form("P=%2.5fGeV #sigma=%1.2f x 10^{-3}", fCrystalMomentumPar[1],fCrystalMomentumPar[2]*1000));
	t1->SetTextSize(0.055);
	t1->SetTextAlign(12);
	t1->SetTextColor(2);
	t1->Draw("same");

	TLatex *t2 = new TLatex(fCrystalMomentumPar[6] + 2 * fCrystalMomentumPar[7],fCrystalMomentumPar[5], Form("P=%2.5fGeV #sigma=%1.2f x 10^{-3}", fCrystalMomentumPar[6],fCrystalMomentumPar[7]*1000));
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
    {
        SieveRecCanvas->cd(3)->cd(1);
        TH1F *tgThetah=new TH1F(Form("tg_th_%d",runID),Form("tg_th_%d",runID),1000,-0.045,0.045);
        chain->Project(tgThetah->GetName(),Form("%s.gold.th",HRS.Data()),Form("%s && %s",generalcut.Data(),cutg->GetName()));
        tgThetah->GetXaxis()->SetRangeUser(tgThetah->GetMaximum()-0.005,tgThetah->GetMaximum()+0.005);
        tgThetah->Fit("gaus");
        tgThetah->Draw();
        auto thetaFunc=tgThetah->GetFunction("gaus");
        TLatex *txTheta=new TLatex(thetaFunc->GetParameter(1),thetaFunc->GetParameter(0),Form("theta:%f",thetaFunc->GetParameter(1)));
        txTheta->Draw("same");

        SieveRecCanvas->cd(3)->cd(2);
        TH1F *tgPhih=new TH1F(Form("tg_ph_%d",runID),Form("tg_ph_%d",runID),1000,-0.045,0.045);
        chain->Project(tgPhih->GetName(),Form("%s.gold.ph",HRS.Data()),Form("%s && %s",generalcut.Data(),cutg->GetName()));
        tgPhih->GetXaxis()->SetRangeUser(tgPhih->GetMaximum()-0.005,tgPhih->GetMaximum()+0.005);
        tgPhih->Fit("gaus");
        tgPhih->Draw();
        auto phiFunc=tgPhih->GetFunction("gaus");
        TLatex *txPhi=new TLatex(phiFunc->GetParameter(1),phiFunc->GetParameter(0),Form("phi:%f",phiFunc->GetParameter(1)));
        txPhi->Draw("same");

        // create file and write the data into it
        std::ofstream txtfileio("./FinalData/TargetVar/Carbon_tg_variableList.txt",std::ofstream::app);
        auto writeString=Form("%5d  %1.5f   %1.5f   %1.5f",runID,thetaFunc->GetParameter(1),phiFunc->GetParameter(1),fCrystalMomentumPar[1]);
        txtfileio << writeString<<std::endl;
        txtfileio.close();
    }
	SieveRecCanvas->SaveAs(Form("Carbon/Carbon_%d.jpg",runID));
//	hSieveHole->Delete();
//	f1->Close();
}



// load the root file and  return the TChain
TChain *LoadrootFile(UInt_t runID,TString folder="/home/newdriver/Storage/Research/CRex_Experiment/RasterReplay/Replay/Result"){
	TChain *chain=new TChain("T");
	TString HRS="R";
		if(runID<20000){HRS="L";};

		if(folder.EndsWith(".root")){
			chain->Add(folder.Data());
		}else{
			TString rootDir(folder.Data());
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
					}
				}else{
					std::cout<<"\033[1;33m [Warning]\033[0m Missing file :"<<Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
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
					}
				}else{
					std::cout<<"\033[1;33m [Warning]\033[0m Missing file :"<<Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
				}
			}
		}
		return chain;
}

Bool_t  getTheoreticalDp(int runID_in, int col_in, int row_in, double &valueP0,double &valueP1, std::string filenamePattern="./Data/theoreticalDp"){
	// get the runID and the Dp scan KineID
	std::map<int,int>kineIDList;
	{
		kineIDList[21642]=0;
		kineIDList[21641]=1;
		kineIDList[21626]=2;
		kineIDList[21632]=3;
	}

	// check the existance of the runID
	if(!(kineIDList.find(runID_in)!=kineIDList.end())) return false;


	TString HRS="R";
	if(runID_in<20000){HRS="L";};
	// check the existance of the folder
	std::string filenameP0(Form("%s/%sHRS/CheckDp_test2_DpKine%d.txt",filenamePattern.c_str(),HRS.Data(),kineIDList[runID_in]));
	std::string filenameP1(Form("%s/%sHRS/CheckDp_test2_DpKine%d.txt",filenamePattern.c_str(),HRS.Data(),kineIDList[runID_in]+4));

	//check the existance of the file
	if(!((boost::filesystem::is_regular_file(filenameP0.c_str()))&&(boost::filesystem::is_regular_file(filenameP1.c_str())))){
		return false;
	}

	// read in
	// open the ground states
	std::map<int,std::map<int, std::map<int, double>>>theroreticalDpP0list;
	std::map<int,std::map<int, std::map<int, double>>>theroreticalDpP1list;

	// read in the ground states
	{
		if (boost::filesystem::is_regular_file(filenameP0.c_str())) {
			int kineID = -1;
			int col = -1;
			int row = -1;
			double dp = -1.0;
			std::ifstream infile(filenameP0.c_str());

			while(infile >> kineID>> col >> row >> dp){
				theroreticalDpP0list[runID_in][col][row]=dp;
//				std::cout<<"KineID:"<<kineID<<"	col::"<<col<<"	row::"<<row<<"	dp::"<<dp<<std::endl;
			}
		}

	}

	// read in the first exicted states
	{

		if (boost::filesystem::is_regular_file(filenameP1.c_str())) {
			int kineID = -1;
			int col = -1;
			int row = -1;
			double dp = -1.0;
			std::ifstream infile(filenameP1.c_str());

			while(infile >> kineID>> col >> row >> dp){
				theroreticalDpP1list[runID_in][col][row]=dp;
//				std::cout<<"KineID:"<<kineID<<"	col::"<<col<<"	row::"<<row<<"	dp::"<<dp<<std::endl;
			}
		}
	}


	if ((theroreticalDpP0list.find(runID_in) != theroreticalDpP0list.end())
			&& (theroreticalDpP0list[runID_in].find(col_in)
					!= theroreticalDpP0list[runID_in].end())
			&& (theroreticalDpP0list[runID_in][col_in].find(row_in)
					!= theroreticalDpP0list[runID_in][col_in].end())) {

		valueP0 = theroreticalDpP0list[runID_in][col_in][row_in];

		if ((theroreticalDpP1list.find(runID_in) != theroreticalDpP1list.end())
				&& (theroreticalDpP1list[runID_in].find(col_in)
						!= theroreticalDpP1list[runID_in].end())
				&& (theroreticalDpP1list[runID_in][col_in].find(row_in)
						!= theroreticalDpP1list[runID_in][col_in].end())) {

			valueP1 = theroreticalDpP1list[runID_in][col_in][row_in];
			return true;
		}
	}

	return false;
}

Bool_t getTheoreticalDp(int runID_in, int col_in, int row_in, double &value, std::string filenamePattern="./Data/theoreticalDp"){
	//
	TString HRS="R";
	if(runID_in<20000){HRS="L";};
	std::string filename(Form("%s/DpKine_%s_%d.txt",filenamePattern.c_str(),HRS.Data(),runID_in));

	std::map<int,std::map<int, std::map<int, double>>>theroreticalDplist;
	// check the exitance of the file
	if(boost::filesystem::is_regular_file(filename.c_str())){
		int kineID=-1;
		int col=-1;
		int row=-1;
		double dp=-1.0;

		std::ifstream infile(filename.c_str());

		while(infile >> kineID>> col >> row >> dp){
			theroreticalDplist[kineID][col][row]=dp;
		}

		runID_in=kineID;
		if ((theroreticalDplist.find(runID_in) != theroreticalDplist.end())
				&& (theroreticalDplist[runID_in].find(col_in)
						!= theroreticalDplist[runID_in].end())
				&& (theroreticalDplist[runID_in][col_in].find(row_in)
						!= theroreticalDplist[runID_in][col_in].end())) {
			value=theroreticalDplist[runID_in][col_in][row_in];
			return true;
		}else{
			return false;
		}
	}else{
		return false;
	}
}


void test(){
	double a,b;
	if(getTheoreticalDp(21626,3,2,a,b)){
		std::cout<<a<<"	"<<b<<std::endl;
	}
}


//Used the cut file and the root file to calculate the momentum fit
// take the cut root file and the data root file, make the cut and fit all the sieve holes
// carbon target
int OpticsGraphicCutDpCheck(UInt_t runID,
		TString cutFile =
						"/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Cut20200526/RHRS/GroundMomCut",
				TString folder =
						"/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Cut20200526/RHRS/rootfiles"){

	// check which HRS we are working on
	TString HRS="R";
	if(runID<20000){HRS="L";};

	TFile *rootfileIO=new TFile(Form("./OpticsCheckRun%d_SieveMom.root",runID),"recreate");

	auto chain=LoadrootFile(runID, folder);
	//load the cut and load the canvas
	//plot the theta and phi, and load the cut file
	if(HRS=="L"){
		generalcut=generalcutL;
	}else{
		generalcut=generalcutR;
	}

	double CentralP;
	if(HRS=="L"){
		TH1F *HallProbHH = new TH1F("HallLProb", "HallLProb", 1000, -1, 0);
		chain->Project(HallProbHH->GetName(),
				"HacL_D_LS450_FLD_DATA", generalcut.Data());
		if (HallProbHH->GetEntries() != 0) {
			CentralP = std::abs(
					(HallProbHH->GetMean()) * 0.95282 / 0.33930);
			std::cout << "CentralMomentum is ::" << (CentralP) << std::endl;
		}
		HallProbHH->Delete();
	}else{
		//HacR_D1_NMR_SIG

		TH1F *HallR_NMR = new TH1F("HallR_NMR", "HallR_NMR", 1000, 0.7, 0.9);
		chain->Project(HallR_NMR->GetName(), "HacR_D1_NMR_SIG",
				generalcut.Data());
		if (HallR_NMR->GetEntries()) {
			double Mag = HallR_NMR->GetMean();
			CentralP = 2.702 * (Mag) - 1.6e-03 * (Mag) * (Mag) * (Mag);
			std::cout << "CentralMomentum is (RHRS) from NMR::" << CentralP
					<< std::endl;

			std::cout << "CentralMomentum is ::" << (CentralP) << std::endl;
			Mag=HallR_NMR->GetRMS();
			double nmrperror=2.702 * (Mag) - 1.6e-03 * (Mag) * (Mag) * (Mag);

		} else {
			std::cout << "\033[1;33m [Warning]\033[0m Missing HallR_NMR:"
					<< std::endl;
		}
	}


	// searching for the cut file
	//if the cut file is the path pointing to the cut file, it will automaticly searching for the cut file according to name rule
	if(!cutFile.EndsWith(".root")){
		cutFile=Form("%s/prexRHRS_%d_-1.root.FullCut.root",cutFile.Data(),runID);
	}

	TFile *cutFileIO=new TFile(cutFile.Data(),"READ");
	if(cutFileIO->IsZombie()){
		std::cout<<"[ERROR]:: CAN NOT FIND CUT FILE \" "<<cutFile.Data()<<"\""<<std::endl;
		return -1;
	}

	TCanvas *mainPatternCanvas=(TCanvas *)gROOT->GetListOfCanvases()->FindObject("DpCheck");
	if(!mainPatternCanvas){
		mainPatternCanvas=new TCanvas("DpCheck","DpCheck",1960,1080);
	}else{
		mainPatternCanvas->Clear();
	}

	mainPatternCanvas->Divide(1,2);

	mainPatternCanvas->cd(1)->Divide(3,1);
	mainPatternCanvas->cd(1)->cd(3)->Divide(1,2);


	mainPatternCanvas->cd(1)->cd(1);
	mainPatternCanvas->cd(1)->cd(1)->SetGridx();
	mainPatternCanvas->cd(1)->cd(1)->SetGridy();

	TH2F *TargetThPhHH=(TH2F *)gROOT->FindObject("th_vs_ph");
	if(TargetThPhHH) TargetThPhHH->Delete();
	TargetThPhHH=new TH2F("th_vs_ph","th_vs_ph",1000,-0.03,0.03,1000,-0.045,0.05);
	chain->Project(TargetThPhHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),generalcut.Data());
	TargetThPhHH->Draw("zcol");

	//loop on the files in the cut and find all the sieve hole cuts
	TCutG *sieveCut[NSieveCol][NSieveRow];
	TCut sieveAllHoleCut;
	for (int16_t col = 0; col < NSieveCol; col++){
		for (int16_t row = 0; row < NSieveRow; row++){
			auto cutg=(TCutG*)gROOT->FindObject(Form("hcut_R_%d_%d_%d", FoilID, col, row));
			if(cutg){
				sieveCut[col][row]=cutg;
				sieveCut[col][row]->SetLineWidth(2);
				sieveCut[col][row]->SetLineColor(kRed);
				sieveCut[col][row]->Draw("same");
				sieveAllHoleCut=sieveAllHoleCut||TCut(Form("hcut_R_%d_%d_%d", FoilID, col, row));

				//get the data for this canvas
				TH2F *selectedSievehh=(TH2F *)  gROOT->FindObject("Sieve_Selected_th_ph");
				if (selectedSievehh) {
					selectedSievehh->Clear();
				} else {
					selectedSievehh = new TH2F("Sieve_Selected_th_ph",
							"Sieve_Selected_th_ph", 1000,
							TargetThPhHH->GetXaxis()->GetXmin(),
							TargetThPhHH->GetXaxis()->GetXmax(), 1000,
							TargetThPhHH->GetYaxis()->GetXmin(),
							TargetThPhHH->GetYaxis()->GetXmax());
				}
				chain->Project(selectedSievehh->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),Form("%s&&%s",sieveCut[col][row]->GetName(),generalcut.Data()));
				TLatex *label=new TLatex(selectedSievehh->GetMean(1),selectedSievehh->GetMean(2),Form("(%d %d)",col,row));
				label->SetTextSize(0.03);
				label->SetTextColor(2);
				label->Draw("same");
				selectedSievehh->Delete();
			}
		}
	}
	// loop on the sieve holes and cut the Dp and select the sieves

	// loop on the sieve holes
	std::map<int,std::map<int, TH2F *>> sieveThetaPhiArray;
	std::map<int,std::map<int, TH1F *>> sieveDpArray;
	std::map<int,std::map<int, TF1 *>>sieveDpFit;
	std::map<int,std::map<int, double *>>sieveDpFitPar;

	for (int16_t col = 0; col < NSieveCol; col++) {
		for (int16_t row = 0; row < NSieveRow; row++) {
			auto cutg = (TCutG*) gROOT->FindObject(
					Form("hcut_R_%d_%d_%d", FoilID, col, row));
			if (cutg) {
				// project the chain to the Dp informations
				if(!((sieveDpArray.find(col)!=sieveDpArray.end())&&(sieveDpArray[col].find(row)!=sieveDpArray[col].end()))){
					sieveDpArray[col][row]=new TH1F(Form("Sieve_Col%d_Row%d_Dp",col,row),Form("Sieve_Col%d_Row%d_Dp",col,row),1000,-0.03,0.02);
					sieveThetaPhiArray[col][row]=new TH2F(Form("th_vs_ph_col%d_row%d",col,row),Form("th_vs_ph_col%d_row%d",col,row),1000,-0.03,0.03,1000,-0.045,0.05);
				}
				mainPatternCanvas->SetTitle(Form("SieveDp_Col%d_Row%d",col,row));

				mainPatternCanvas->cd(1)->cd(2);
				mainPatternCanvas->cd(1)->cd(2)->SetGridx();
				mainPatternCanvas->cd(1)->cd(2)->SetGridy();
				chain->Project(sieveThetaPhiArray[col][row]->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),Form("%s && %s", generalcut.Data(),cutg->GetName()));
				sieveThetaPhiArray[col][row]->Draw("zcol");


				mainPatternCanvas->cd(2);
				chain->Project(sieveDpArray[col][row]->GetName(),Form("%s.gold.dp",HRS.Data()),Form("%s && %s", generalcut.Data(),cutg->GetName()));
				sieveDpArray[col][row]->Draw();

				// start the fit
				sieveDpFit[col][row]=SpectroCrystalFitDp_C12(sieveDpArray[col][row],2);
				sieveDpFit[col][row]->Draw("same");

				//get the fit function and, get the Dp value  for each peak
//				double* sieveDpFitPar_temp;
				sieveDpFitPar[col][row]=new double[sieveDpFit[col][row]->GetNpar()];
				sieveDpFit[col][row]->GetParameters(sieveDpFitPar[col][row]);
				//draw the dp value on the canvas

				mainPatternCanvas->cd(1)->cd(3)->cd(1);
				// plot the detail on the seperate canvas
				TH1F *groundDp_temp=(TH1F *)sieveDpArray[col][row]->Clone(Form("Sieve_Col%d_Row%d_P0",col,row));
				groundDp_temp->GetXaxis()->SetRangeUser(sieveDpFitPar[col][row][1]-4*sieveDpFitPar[col][row][2],sieveDpFitPar[col][row][1]+4*sieveDpFitPar[col][row][2]);
				groundDp_temp->Draw();
				sieveDpFit[col][row]->Draw("same");

				mainPatternCanvas->cd(1)->cd(3)->cd(2);
				// plot the detail on the seperate canvas
				TH1F *firsrDp_temp=(TH1F *)sieveDpArray[col][row]->Clone(Form("Sieve_Col%d_Row%d_P1",col,row));
				firsrDp_temp->GetXaxis()->SetRangeUser(sieveDpFitPar[col][row][6]-4*sieveDpFitPar[col][row][7],sieveDpFitPar[col][row][6]+4*sieveDpFitPar[col][row][7]);
				firsrDp_temp->Draw();
				sieveDpFit[col][row]->Draw("same");


				//check the existance of the Dp array list,if exist, get the Dp bias
				// get the C-12 Optics
				mainPatternCanvas->cd(2);
				double theoreticalDpValueP0;
				double theoreticalDpValueP1;
				if(getTheoreticalDp(runID,col,row,theoreticalDpValueP0,theoreticalDpValueP1)){
					TLine *line1 = new TLine(theoreticalDpValueP0, 0,
							theoreticalDpValueP0, sieveDpFitPar[col][row][0]);
					line1->SetLineColor(3);
					line1->Draw("same");

					TLine *line2 = new TLine(theoreticalDpValueP1, 0,
							theoreticalDpValueP1, sieveDpFitPar[col][row][5]);
					line2->SetLineColor(3);
					line2->Draw("same");

					TLatex *text1=new TLatex(sieveDpFitPar[col][row][1],sieveDpFitPar[col][row][0],Form("Dp=%1.4f bias=%1.4f #times 10^{-3}",1000.0*sieveDpFitPar[col][row][1],(theoreticalDpValueP0-sieveDpFitPar[col][row][1])*1000.0));
					text1->Draw("same");
					TLatex *text2=new TLatex(sieveDpFitPar[col][row][6],sieveDpFitPar[col][row][5],Form("Dp=%1.4f bias=%1.4f #times 10^{-3}",1000.0*sieveDpFitPar[col][row][6],(theoreticalDpValueP1-sieveDpFitPar[col][row][6])*1000.0));
					text2->Draw("same");

					TLatex *textDeltaP=new TLatex(sieveDpFitPar[col][row][1]*0.5+sieveDpFitPar[col][row][6]*0.5,sieveDpFitPar[col][row][0]*0.5+sieveDpFitPar[col][row][5]*0.5,Form("#DeltaP=%1.3f MeV",(sieveDpFitPar[col][row][1]-sieveDpFitPar[col][row][6])*1000.0*CentralP));
					textDeltaP->Draw("same");

				}else{
					if(sieveDpFit[col][row]->GetNpar() >= 10){
						TLatex *text1=new TLatex(sieveDpFitPar[col][row][1],sieveDpFitPar[col][row][0],Form("Dp=%1.4f#times 10^{-3}",1000.0*sieveDpFitPar[col][row][1]));
						text1->Draw("same");
						TLatex *text2=new TLatex(sieveDpFitPar[col][row][6],sieveDpFitPar[col][row][5],Form("Dp=%1.4f#times 10^{-3}",1000.0*sieveDpFitPar[col][row][6]));
						text2->Draw("same");
					}
				}

				mainPatternCanvas->Update();
				rootfileIO->WriteObject(mainPatternCanvas,Form("SieveDp_Col%d_Row%d_canv",col,row));

			}
		}
	}

	//write all the plot to the root file
	for(auto col_iter= sieveThetaPhiArray.begin(); col_iter!=sieveThetaPhiArray.end();col_iter++){
		for(auto row_iter= col_iter->second.begin(); row_iter!=col_iter->second.end(); row_iter++){
			rootfileIO->WriteObject(sieveThetaPhiArray[col_iter->first][row_iter->first],sieveThetaPhiArray[col_iter->first][row_iter->first]->GetName());
			rootfileIO->WriteObject(sieveDpArray[col_iter->first][row_iter->first],sieveDpArray[col_iter->first][row_iter->first]->GetName());
			rootfileIO->WriteObject(sieveDpFit[col_iter->first][row_iter->first],sieveDpFit[col_iter->first][row_iter->first]->GetName());
		}
	}

	// loop on the canvas and write to root file
	rootfileIO->Write();
	rootfileIO->Close();

	return 1;
}

