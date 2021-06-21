/*
 * cutPro.C
 *
 *  Created on: Dec 12, 2019
 *      Author: newdriver
 */

#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TString.h>
#include <TChain.h>
#include <TCut.h>
#include <TCutG.h>
#include <TMath.h>
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
#include <fstream>
#include <TLatex.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TVector3.h>
#include <TRotation.h>
#include "TImage.h"
#include <Math/Functor.h>

#include "Math/RootFinder.h"

int FoilID=0;

int col=3;
int row=1;
int row_min=0;
int row_count=10;
const UInt_t NSieveCol = 13;
const UInt_t NSieveRow = 7;
double CentralP;

//////////////////////////////////////////////////////////////////////////////
// Work Directory
// cut options
// Need to change
//////////////////////////////////////////////////////////////////////////////
TString prepcut;
TString generalcut;
//TString generalcutR="R.tr.n==1 && R.vdc.u1.nclust==1&& R.vdc.v1.nclust==1 && R.vdc.u2.nclust==1 && R.vdc.v2.nclust==1 && fEvtHdr.fEvtType==1 && R.gold.p > 2.14 && R.gold.p < 2.2";
TString generalcutR="R.tr.n==1";// && R.vdc.u1.nclust==1&& R.vdc.v1.nclust==1 && R.vdc.u2.nclust==1 && R.vdc.v2.nclust==1 && R.gold.p > 2.14 && R.gold.p < 2.19";
//TString generalcutL="L.tr.n==1 && L.vdc.u1.nclust==1&& L.vdc.v1.nclust==1 && L.vdc.u2.nclust==1 && L.vdc.v2.nclust==1 && fEvtHdr.fEvtType==1 && L.gold.p > 2.14 && L.gold.p < 2.19";
TString generalcutL="L.tr.n==1";// && L.vdc.u1.nclust==1&& L.vdc.v1.nclust==1 && L.vdc.u2.nclust==1 && L.vdc.v2.nclust==1  && L.gold.p > 2.14 && L.gold.p < 2.19";


//
inline double GetPointingAngle(double DeltaE, double BeamE=2.17568){
	//In pointing measurement,
	// calculate the HRS angle according to H and O seperation

	Double_t amu = 931.494028 * 1.e-3;  // amu to GeV
	Double_t H2O = 18 * amu;
	Double_t TargetH = amu;
	double_t TargetO = 16 * amu;
	Double_t mass_tg = H2O;
	double beamE = BeamE;         // in GeV
	// get the pointing measurement
	double deltaE = DeltaE; // in GeV

	double a = 4.0 * beamE * deltaE;
	double b = 2.0 * deltaE * (TargetH + TargetO)+ 2.0 * beamE * (TargetH - TargetO);
	double c = deltaE * TargetH * TargetO / beamE;
	double sin2theta = ((0. - b) - TMath::Sqrt(b * b - 4.0 * a * c)) / (2.0 * a);
	double HRSAngleReal=TMath::ASin(TMath::Sqrt(sin2theta)) * 2.0 * 180.0 / TMath::Pi();
	return HRSAngleReal;
}


inline Bool_t IsFileExist (const std::string& name) {
    return !gSystem->AccessPathName(name.c_str());
//	  struct stat buffer;
//	  return (stat (name.c_str(), &buffer) == 0);
}


//
double GLobalSovler_scatteredAngle;
double GLobalSovler_theta_tg;
double GLobalSovler_phi_tg;

double GetHRSAngle(double HRSAngle){

    TVector3 TCSX(0, -1, 0);
    TVector3 TCSZ(TMath::Sin(HRSAngle), 0, TMath::Cos(HRSAngle));
    TVector3 TCSY = TCSZ.Cross(TCSX);
    TRotation fTCSInHCS;
    fTCSInHCS.RotateAxes(TCSX, TCSY, TCSZ);
    TVector3 MomDirectionTCS(GLobalSovler_theta_tg,GLobalSovler_phi_tg,1); // target variables
    TVector3 MomDirectionHCS = fTCSInHCS*MomDirectionTCS;
    TVector3 BeamDirection(0, 0, 1);
    const Double_t ScatteringAngle = BeamDirection.Angle(MomDirectionHCS);
    return ScatteringAngle - GLobalSovler_scatteredAngle;
}

// fit function for the water cell target
TF1 *SpectroCrystalFit_H2O(TH1F*momentumSpectro){

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


// input: the runID
//		: cut root file
//TODO, need to input the size ID vs. scattered angle chart
Int_t OpticsGraphicCutProH20(UInt_t type,UInt_t runID,
		TString cutFile =
				"/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Water/WithOutMomCut/prexLHRS_2726_-1.root.FullCut.root",
		TString folder =
				"/home/newdriver/Storage/Research/CRex_Experiment/RasterReplay/Replay/Result") {
	TChain *chain=new TChain("T");
	// if the folder itself is and root file
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
	// get list of files, and print summary



	//load the cut and load the canvas
	//plot the theta and phi, and load the cut file
	if(HRS=="L"){
		generalcut=generalcutL;
	}else{
		generalcut=generalcutR;
	}

	// try to extract the hall prob if this is LHRS
	double CentralP;
	if(HRS=="L"){
		TH1F *HallProbHH = new TH1F("HallLProb", "HallLProb", 1000, -1, 0);
		chain->Project(HallProbHH->GetName(), "HacL_D_LS450_FLD_DATA",generalcut.Data());
		CentralP = std::abs((HallProbHH->GetMean()) * 0.95282 / 0.33930);
		std::cout << "CentralMomentum is (LHRS) for Hall Probe::" << (CentralP) << std::endl;
	}else{
		//HacR_D1_NMR_SIG
		TH1F *HallR_NMR = new TH1F("HallR_NMR", "HallR_NMR", 1000, 0.7, 0.9);
		chain->Project(HallR_NMR->GetName(), "HacR_D1_NMR_SIG",generalcut.Data());
		if (HallR_NMR->GetEntries()) {
			double Mag = HallR_NMR->GetMean();
			CentralP = 2.702 * (Mag) - 1.6e-03 * (Mag) * (Mag) * (Mag);
			std::cout << "CentralMomentum is (RHRS) from NMR::" << CentralP
					<< std::endl;
		}else{

			std::cout<<"\033[1;33m [Warning]\033[0m Missing HallR_NMR:"<<std::endl;
		}
	}


	TCanvas *mainPatternCanvas=(TCanvas *)gROOT->GetListOfCanvases()->FindObject("cutPro");
	if(!mainPatternCanvas){
		mainPatternCanvas=new TCanvas("cutPro","cutPro",600,600);
	}else{
		mainPatternCanvas->Clear();
	}
	mainPatternCanvas->Divide(2,2);
	mainPatternCanvas->Draw();

	mainPatternCanvas->cd(1);
	// check initial data set without cut and check it with cut
	TH2F *TargetThPh_SieveNoCutHH=(TH2F *)gROOT->FindObject("th_vs_ph_No_cut");
	if(TargetThPh_SieveNoCutHH) TargetThPh_SieveNoCutHH->Delete();
	TargetThPh_SieveNoCutHH=new TH2F("th_vs_ph_No_cut","th_vs_ph with No Cut",1000,-0.03,0.03,1000,-0.045,0.045);
	chain->Project(TargetThPh_SieveNoCutHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()));   // draw all the data with no cut
	TargetThPh_SieveNoCutHH->Draw("zcol");
	if(!(TargetThPh_SieveNoCutHH->GetEntries())){
		TPaveText *text=new TPaveText(0.2,0.2,0.8,0.8,"NDC");
		text->AddText("No Event Found !!!");
		text->SetTextColor(kRed);
		text->Draw("same");
	}

	mainPatternCanvas->cd(2);
	TH2F *TargetThPhGeneralCutHH=(TH2F *)gROOT->FindObject("th_vs_ph_GeneralCut");
	if(TargetThPhGeneralCutHH) TargetThPhGeneralCutHH->Delete();
	TargetThPhGeneralCutHH=new TH2F("th_vs_ph_GeneralCut","th_vs_ph General Cut",1000,-0.03,0.03,1000,-0.045,0.045);
	chain->Project(TargetThPhGeneralCutHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),generalcut.Data());
	TargetThPhGeneralCutHH->Draw("zcol");
	if(!(TargetThPhGeneralCutHH->GetEntries())){
			TPaveText *text=new TPaveText(0.2,0.2,0.8,0.8,"NDC");
			text->AddText("No Event Found !!!");
			text->SetTextColor(kRed);
			text->Draw("same");
		}

	mainPatternCanvas->cd(3);
	TH2F *TargetThPhHH=(TH2F *)gROOT->FindObject("th_vs_ph");
	if(TargetThPhHH) TargetThPhHH->Delete();
	TargetThPhHH=new TH2F("th_vs_ph","th_vs_ph",1000,-0.03,0.03,1000,-0.045,0.045);

	chain->Project(TargetThPhHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),generalcut.Data());
	TargetThPhHH->Draw("zcol");

	// read the cut file and load the cut, create the new data set with the cut
	// load the cut file
	TFile *cutFileIO=new TFile(cutFile.Data(),"READ");
	if(cutFileIO->IsZombie()){
		std::cout<<"[ERROR]:: CAN NOT FIND CUT FILE \" "<<cutFile.Data()<<"\""<<std::endl;
		return -1;
	}

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
				label->SetTextSize(0.04);
				label->SetTextColor(2);
				label->Draw("same");
				selectedSievehh->Delete();
			}
		}
	}
	sieveAllHoleCut=sieveAllHoleCut+TCut(generalcut.Data());

	mainPatternCanvas->cd(4);
	TH2F *TargetThPh_SieveCutHH=(TH2F *)gROOT->FindObject("th_vs_ph_cut");
	if(TargetThPh_SieveCutHH) TargetThPh_SieveCutHH->Delete();
	TargetThPh_SieveCutHH=new TH2F("th_vs_ph_cut","th_vs_ph_cut",1000,-0.03,0.03,1000,-0.045,0.045);
	chain->Project(TargetThPh_SieveCutHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),sieveAllHoleCut);
	TargetThPh_SieveCutHH->Draw("zcol");
	mainPatternCanvas->Update();


	// fit each individual momentum
	TCanvas *canvastest=new TCanvas("canvs","sasas",1000,1000);
	canvastest->Divide(NSieveCol,NSieveRow);
		canvastest->Draw();
	std::map<int, std::map<int, double *>> MomentumFitParArray;
	std::map<int, std::map<int, TH1F *>> SieveMomentumArray;

	for (int16_t col = 0; col < NSieveCol; col++) {
		for (int16_t row = 0; row < NSieveRow; row++) {
			auto cutg = (TCutG*) gROOT->FindObject(Form("hcut_R_%d_%d_%d", FoilID, col, row));
			if (cutg) {
				canvastest->cd(col*NSieveRow+row+1);
				std::cout<<"Fitting col"<<col<<"row"<<row<<std::endl;
				SieveMomentumArray[col][row]=new TH1F(Form("Sieve_Col%d_Row%d_Momentum",col,row),Form("Sieve_Col%d_Row%d_Momentum",col,row),1000,2.1,2.2);
				TCut sieveCut=TCut(generalcut.Data())+TCut(Form("hcut_R_%d_%d_%d", FoilID, col, row));
				chain->Project(SieveMomentumArray[col][row]->GetName(),Form("%s.gold.dp*%f+%f",HRS.Data(),CentralP,CentralP),sieveCut);
				SieveMomentumArray[col][row]->Draw("zcol");
				if(SieveMomentumArray[col][row]->GetEntries())
				{
					auto fitFunction=SpectroCrystalFit_H2O(SieveMomentumArray[col][row]);

					MomentumFitParArray[col][row]=new double[10];
					fitFunction->GetParameters(MomentumFitParArray[col][row]);
					fitFunction->Draw("same");
					canvastest->Update();
				}
			}
		}
	}
	canvastest->Update();

	// plot all the Momentum, and get  the momentum difference
	std::map<int, std::map<int, double>> scatteredAngleArray;
	std::map<int, std::map<int, double>> scatteredAngleEnergyDiff;

	for (int16_t col = 0; col < NSieveCol; col++) {
		for (int16_t row = 0; row < NSieveRow; row++) {
			if((SieveMomentumArray.find(col)!=SieveMomentumArray.end()) &&(SieveMomentumArray[col].find(row)!=SieveMomentumArray[col].end())){
				//calculate the sieve momentumn difference
				double  DeltaE=MomentumFitParArray[col][row][1]-MomentumFitParArray[col][row][6];
				double scatteredAngle=GetPointingAngle(DeltaE);
				// take the angle on the target coordination and get the central sieve position
				scatteredAngleArray[col][row]=scatteredAngle;
				scatteredAngleEnergyDiff[col][row]=DeltaE;
			}
		}
	}

	// how to the get the position for the central angles
	// get the target theta and phi from the measurement
	std::map<int, std::map<int, double>> thetaValueArray;
	std::map<int, std::map<int, double>> phiValueArray;
	for (int16_t col = 0; col < NSieveCol; col++) {
		for (int16_t row = 0; row < NSieveRow; row++) {
			auto cutg = (TCutG*) gROOT->FindObject(Form("hcut_R_%d_%d_%d", FoilID, col, row));
			if (cutg) {
				std::cout<<"Get the theta/phi position: col"<<col<<"row"<<row<<std::endl;
				// get the theta and phi for the sieve hole
				TH2F *TargetThPhSieve=(TH2F *)gROOT->FindObject(Form("th_vs_ph_cut_col%d_row%d",col,row));
				if(TargetThPhSieve) TargetThPhSieve->Delete();
				TargetThPhSieve=new TH2F(Form("th_vs_ph_cut_col%d_row%d",col,row),Form("th_vs_ph_cut_col%d_row%d",col,row),1000,-0.03,0.03,1000,-0.045,0.045);
				TCut sieveCut=TCut(generalcut.Data())+TCut(Form("hcut_R_%d_%d_%d", FoilID, col, row));
				//project the sieve
				chain->Project(TargetThPhSieve->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),sieveCut);

				auto projectx=TargetThPhSieve->ProjectionX();
				auto projecty=TargetThPhSieve->ProjectionY();

				// get central sieve position
				auto maximumX=projectx->GetXaxis()->GetBinCenter(projectx->GetMaximumBin());
				auto maximumY=projecty->GetXaxis()->GetBinCenter(projecty->GetMaximumBin());

				projectx->Fit("gaus","R","ep",maximumX-0.003,maximumX+0.003);
				projecty->Fit("gaus","R","ep",maximumY-0.003,maximumY+0.003);

				double gausXFitPar[3];
				double gausYFitPar[3];
				projectx->GetFunction("gaus")->GetParameters(gausXFitPar);
				projecty->GetFunction("gaus")->GetParameters(gausYFitPar);

				thetaValueArray[col][row]=gausXFitPar[1];
				phiValueArray[col][row]=gausYFitPar[1];
			}
		}
	}

	// TODO add the code used for calculated the HRS
	//get the scattered angle

	TH2F *sieveIDHRSAngle=new TH2F("ID vs. HRS angle","ID vs. HRS angle",100,0,100,100,0,10);
	for (int16_t col = 0; col < NSieveCol; col++) {
			for (int16_t row = 0; row < NSieveRow; row++) {
			if (thetaValueArray.find(col) != thetaValueArray.end()
					&& (thetaValueArray[col].find(row)
							!= thetaValueArray[col].end())
					&& (scatteredAngleArray.find(col)
							!= scatteredAngleArray.end())
					&& (scatteredAngleArray[col].find(row)
							!= scatteredAngleArray[col].end())) {
				std::cout<<"Get the HRS angle from ("<<col<<","<<row<<std::endl;
				double a = scatteredAngleArray[col][row];
				double theta_tg=thetaValueArray[col][row];
				double phi_tg=phiValueArray[col][row];

//				auto result = GetHRSAngle(a,theta_tg,phi_tg);
//				std::cout<<"result:: 1-> "<<result<<std::endl;

				GLobalSovler_phi_tg=phi_tg;
				GLobalSovler_theta_tg=theta_tg;
				GLobalSovler_scatteredAngle=a*TMath::Pi()/180.0;

				// sovle the function and get the angle
				 ROOT::Math::Functor1D f1D(&GetHRSAngle);
				 ROOT::Math::RootFinder rfb(ROOT::Math::RootFinder::kBRENT);
				 rfb.SetFunction(f1D,0.0,6.0*TMath::Pi()/180.0);
				 rfb.Solve();
				 std::cout << rfb.Root()*180.0/TMath::Pi() << "  incease error:";
				 sieveIDHRSAngle->Fill(col*NSieveRow+row,rfb.Root()*180.0/TMath::Pi());
				 GLobalSovler_phi_tg=phi_tg*1.01;
				 GLobalSovler_theta_tg=theta_tg*1.01;
				 GLobalSovler_scatteredAngle=a*TMath::Pi()/180.0;
				 rfb.Solve();
				 std::cout << rfb.Root()*180.0/TMath::Pi()<<std::endl;

				}
			}
	}
	TCanvas *canvastest1=new TCanvas("canvs1","sasas1",1000,1000);
	canvastest1->cd();

	sieveIDHRSAngle->SetLineWidth(2);
	sieveIDHRSAngle->SetMarkerStyle(20);
	sieveIDHRSAngle->Draw();
	canvastest1->Update();


	TH2F *momentumP1=new TH2F("momentumP1","MomentmP0",100,0,100,1000,2.1,2.2);
	TH2F *momentumP2=new TH2F("momentumP2","MomentmP1",100,0,100,1000,2.1,2.2);
	for (auto iter_col=MomentumFitParArray.begin();iter_col!=MomentumFitParArray.end();iter_col++){
		for (auto iter_row=iter_col->second.begin();iter_row!=iter_col->second.end();iter_row++){
			momentumP1->Fill(iter_col->first*NSieveRow+iter_row->first,iter_row->second[1]);
			momentumP2->Fill(iter_col->first*NSieveRow+iter_row->first,iter_row->second[6]);
		}
	}

	TCanvas *canvastest2=new TCanvas("canvs2","sasas2",1000,1000);
	canvastest2->cd();

	momentumP1->SetLineWidth(2);
	momentumP1->SetMarkerStyle(20);
	momentumP1->Draw();

	momentumP2->SetLineWidth(2);
		momentumP2->SetMarkerStyle(20);
		momentumP2->SetMarkerColor(kRed);
		momentumP2->SetLineColor(kRed);

		momentumP2->Draw("same");

	canvastest2->Update();


	return 1;
}



// input: the runID
//        cut root file
//TODO, need to input the size ID vs. scattered angle chart
Int_t OpticsGraphicCutProH20(UInt_t runID,double centralP,TString cutFile, TString folder="/home/newdriver/Storage/Research/CRex_Experiment/optReplay/Result") {
	CentralP=centralP;
	TChain *chain=new TChain("T");
	// if the folder itself is and root file
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
	}

	//load the cut and load the canvas
	//plot the theta and phi, and load the cut file
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
	mainPatternCanvas->Divide(1,2);
	mainPatternCanvas->Draw();
	mainPatternCanvas->cd(1);
	TH2F *TargetThPhHH=(TH2F *)gROOT->FindObject("th_vs_ph");
	if(TargetThPhHH) TargetThPhHH->Delete();
	TargetThPhHH=new TH2F("th_vs_ph","th_vs_ph",1000,-0.03,0.03,1000,-0.045,0.045);

	chain->Project(TargetThPhHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),generalcut.Data());
	TargetThPhHH->Draw("zcol");


	// read the cut file and load the cut, create the new data set with the cut
	// load the cut file
	TFile *cutFileIO=new TFile(cutFile.Data(),"READ");
	if(cutFileIO->IsZombie()){
		std::cout<<"[ERROR]:: CAN NOT FIND CUT FILE \" "<<cutFile.Data()<<"\""<<std::endl;
		return -1;
	}

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
				label->SetTextSize(0.04);
				label->SetTextColor(2);
				label->Draw("same");
				selectedSievehh->Delete();
			}
		}
	}
	sieveAllHoleCut=sieveAllHoleCut+TCut(generalcut.Data());

	mainPatternCanvas->cd(2);
	TH2F *TargetThPh_SieveCutHH=(TH2F *)gROOT->FindObject("th_vs_ph_cut");
	if(TargetThPh_SieveCutHH) TargetThPh_SieveCutHH->Delete();
	TargetThPh_SieveCutHH=new TH2F("th_vs_ph_cut","th_vs_ph_cut",1000,-0.03,0.03,1000,-0.045,0.045);
	chain->Project(TargetThPh_SieveCutHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),sieveAllHoleCut);
	TargetThPh_SieveCutHH->Draw("zcol");
	mainPatternCanvas->Update();

	// fit each individual sieve holes
	//create the
	return 1;
}

Int_t OpticsGraphicCutProH20(UInt_t runID,TString folder="/home/newdriver/Storage/Research/CRex_Experiment/RasterReplay/Replay/Result") {
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


double HRSAngleBPMCorrection(UInt_t runID){
    //create the the canvas and calculate the angle correction cause the bpm
    std::map<UInt_t, double> bpmValue;


    std::map<UInt_t,double> bpmCorrectArray;

    return 0.0;
}

double_t getBeamE(int runID,TChain *chain,TString beamEfname="/home/newdriver/Learning/GeneralScripts/halog/beamE.txt"){
	std::map<int, double_t> beamE;
	beamE[21739]=2.1763077;
	beamE[21740]=2.1763047;
	beamE[21789]=2.1762745;
	beamE[21790]=2.1762517;
    beamE[21762]=2.1763254;

	beamE[2566]=2.175918588;
	beamE[2565]=2.175984498;
	beamE[2550]=2.17560073;
	beamE[2556]=2.1762867;
	beamE[2674]=2.1763062;
	beamE[2697]=2.1763254;
	beamE[2726]=2.1762729;

	//TODO check the root file, if it contains the hallp information, need to use this value
    {
        double beamERangeMin=2100;
        double beamERangeMax=2200;
        chain->GetListOfBranches()->Contains("HALLA_p");
        TH1F *HallEpicsBeamE = new TH1F("HallEpicsBeamE", "HallEpicsBeamE", 1000, beamERangeMin, beamERangeMax);
        chain->Project(HallEpicsBeamE->GetName(),"HALLA_p");
        double epicsBeamE=HallEpicsBeamE->GetMean();
        if ((epicsBeamE > 2170)&&(epicsBeamE < 2180)){
            std::cout<<"\033[1;32m [Infor]\033[0m Read in the Beam E in ROOT file: "<<epicsBeamE<<std::endl;
            return  epicsBeamE/1000.0;
        }
//        else{
//            std::cout<<"\033[1;31m [CAUTION]\033[0m Can not read reasonable Beam E from ROOT file:: "<<runID<<" ( "<<epicsBeamE/1000.0<<")"<<std::endl;
//        }
    }

	//read in the beamE information and parser
	if ((!beamEfname.IsNull()) && IsFileExist(beamEfname.Data())){
		std::cout<<"\033[1;32m [Infor]\033[0m Read in the Beam E file: "<<beamEfname.Data()<<std::endl;

		std::ifstream infile(beamEfname.Data());

		int runID_temp;
		float beamE_temp;
		while (infile >> runID_temp >> beamE_temp){
        //std::cout<<"runID:"<<runID_temp<<"   ->   "<<beamE_temp<<std::endl;
           beamE[runID_temp]=beamE_temp/1000.0;
		}

	}else{
		std::cout<<"\033[1;33m [Warning]\033[0m can not find file "<<beamEfname.Data()<<" Skip the beamE file!!!"<<std::endl;

	}


	if(beamE.find(runID)!=beamE.end()){

		return beamE[runID];
	}else{
		std::cout<<"\033[1;31m [CAUTION]\033[0m Can not find the Beam E for run"<<runID<<" Using default value!!!["<<__func__<<"("<<__LINE__<<")]"<<std::endl;
		return 2.17568;
	}

}


void DynamicCanvas(){
	//check which button is clicked
	//if the S button clicked, save the current  cut
	//if the the d button clicked, skip the current hole and continue with the next one

	// need to get the beamE


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


	TFile *f1=new TFile("test_temp.root","RECREATE");
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



	TH1F *eventIDhh=new TH1F("eventID","eventID",800000,0,800000);
	chain->Project(eventIDhh->GetName(),"fEvtHdr.fRun");
	int eventID=int(eventIDhh->GetMean());


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
	} else
		SieveRecCanvas = new TCanvas("SieveRecCanvas", "Projection Canvas",
				1000, 1000);

	SieveRecCanvas->Divide(1, 3);   // on the third line, will display the bpm correction term

	SieveRecCanvas->cd(2)->Divide(4, 1);
    SieveRecCanvas->cd(3)->Divide(4,1);
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
	TH1F *momentum=new TH1F(Form("H2O gold.p run%d",eventID),Form("H2O gold.p run%d",eventID),500,2.1,2.2);
	chain->Project(momentum->GetName(),Form("%s.gold.dp*%f+%f",HRS.Data(),CentralP,CentralP),Form("%s && %s",generalcut.Data(),cutg->GetName()));
	// get the maximum bin, this should be the first excited states
	auto CGroundp=momentum->GetXaxis()->GetBinCenter(momentum->GetMaximumBin());

	momentum->GetXaxis()->SetRangeUser(CGroundp-0.02,CGroundp+0.0044*2);
	momentum->GetXaxis()->SetTitle("gold.p");
	momentum->GetYaxis()->SetTitle("#");

	momentum->Draw();
	double_t fgroudGausPar[3];
	double_t ffirstGuasPar[3];
	TF1 *fgroudGaus=new TF1("groudstatesgaus","gaus",CGroundp-0.0005,CGroundp+0.0005);
	momentum->Fit("groudstatesgaus","RQ0","ep",fgroudGaus->GetXmin(),fgroudGaus->GetXmax());
	fgroudGaus->Draw("same");
	fgroudGaus->GetParameters(fgroudGausPar);

	TH1F *test=(TH1F *)momentum->Clone("fitTest");
	test->GetXaxis()->SetRangeUser(momentum->GetXaxis()->GetXmin(),fgroudGausPar[1]-10*fgroudGausPar[2]);

	auto C1stp=test->GetXaxis()->GetBinCenter(test->GetMaximumBin());
//	auto C1stp=2.1565;//CGroundp-0.016504;

	TF1 *ffirstGuas=new TF1 ("firststatesgaus","gaus",C1stp-0.0015,C1stp+0.00155);
	momentum->Fit("firststatesgaus","RQ0","ep",ffirstGuas->GetXmin(),ffirstGuas->GetXmax());
	ffirstGuas->Draw("same");
	ffirstGuas->GetParameters(ffirstGuasPar);


	// change the gause fit to cristal ball
	double_t fgroundCrystalballPar[5];
	TF1 *fgroundCrystalball=new TF1("fgroundCrystal","crystalball",fgroudGausPar[1]-0.0030,fgroudGaus->GetXmax()+0.0003);
	fgroundCrystalball->SetParameters(fgroudGausPar[0],fgroudGausPar[1],fgroudGausPar[2],1.64,1.1615);
	momentum->Fit("fgroundCrystal","RQ0","same",fgroundCrystalball->GetXmin(),fgroundCrystalball->GetXmax());
	fgroundCrystalball->GetParameters(fgroundCrystalballPar);

	//fgroundCrystalball->Draw("same");

	double_t ffirstCrystalPar[5];
	TF1 *ffirstCrystal=new TF1("ffirstCrystal","crystalball",ffirstGuasPar[1]-0.0025,ffirstGuas->GetXmax());
	ffirstCrystal->SetParameters(ffirstGuasPar[0],ffirstGuasPar[1],ffirstGuasPar[2],1.64,1.1615);
	momentum->Fit("ffirstCrystal","RQ0","ep",ffirstCrystal->GetXmin(),ffirstCrystal->GetXmax());
	ffirstCrystal->GetParameters(ffirstCrystalPar);
	//	ffirstCrystal->Draw("same");
	// fit together
	double_t fCrystalMomentumPar[10];
	TF1 *fCrystalMomentum=new TF1("fCrystalMomentum","crystalball(0)+crystalball(5)",ffirstCrystal->GetXmin(),fgroundCrystalball->GetXmax());
	std::copy(fgroundCrystalballPar,fgroundCrystalballPar+5,fCrystalMomentumPar);
	std::copy(ffirstCrystalPar,ffirstCrystalPar+5,fCrystalMomentumPar+5);
	fCrystalMomentum->SetParameters(fCrystalMomentumPar);
	momentum->Fit("fCrystalMomentum","R","",fCrystalMomentum->GetXmin(),fCrystalMomentum->GetXmax());
	fCrystalMomentum->Draw("same");
	fCrystalMomentum->GetParameters(fCrystalMomentumPar);



	SieveRecCanvas->Update();
	// plot the reconstrcution peak
	TLine *groudposLine=new TLine(fCrystalMomentumPar[1],0,fCrystalMomentumPar[1],fgroudGausPar[0]*1.1);
	groudposLine->SetLineColor(3);
	groudposLine->SetLineWidth(2);
	groudposLine->Draw("same");

	TLine *firstposLine=new TLine(fCrystalMomentumPar[6],0,fCrystalMomentumPar[6],ffirstGuasPar[0]*2.0);
	firstposLine->SetLineColor(3);
	firstposLine->SetLineWidth(2);
	firstposLine->Draw("same");

	TPaveText *pt = new TPaveText(0.1,0.8,0.6,0.9,"NDC");
	double_t deltaE=fCrystalMomentumPar[1]-fCrystalMomentumPar[6];
	double_t deltaErr=TMath::Sqrt( (fCrystalMomentum->GetParError(1))*(fCrystalMomentum->GetParError(1))+(fCrystalMomentum->GetParError(6))*(fCrystalMomentum->GetParError(6)));

//	if(HallProbHH->GetEntries()!=0)
	{
	    //Step-1 calculate the correction
        double HRSBPMCorrection=0.0;

	        //load the BPM information from the data file. bpm on target
	        TH1F *bpmXinforh=new TH1F(Form("BPM_X_on_targ"),Form("BPM_X_on_targ"),500,-5,5);
            TH1F *bpmYinforh=new TH1F(Form("BPM_Y_on_targ"),Form("BPM_Y_on_targ"),500,-5,5);
            bpmXinforh->GetXaxis()->SetTitle(Form("targx"));
            bpmYinforh->GetXaxis()->SetTitle(Form("targy"));
            chain->Project(bpmXinforh->GetName(),Form("targx"),Form("%s && %s",generalcut.Data(),cutg->GetName()));
            chain->Project(bpmYinforh->GetName(),Form("targy"),Form("%s && %s",generalcut.Data(),cutg->GetName()));

            // set the range, and do the fit on BPM
//            bpmXinforh->GetXaxis()->SetRangeUser(bpmXinforh->GetBinCenter(bpmXinforh->GetMaximumBin())-1,bpmXinforh->GetBinCenter(bpmXinforh->GetMaximumBin())+1);
//            bpmXinforh->Fit("gaus","","",bpmXinforh->GetBinCenter(bpmXinforh->GetMaximumBin())-1,bpmXinforh->GetBinCenter(bpmXinforh->GetMaximumBin())+1);

//            bpmYinforh->GetXaxis()->SetRangeUser(bpmYinforh->GetBinCenter(bpmYinforh->GetMaximumBin())-1,bpmYinforh->GetBinCenter(bpmYinforh->GetMaximumBin())+1);
//            bpmYinforh->Fit("gaus","","",bpmYinforh->GetBinCenter(bpmYinforh->GetMaximumBin())-1,bpmYinforh->GetBinCenter(bpmYinforh->GetMaximumBin())+1);

            double bpmX=bpmXinforh->GetMean();
            double bpmY=bpmYinforh->GetMean();


            double sieveX=82.46;
            double sieveY=-0.6;
            double sieveZ=993.03;
            if(HRS == "R"){
                sieveX=-82.66;
                sieveY=-0.46;
                sieveZ=993.26;
            }
            // calculate the target value, and project the data to the hall-0 position


            SieveRecCanvas->cd(3)->cd(1);
            TLine *xMeanLine=new TLine(bpmX,0,bpmX,bpmXinforh->GetMaximumBin());
            bpmXinforh->Draw();
            xMeanLine->Draw("same");
            SieveRecCanvas->cd(3)->cd(2);
            TLine *yMeanLine=new TLine(bpmY,0,bpmY,bpmYinforh->GetMaximumBin());
            bpmYinforh->Draw();
            yMeanLine->Draw("same");

            // calculate the angle
            HRSBPMCorrection=TMath::ATan((bpmX)/sieveZ) * 180.0 / TMath::Pi();
            std::cout<<"Correction angle::"<<HRSBPMCorrection<<std::endl;

            SieveRecCanvas->cd(3)->cd(3);
            TString imgfname=Form("/home/newdriver/Learning/GeneralScripts/halog/result/BeamE%d.jpg",eventID);
            if(!gSystem->AccessPathName(imgfname.Data())){
                TImage *img=TImage::Open(imgfname.Data());
                img->Draw();
            }

            // get the equantion and the equation that used for calculate the correction angle
            SieveRecCanvas->cd(3)->cd(4);

            TPaveText *infor=new TPaveText(0.1,0.8,0.6,0.9,"NDC");
            infor->AddText(Form("Bpm on Targ x.y/cm:(%f,%f)",bpmX,bpmY));
            infor->AddText(Form("Targ to Sieve on Beamline:%f",sieveZ));
            infor->AddText(Form("Atan(#frac{%f}{%f})=%f (%f#circ)",bpmX,sieveZ,TMath::ATan((bpmX)/sieveZ),HRSBPMCorrection));
            infor->Draw("same");

        SieveRecCanvas->cd(1);
		//calculate the Dp with +/x 10^-4 change
		//pt->AddText(Form("#DeltaDp +2*10^{-4}:%1.3f MeV (%1.4f Degree)",1000.0*deltaE+0.0002*CentralP*1000.0,GetPointingAngle(deltaE+0.0002*CentralP)));
		//pt->AddText(Form("#DeltaDp +1*10^{-4}:%1.3f MeV (%1.4f Degree)",1000.0*deltaE+0.0001*CentralP*1000.0,GetPointingAngle(deltaE+0.0001*CentralP)));
		// get the error
		double DeltaEMax=deltaE+deltaErr;
		double DeltaE200=deltaE+200.0/1000000.0;
		double DeltaEMin=deltaE-deltaErr;
		double AngleMax=GetPointingAngle(DeltaEMax,getBeamE(eventID,chain));
		double AngleMin=GetPointingAngle(DeltaEMin,getBeamE(eventID,chain));
		double Angletemp=GetPointingAngle(deltaE,getBeamE(eventID,chain));
		double errorAngle=0;
		if(abs(AngleMax-Angletemp) >abs(Angletemp-AngleMin) ){
			errorAngle=abs(AngleMax-Angletemp);
		}else{
			errorAngle=abs(AngleMin-Angletemp);
		}

		double errorAngle1=abs(GetPointingAngle(DeltaE200,getBeamE(eventID,chain)))-Angletemp;
		//add the correction
		double HRSAngle=GetPointingAngle(deltaE,getBeamE(eventID,chain));

		pt->AddText(Form("#DeltaDp :%1.3f MeV (%1.3f#pm%1.3f#pm%1.3f Degree)",1000.0*deltaE,HRSAngle,errorAngle,errorAngle1));
//        pt->AddText(Form("#DeltaDp :%1.3f MeV (%1.3f#pm%1.3f#pm%1.3f Degree)",1000.0*deltaE,HRSAngle-HRSBPMCorrection,errorAngle,errorAngle1));
        pt->AddText(Form("BPM x::%1.5f  , BPM y::%1.5f",bpmX,bpmY));

		//pt->AddText(Form("#DeltaDp -1*10^{-4}:%1.3f MeV (%1.4f Degree)",1000.0*deltaE-0.0001*CentralP*1000.0,GetPointingAngle(deltaE-0.0001*CentralP)));
		//pt->AddText(Form("#DeltaDp -2*10^{-4}:%1.3f MeV (%1.4f Degree)",1000.0*deltaE-0.0002*CentralP*1000.0,GetPointingAngle(deltaE-0.0002*CentralP)));
		//pt->AddText("CentalP : HallProb * 0.95282/0.33930");

		//calculate the Dp with +/x 10^-4 change
		//pt->AddText(Form("#DeltaDp +2*10^{-4}:%1.3f MeV (%1.4f Degree)",1000.0*deltaE+0.0002*CentralP*1000.0,GetPointingAngle(deltaE+0.0002*CentralP)));
		//pt->AddText(Form("#DeltaDp +1*10^{-4}:%1.3f MeV (%1.4f Degree)",1000.0*deltaE+0.0001*CentralP*1000.0,GetPointingAngle(deltaE+0.0001*CentralP)));
        //pt->AddText(Form("#DeltaDp  0*10^{-4}:%1.3f MeV (%1.4f Degree)",1000.0*deltaE,GetPointingAngle(deltaE,getBeamE(eventID,chain))));
		//pt->AddText(Form("#DeltaDp -1*10^{-4}:%1.3f MeV (%1.4f Degree)",1000.0*deltaE-0.0001*CentralP*1000.0,GetPointingAngle(deltaE-0.0001*CentralP)));
		//pt->AddText(Form("#DeltaDp -2*10^{-4}:%1.3f MeV (%1.4f Degree)",1000.0*deltaE-0.0002*CentralP*1000.0,GetPointingAngle(deltaE-0.0002*CentralP)));
		//pt->AddText("CentalP : HallProb * 0.95282/0.33930");

	}
//	else{
//		pt->AddText(Form("#DeltaDp  0*10^{-4}:%1.3f MeV (%1.4f Degree)",1000.0*deltaE,GetPointingAngle(deltaE)));
//	}
	pt->Draw("same");

	TLatex *t1 = new TLatex(fgroudGausPar[1] + 2 * fgroudGausPar[2],fgroudGausPar[0], Form("P=%2.5fGeV #pm %1.3fMeV", fCrystalMomentumPar[1],1000.0*(fCrystalMomentum->GetParError(1))));
	t1->SetTextSize(0.055);
	t1->SetTextAlign(12);
	t1->SetTextColor(2);
	t1->Draw("same");

	TLatex *t2 = new TLatex(ffirstGuasPar[1]+ffirstGuasPar[2]*0.5,fCrystalMomentumPar[5], Form("P=%2.5fGeV #pm %1.3fMeV", fCrystalMomentumPar[6],1000.0*(fCrystalMomentum->GetParError(6))));
	t2->SetTextSize(0.055);
	t2->SetTextAlign(12);
	t2->SetTextColor(2);
	t2->Draw("same");

	TLatex *t3 = new TLatex((ffirstGuasPar[1]+fgroudGausPar[1])/2.0,(fCrystalMomentumPar[5]+fgroudGausPar[0])*0.5, Form("#DeltaP=%2.3f #pm %1.3fMeV", 1000.0*deltaE,1000.0*deltaErr));
	t3->SetTextSize(0.055);
	t3->SetTextAlign(12);
	t3->SetTextColor(2);
	t3->Draw("same");



	//plot the bigger plot for first excited states
	SieveRecCanvas->cd(2)->cd(3);
	TH1F *groundStats=(TH1F *)momentum->Clone("H2O p.ground");
	groundStats->GetXaxis()->SetRangeUser(fCrystalMomentumPar[1]-0.002,fCrystalMomentumPar[1]+0.002);
	groundStats->Draw();
	groudposLine->Draw("same");

	SieveRecCanvas->cd(2)->cd(4);
	TH1F *firstStats=(TH1F *)momentum->Clone("H2O p.first");
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
	SieveRecCanvas->SaveAs(Form("./PointingCheck/result/Pointing_%d.root",eventID));
	hSieveHole->Delete();
	cutg->Delete();
}

