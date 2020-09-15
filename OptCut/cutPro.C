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
#include <sys/stat.h>
#include <sstream>
#include <TComplex.h>
#include <TVirtualPad.h>

#include <TSpectrum2.h>
#include <TF2.h>
#include <TObject.h>
#include "TMinuit.h"
#include <TFile.h>
#include <fstream>
#include <TSystem.h>
#include <TApplication.h>
#include <TLatex.h>
#include <TGApplication.h>

// used for create the folder if does not exist in the destintion folder
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

//CRex C-12
TString generalcutR="R.tr.n==1 ";//&& R.vdc.u1.nclust==1&& R.vdc.v1.nclust==1 && R.vdc.u2.nclust==1 && R.vdc.v2.nclust==1 && R.gold.dp<1 && R.gold.dp > -0.1 && fEvtHdr.fEvtType==1";
TString generalcutL="L.tr.n==1 && L.vdc.u1.nclust==1&& L.vdc.v1.nclust==1 && L.vdc.u2.nclust==1 && L.vdc.v2.nclust==1 && L.gold.dp<1 && L.gold.dp > -0.1 && fEvtHdr.fEvtType==1";

// CRex Water
//TString generalcutR="R.tr.n==1 && R.vdc.u1.nclust==1&& R.vdc.v1.nclust==1 && R.vdc.u2.nclust==1 && R.vdc.v2.nclust==1 && fEvtHdr.fEvtType==1 && R.gold.p > 2.14 && R.gold.p < 2.2";
//TString generalcutL="L.tr.n==1 && L.vdc.u1.nclust==1&& L.vdc.v1.nclust==1 && L.vdc.u2.nclust==1 && L.vdc.v2.nclust==1 && L.gold.p > 2.14 && L.gold.p < 2.19";

//////////////////////////////////////////////////////////////////////////////
// Work Directory
//////////////////////////////////////////////////////////////////////////////
//TString WorkDir = "/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Cut20200311/RHRS/";
//TString WorkDir = "/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Cut20200322/LHRS/";
//TString WorkDir = "/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Cut20200413/RHRS/";
//TString WorkDir = "/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Cut20200526/RHRS/";
//TString WorkDir = "/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Cut20200530/RHRS/";
//TString WorkDir = "/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Cut20200701/";
//TString WorkDir = "/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Cut20200701/water";
TString WorkDir = "/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Cut20200915";

TString CutSuf = ".FullCut.root";
TString CutDescFileSufVertex = ".VertexCut.cut";
TString CutDescFileSufDp = ".DpCut.cut";
TString CutDescFileSufSieve = ".SieveCut.%d_%d.cut";
TString RootFileName;


//LHRS
int numberofSieveHoles[13]={0,0,0,5,6,5,5,6,5,5,4,3,2};
int minSieveHoles[13]=     {0,0,0,1,0,1,1,0,1,1,1,2,2};


//RHRS
//int numberofSieveHoles[13]={0,0,0,6,6,5,5,6,5,5,4,3,2};
//int minSieveHoles[13]=     {0,0,0,0,0,1,1,0,1,1,1,2,2};


inline Bool_t IsFileExist (const std::string& name) {
	  struct stat buffer;
	  return (stat (name.c_str(), &buffer) == 0);
}


// check which experiment and which HRS and load its configure
// experiment: PRex/CRex
// HRS: L/R
void ExperimentConfigure(TString experiment, TString HRS){


}

Int_t cutPro(UInt_t runID,UInt_t current_col=3,TString folder="/home/newdriver/Storage/Research/CRex_Experiment/RasterReplay/Replay/Result/") {
	// need to check the folder
	std::string bufferedWorkFolder;
	std::string bufferedSourceDir;
	int bufferedCol=-1;
	int bufferedRunID=-1;

	std::string cutrunProfname=Form("%s/logfile.txt",WorkDir.Data());
	if(!boost::filesystem::is_regular_file(cutrunProfname.c_str())){
		bufferedSourceDir=folder;
		bufferedWorkFolder=WorkDir;
		bufferedCol=3;
		bufferedRunID=runID;
		// create the folder and save the infor
	}else{
		std::ifstream textinfile(cutrunProfname.c_str());
		textinfile>>bufferedSourceDir>>bufferedWorkFolder>>bufferedRunID>>bufferedCol;

		if((bufferedSourceDir==folder)&&(bufferedWorkFolder==WorkDir)&&(bufferedRunID==runID)){
			bufferedCol++;
		}else{
			bufferedSourceDir=folder;
			bufferedWorkFolder=WorkDir;
			bufferedRunID=runID;
			bufferedCol=3;
		}
		std::cout<<"source dir:"<<bufferedSourceDir.c_str()<<std::endl;
		std::cout<<"current work dir: " << bufferedWorkFolder.c_str()<<"\n  runID:"<<bufferedRunID<<"\n  current col: "<< bufferedCol<<std::endl;
		// update the run infor
	}

	std::ofstream textoutfile;
	textoutfile.open(cutrunProfname.c_str(), std::ios::trunc);
	textoutfile <<bufferedSourceDir.c_str()<<" "<<bufferedWorkFolder.c_str()<< " "<<bufferedRunID<<" "<<bufferedCol << std::endl;

	current_col=bufferedCol;


	gStyle->SetOptStat(0);
	// prepare the data
	TChain *chain=new TChain("T");
	TString rootDir(folder.Data());
	TString HRS="R";
	if(runID>20000){ //RHRS
		if(IsFileExist(Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID))){
			std::cout<<"Add File::"<<Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
			RootFileName=Form("prexRHRS_%d_-1.root",runID);
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
			RootFileName=Form("prexLHRS_%d_-1.root",runID);
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

	if(HRS=="L"){
		generalcut=generalcutL;
	}else{
		generalcut=generalcutR;
	}

	TCanvas *mainPatternCanvas=(TCanvas *)gROOT->GetListOfCanvases()->FindObject("cutPro");
	if(!mainPatternCanvas){
		mainPatternCanvas=new TCanvas("cutPro","cutPro",1000,1200);
	}else{
		mainPatternCanvas->Clear();
	}
	//	TCanvas *mainPatternCanvas=new TCanvas("cut","cut",600,600);
	mainPatternCanvas->Draw();
	TH2F *TargetThPhHH=(TH2F *)gROOT->FindObject("th_vs_ph");
	if(TargetThPhHH) TargetThPhHH->Delete();
	TargetThPhHH=new TH2F("th_vs_ph","th_vs_ph",1000,-0.025,0.025,1000,-0.047,0.05);

	chain->Project(TargetThPhHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),generalcut.Data());
	TargetThPhHH->Draw("zcol");
	mainPatternCanvas->SetGridx(10);
	mainPatternCanvas->SetGridy(10);

//	mainPatternCanvas->Update();
	// input how start row and how many holes in this row
	col=current_col;
    int nhol = 0;
    std::cout << "How many holes in this No." << col << " column?" << std::endl;
//    std::cin >> nhol;
    nhol=numberofSieveHoles[col];
    row_count=nhol;
    std::cout<<numberofSieveHoles[col]<<std::endl;
//    row_count=numberofSieveHoles[col];
    if(nhol < 0)return 0;
    std::cout << "min hole id : ";
    int rmin = -1;
//  std::cin >> rmin;
    rmin=minSieveHoles[col];
    row_min=rmin;
    std::cout<<minSieveHoles[col]<<std::endl;
    row=row_min;

    if(rmin < 0)return 0;
    TLatex *p=new TLatex(-0.025, 0.045,Form("Sieve %d, #frac{%d}{%d}",col,rmin,nhol));
    p->Draw("same");
    mainPatternCanvas->Update();
	mainPatternCanvas->ToggleEventStatus();
	mainPatternCanvas->AddExec("ex", "DynamicCanvas()");
	return 1;
}

//Recognize the save patter
// save the rec hole to the folder
// ground state
void SavePatternHole(double momentumSigmaCut=3.0){
	//search all the holes in this col and save in the folder
	std::cout<<std::endl<<std::endl;
	std::cout<<"*******Save process start ........*******"<<std::endl;
	std::cout<<"	Searching for holes in col ("<<col<<")"<<std::endl;

	TCanvas *SaveCheckCanvas=(TCanvas *) gROOT->GetListOfCanvases()->FindObject("SieveSaveCheck");
	if(!SaveCheckCanvas){
		SaveCheckCanvas =new TCanvas("SieveSaveCheck","SieveSaveCheck",1600,1080);
	}else{
		SaveCheckCanvas->Clear();
	}
	SaveCheckCanvas->Divide(1,2);
	SaveCheckCanvas->cd(1)->Divide(2,1);
	SaveCheckCanvas->cd(2)->Divide(row_count,1);

	TString workdir_temp=WorkDir;
	if(momentumSigmaCut>10.0){
		workdir_temp+="/WithOutMomCut/";
	}else{
		workdir_temp+="/GroundMomCut/";
	}
	// check the existance of the folder, if not create the folder
	if(!boost::filesystem::is_directory(workdir_temp.Data())){
		std::cout<<"Folder :"<< workdir_temp.Data()<<"  Does Not EXIST\n Trying to Create the folder"<<std::endl;
		if(!boost::filesystem::create_directories(workdir_temp.Data())){
			std::cout<<"ERROR:: cannot creat folde, please check the permission!!!!"<<std::endl;
			exit(-1);
		}
	}

	TString CutFileName = workdir_temp + RootFileName + CutSuf;
	TString TempString(Form(CutDescFileSufSieve.Data(), FoilID, col));
	TString PlotDir(RootFileName + Form(".hcut_R_%d_%d/", FoilID, col));
	TString CutDescName = workdir_temp + RootFileName + TempString;
	// prepare the filename and folder
	//TString CutDescName = WorkDir + RootFileName + CutDescFileSufDp;
	//TString CutFileName = WorkDir + RootFileName + CutSuf; // used for save the root file and cut file
	TFile *f1=new TFile(CutFileName,"UPDATE");
	assert(f1);

	std::fstream cutdesc(CutDescName, std::ios_base::out);
	assert(cutdesc.is_open());

	SaveCheckCanvas->cd(1)->cd(1);
	// attached the root file and plot the canvas used for check
	TChain *chain = (TChain *) gROOT->FindObject("T");
	if(!chain) std::cout<<"[ERROR] CAN NOT FIND TREE"<<std::endl;
	TH2F *h=(TH2F*)gROOT->FindObject("th_vs_ph");
	if(!h)
		h=new TH2F("th_vs_ph1","th_vs_ph1",1000,-0.03,0.03,1000,-0.045,0.05);

	TH2F *SavesieveCheck = new TH2F("SaveSieveCheck", "SaveSieveCheck",
			h->GetXaxis()->GetNbins(), h->GetXaxis()->GetXmin(),
			h->GetXaxis()->GetXmax(), h->GetYaxis()->GetNbins(),
			h->GetYaxis()->GetXmin(), h->GetYaxis()->GetXmax());

	TString HRS("R");
	TString filename(chain->GetFile()->GetName());
	if (filename.Contains("RHRS")) {
	} else if (filename.Contains("LHRS")) {
		HRS = "L";
	}
	chain->Project(SavesieveCheck->GetName(),Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),Form("%s",generalcut.Data()));
	//chain->Draw(Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),Form("%s",generalcut.Data()),"zcol");
	SavesieveCheck->Draw("zcol");
	// search how many cut file exist in the root buffer
	for (int i = 0 ; i < row_min; i ++)cutdesc<<"fEvtHdr.fRun==0"<< std::endl;
	std::vector<int> sieveIDList;
	TH2F *sievehole[row_count];
	TH1F *sieveholemomentum[row_count];    // ground states momentum
	TF1  *sieveholemomentumGausFit[row_count];
	TLatex * momentumInfor1[row_count];
	TCutG *cutg;
	for (int row_iter = row_min; row_iter<row_min+row_count;row_iter++){

		cutg=(TCutG *) gROOT->FindObject(Form("hcut_R_%d_%d_%d",FoilID,col,row_iter));
		if(cutg){
			sieveIDList.push_back(row_iter-row_min);
			sievehole[row_iter-row_min] = new TH2F(
					Form("hcut_R_%d_%d_%d_hh", FoilID, col, row_iter),
					Form("hcut_R_%d_%d_%d_hh", FoilID, col, row_iter),
					h->GetXaxis()->GetNbins(), h->GetXaxis()->GetXmin(),
					h->GetXaxis()->GetXmax(), h->GetYaxis()->GetNbins(),
					h->GetYaxis()->GetXmin(), h->GetYaxis()->GetXmax());
			SaveCheckCanvas->cd(2)->cd(row_iter - row_min + 1);
			chain->Project(sievehole[row_iter-row_min]->GetName(), Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),Form("%s && %s",cutg->GetName(),generalcut.Data()));

			sieveholemomentum[row_iter-row_min]=new TH1F(Form("hcut_R_%d_%d_%d_h_momentum", FoilID, col, row_iter),Form("hcut_R_%d_%d_%d_momentum", FoilID, col, row_iter),600,2.1,2.25);
			chain->Project(sieveholemomentum[row_iter-row_min]->GetName(),Form("%s.gold.p",HRS.Data()),Form("%s && %s",cutg->GetName(),generalcut.Data()));
			sieveholemomentumGausFit[row_iter-row_min]=new TF1(Form("1ststatesDpgaushcut_R_%d_%d_%d", FoilID, col, row_iter),"gaus",
					sieveholemomentum[row_iter-row_min]->GetXaxis()->GetBinCenter(sieveholemomentum[row_iter-row_min]->GetMaximumBin())-0.002,
					sieveholemomentum[row_iter-row_min]->GetXaxis()->GetBinCenter(sieveholemomentum[row_iter-row_min]->GetMaximumBin())+0.002);
			sieveholemomentumGausFit[row_iter-row_min]->SetParameter(1,sieveholemomentum[row_iter-row_min]->GetXaxis()->GetBinCenter(sieveholemomentum[row_iter-row_min]->GetMaximumBin()));
			sieveholemomentum[row_iter - row_min]->Fit(
					Form("1ststatesDpgaushcut_R_%d_%d_%d", FoilID, col,
							row_iter), "R", "ep",
					sieveholemomentumGausFit[row_iter-row_min]->GetXmin(),
					sieveholemomentumGausFit[row_iter-row_min]->GetXmax());

			sieveholemomentum[row_iter - row_min]->GetXaxis()->SetRangeUser(
					sieveholemomentum[row_iter - row_min]->GetXaxis()->GetBinCenter(
							sieveholemomentum[row_iter - row_min]->GetMaximumBin())
							- 0.009,
					sieveholemomentum[row_iter - row_min]->GetXaxis()->GetBinCenter(
							sieveholemomentum[row_iter - row_min]->GetMaximumBin())
							+ 0.004);
			sieveholemomentum[row_iter - row_min]->Draw();
			sieveholemomentumGausFit[row_iter-row_min]->Draw("same");

			auto groudpcenter=sieveholemomentumGausFit[row_iter-row_min]->GetParameter(1);
			auto groudpsigma=sieveholemomentumGausFit[row_iter-row_min]->GetParameter(2);

			momentumInfor1[row_iter - row_min] =
					new TLatex(groudpcenter + 2 * groudpsigma-0.007,
							sieveholemomentumGausFit[row_iter - row_min]->GetParameter(0),
							Form("P_{0}= %2.5f",
									groudpcenter));//, groudpsigma
			momentumInfor1[row_iter - row_min]->SetTextSize(0.055);
			momentumInfor1[row_iter - row_min]->SetTextAlign(12);
			momentumInfor1[row_iter - row_min]->SetTextColor(2);
			momentumInfor1[row_iter - row_min]->Draw("same");

			SaveCheckCanvas->cd(2)->cd(row_iter - row_min + 1)->Update();
			if(groudpsigma>0.0008)groudpsigma=0.0008;
			// plot the boundary of the cut
			TLine *leftboundary=new TLine(groudpcenter-momentumSigmaCut*groudpsigma,0,groudpcenter-momentumSigmaCut*groudpsigma,(0.9*SaveCheckCanvas->cd(2)->cd(row_iter - row_min + 1)->GetUymax()));
			leftboundary->SetLineColor(3);
			leftboundary->SetLineWidth(2);
			leftboundary->Draw("same");

			TLine *rightboundary=new TLine(groudpcenter+momentumSigmaCut*groudpsigma,0,groudpcenter+momentumSigmaCut*groudpsigma,(0.9*SaveCheckCanvas->cd(2)->cd(row_iter - row_min + 1)->GetUymax()));
			rightboundary->SetLineColor(3);
			rightboundary->SetLineWidth(2);
			rightboundary->Draw("same");

			SaveCheckCanvas->cd(1)->cd(1);
			cutg->SetName(Form("hcut_R_%d_%d_%d", FoilID, col, row_iter));
			cutg->SetVarX(Form("%s.gold.ph",HRS.Data()));
			cutg->SetVarY(Form("%s.gold.th",HRS.Data()));
			cutg->SetLineColor(kMagenta);
			cutg->SetLineWidth(2);
			cutg->Draw("PL same");
			SaveCheckCanvas->cd(1)->cd(1)->SetGridx(20);
			SaveCheckCanvas->cd(1)->cd(1)->SetGridy(20);
			//plot the momentum and apply cut on the momentum

			cutg->Write("", TObject::kOverwrite); // Overwrite old cut
			if(groudpsigma>0.0008)groudpsigma=0.0008;
			cutdesc << Form("hcut_R_%d_%d_%d", FoilID, col, row_iter) << " && ";

			if(momentumSigmaCut>10.0){
				cutdesc << (const char*)generalcut << std::endl;
			}else{
			cutdesc << (const char*)generalcut <<" && "
					<<Form("abs(%s.gold.p-%f)<%f*%f",HRS.Data(),groudpcenter,momentumSigmaCut,groudpsigma)
					<< std::endl;
			}

			SaveCheckCanvas->cd(1)->cd(2);
			sievehole[row_iter-row_min]->Draw("same");
			TLatex *eventCountLable=new TLatex(sievehole[row_iter-row_min]->GetMean(1) + 0.005,sievehole[row_iter-row_min]->GetMean(2), Form("Entries(%d,%d): %2.0f",col,row_iter, (sievehole[row_iter-row_min]->GetEntries())));
			eventCountLable->SetTextSize(0.03);
			eventCountLable->SetTextAlign(12);
			eventCountLable->SetTextColor(2);
			eventCountLable->Draw("same");
			SaveCheckCanvas->cd(1)->cd(2)->SetGridx(20);
			SaveCheckCanvas->cd(1)->cd(2)->SetGridy(20);
			//sievehole[row_iter-row_min]->Delete();
			cutg->Draw("same");
			SaveCheckCanvas->Update();

		}else{
			//if the cut does not exist,then write the cut
			cutdesc << "fEvtHdr.fRun==0" << std::endl;
		}

	}

	for(int i = row_min+row_count; i < NSieveRow; i++)
		cutdesc << "fEvtHdr.fRun==0" << std::endl;
	SaveCheckCanvas->SetName(Form("CutProfcut_R_%d_%d",FoilID, col));
	SaveCheckCanvas->Write("", TObject::kOverwrite);
	SaveCheckCanvas->SaveAs(Form("%s/%s.hcut_R_%d_%d.jpg",workdir_temp.Data(),RootFileName.Data(),FoilID, col));

	for(auto i : sieveIDList){
//	for (unsigned int i = 0; i < row_count; i++) {
		if (!sievehole[i]->IsZombie()) {
			sievehole[i]->Delete();
		}
		if (!sieveholemomentum[i]->IsZombie()) {
			sieveholemomentum[i]->Delete();
		}
		if (!sieveholemomentumGausFit[i]->IsZombie()) {
			sieveholemomentumGausFit[i]->Delete();
		}

	}
	if(!SavesieveCheck->IsZombie()){
		SavesieveCheck->Delete();
	}
	if(!h->IsZombie()){
		h->Delete();
	}

	f1->Write();
	f1->ls();
	f1->Close();
	cutdesc.close();
}


inline  int16_t getUID(UInt_t KineID,UInt_t Col, UInt_t Row){
	// get the uni-id for the sieveholes
	return KineID*98+Col*NSieveRow+Row;
}


// take the cut file and  the root file as input, and generate the average value the parameters on the focal plane
Int_t OpticsFocalAverageGenerator(UInt_t runID,UInt_t KineID,
		TString cutFile =
				"/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Cut20200701/water/WithOutMomCut",
		TString folder =
				"/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/Tools/PlotCut/Result/Cut20200701/rootfiles") {

	TFile *rootFileIO=new TFile(Form("./FinalData/OpticsFocalDiagnose_run%d.root",runID),"recreate");
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
		mainPatternCanvas=new TCanvas("cutPro","cutPro",1960,1080);
	}else{
		mainPatternCanvas->Clear();
	}

	mainPatternCanvas->Divide(1,3);
	mainPatternCanvas->cd(1)->Divide(3,1);
	mainPatternCanvas->cd(2)->Divide(4,1);
	mainPatternCanvas->cd(3)->Divide(4,1);
	mainPatternCanvas->Draw();
	mainPatternCanvas->cd(1)->cd(1);

	TH2F *TargetThPhHH=(TH2F *)gROOT->FindObject("th_vs_ph");
	if(TargetThPhHH) TargetThPhHH->Delete();
	TargetThPhHH=new TH2F("th_vs_ph","th_vs_ph",1000,-0.03,0.03,1000,-0.045,0.05);

	chain->Project(TargetThPhHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),generalcut.Data());
	TargetThPhHH->Draw("zcol");

	// read the cut file and load the cut, create the new data set with the cut
	// load the cut file

	// searching for the cut file
	//if the cut file is the path pointing to the cut file, it will automaticly searching for the cut file according to name rule
	if(!cutFile.EndsWith(".root")){
		if(HRS == 'R'){
			cutFile=Form("%s/prexRHRS_%d_-1.root.FullCut.root",cutFile.Data(),runID);
		}else{
			cutFile=Form("%s/prexLHRS_%d_-1.root.FullCut.root",cutFile.Data(),runID);
		}
	}else{
		cutFile=Form("%s",cutFile.Data());
	}


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
	//merge all the sieve holes cut together
	sieveAllHoleCut=sieveAllHoleCut+TCut(generalcut.Data());
	mainPatternCanvas->cd(1)->cd(2);
	TH2F *TargetThPh_SieveCutHH=(TH2F *)gROOT->FindObject("th_vs_ph_cut");
	if(TargetThPh_SieveCutHH) TargetThPh_SieveCutHH->Delete();
	TargetThPh_SieveCutHH=new TH2F("th_vs_ph_cut","th_vs_ph_cut",1000,-0.03,0.03,1000,-0.045,0.05);
	chain->Project(TargetThPh_SieveCutHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),sieveAllHoleCut);
	TargetThPh_SieveCutHH->Draw("zcol");
	mainPatternCanvas->Update();


	// generate the sieve holes

	std::ofstream f51OutPut;
	f51OutPut.open(Form("./Sieve._%d_cut.f51",runID));

	// start analysis the cut informations
	std::map<int, std::map<int, TH1F *>> focalThetah;
	std::map<int, std::map<int, TH1F *>> focalXh;
	std::map<int, std::map<int, TH1F *>> focalPhih;
	std::map<int, std::map<int, TH1F *>> focalYh;
	std::map<int, std::map<int, TH1F *>> bpmXinforh;
	std::map<int, std::map<int, TH1F *>> bpmYinforh;
	std::map<int, std::map<int, TH1F *>> sieveholemomentum;
	TH2F *SieveThetaPhiCuthh;
	for (int16_t col = 0; col < NSieveCol; col++) {
		for (int16_t row = 0; row < NSieveRow; row++) {
			// get the informations
			//focal plane x, focal plane Y , focal plane theta, focal plane Y
			//kCutID, kx, ky, kTh, KPHi, Kurb_e, kbeamX, kbeamY, kBeamvz
			auto cutg=(TCutG*)gROOT->FindObject(Form("hcut_R_%d_%d_%d", FoilID, col, row));
			if(cutg)
			if (cutg){
				double  uid=0, focal_x=0, focal_y=0, focal_th=0,focal_ph=0;
				double  bpmX=0, bpmY=0;

				const double sieveMomCutSigma=3.0;


				TCut sieveMomCut(Form("%s && %s",(Form("hcut_R_%d_%d_%d", FoilID, col, row)),generalcut.Data()));
				// need cut on the Ground states
				mainPatternCanvas->cd(1)->cd(3);
				sieveholemomentum[col][row]=new TH1F(Form("hcut_R_%d_%d_%d_h_momentum", FoilID, col, row),Form("hcut_R_%d_%d_%d_momentum", FoilID, col, row),600,2.1,2.25);
				chain->Project(sieveholemomentum[col][row]->GetName(),Form("%s.gold.p",HRS.Data()),sieveMomCut);
				sieveholemomentum[col][row]->GetXaxis()->SetRangeUser(
						sieveholemomentum[col][row]->GetXaxis()->GetBinCenter(
								sieveholemomentum[col][row]->GetMaximumBin())
											- 0.009,
											sieveholemomentum[col][row]->GetXaxis()->GetBinCenter(
													sieveholemomentum[col][row]->GetMaximumBin())
											+ 0.004);
				sieveholemomentum[col][row]->Fit("gaus","","",sieveholemomentum[col][row]->GetBinCenter(sieveholemomentum[col][row]->GetMaximumBin())-0.002,sieveholemomentum[col][row]->GetBinCenter(sieveholemomentum[col][row]->GetMaximumBin())+0.002);
				double SieveMomFitPar[3];
				sieveholemomentum[col][row]->GetFunction("gaus")->GetParameters(SieveMomFitPar);
				sieveholemomentum[col][row]->Draw();


				TString sieveGroundPcut(Form("abs(%s.gold.p-%f)<%f*%f",HRS.Data(),SieveMomFitPar[1],sieveMomCutSigma,SieveMomFitPar[2]));

				TLine momLineCenter(SieveMomFitPar[1],0,SieveMomFitPar[1],SieveMomFitPar[0]);
				momLineCenter.SetLineColor(kRed);
				momLineCenter.Draw("same");

				TLine momLineMin(SieveMomFitPar[1]-sieveMomCutSigma*SieveMomFitPar[2],0,SieveMomFitPar[1]-sieveMomCutSigma*SieveMomFitPar[2],SieveMomFitPar[0]);
				momLineMin.SetLineColor(kGreen);
				momLineMin.Draw("same");

				TLine momLineMax(SieveMomFitPar[1]+sieveMomCutSigma*SieveMomFitPar[2],0,SieveMomFitPar[1]+sieveMomCutSigma*SieveMomFitPar[2],SieveMomFitPar[0]);
				momLineMax.SetLineColor(kGreen);
				momLineMax.Draw("same");


				uid=getUID(KineID,col,row);
				// project the Beam informations

				focalXh[col][row]=new TH1F(Form("%s%d_focalX_kineID%d_col%d_row%d",HRS.Data(),runID,KineID,col,row),Form("%s%d_focalX_kineID%d_col%d_row%d",HRS.Data(),runID,KineID,col,row),
						2000,-1,1);
				focalThetah[col][row]=new TH1F(Form("%s%d_focalTheta_kineID%d_col%d_row%d",HRS.Data(),runID,KineID,col,row),Form("%s%d_focalTheta_kineID%d_col%d_row%d",HRS.Data(),runID,KineID,col,row),
										2000,-0.3,0.3);
				focalYh[col][row]=new TH1F(Form("%s%d_focalY_kineID%d_col%d_row%d",HRS.Data(),runID,KineID,col,row),Form("%s%d_focalY_kineID%d_col%d_row%d",HRS.Data(),runID,KineID,col,row),
										2000,-0.2,0.2);
				focalPhih[col][row]=new TH1F(Form("%s%d_focalPhi_kineID%d_col%d_row%d",HRS.Data(),runID,KineID,col,row),Form("%s%d_focalPhi_kineID%d_col%d_row%d",HRS.Data(),runID,KineID,col,row),
										1000,-0.1,0.1);

				// project the parameters
				bpmXinforh[col][row]=new TH1F(Form("%s%d_bpmX_kineID%d_col%d_row%d",HRS.Data(),runID,KineID,col,row),Form("%s%d_bpmX_kineID%d_col%d_row%d",HRS.Data(),runID,KineID,col,row),
						500,-5,5);
				bpmYinforh[col][row]=new TH1F(Form("%s%d_bpmY_kineID%d_col%d_row%d",HRS.Data(),runID,KineID,col,row),Form("%s%d_bpmY_kineID%d_col%d_row%d",HRS.Data(),runID,KineID,col,row),
										500,-5,5);


				TCut sieveHoleCut(Form("%s && %s && %s",(Form("hcut_R_%d_%d_%d", FoilID, col, row)),generalcut.Data(),sieveGroundPcut.Data()));
				//extract the informations
				sieveCut[col][row]->SetLineWidth(2);
				sieveCut[col][row]->SetLineColor(kGreen);
				mainPatternCanvas->cd(1)->cd(2);
				sieveCut[col][row]->Draw("same");

				// get the focal plane variables
				mainPatternCanvas->cd(2)->cd(1);
				chain->Project(focalXh[col][row]->GetName(),Form("%s.tr.r_x",HRS.Data()),sieveHoleCut);
				focalXh[col][row]->GetXaxis()->SetRangeUser(focalXh[col][row]->GetBinCenter(focalXh[col][row]->GetMaximumBin())-0.05,focalXh[col][row]->GetBinCenter(focalXh[col][row]->GetMaximumBin())+0.03);
				focalXh[col][row]->Fit("gaus","","",focalXh[col][row]->GetBinCenter(focalXh[col][row]->GetMaximumBin())-0.01,focalXh[col][row]->GetBinCenter(focalXh[col][row]->GetMaximumBin())+0.01);
				focalXh[col][row]->Draw();
				focal_x=focalXh[col][row]->GetFunction("gaus")->GetParameter(1);

				// get the Y parameter and start the fit
				mainPatternCanvas->cd(2)->cd(2);
				chain->Project(focalThetah[col][row]->GetName(),Form("%s.tr.r_th",HRS.Data()),sieveHoleCut);

				focalThetah[col][row]->GetXaxis()->SetRangeUser(focalThetah[col][row]->GetBinCenter(focalThetah[col][row]->GetMaximumBin())-0.015,focalThetah[col][row]->GetBinCenter(focalThetah[col][row]->GetMaximumBin())+0.015);
				focalThetah[col][row]->Fit("gaus","","",focalThetah[col][row]->GetBinCenter(focalThetah[col][row]->GetMaximumBin())-0.0020,focalThetah[col][row]->GetBinCenter(focalThetah[col][row]->GetMaximumBin())+0.002);
				focalThetah[col][row]->Draw();
				focal_th=focalThetah[col][row]->GetFunction("gaus")->GetParameter(1);


				// creat ethe focal plane Y and phi variables

				mainPatternCanvas->cd(2)->cd(3);
				chain->Project(focalYh[col][row]->GetName(),Form("%s.tr.r_y",HRS.Data()),sieveHoleCut);
				focalYh[col][row]->GetXaxis()->SetRangeUser(focalYh[col][row]->GetBinCenter(focalYh[col][row]->GetMaximumBin())-0.01,focalYh[col][row]->GetBinCenter(focalYh[col][row]->GetMaximumBin())+0.01);
				focalYh[col][row]->Fit("gaus","","",focalYh[col][row]->GetBinCenter(focalYh[col][row]->GetMaximumBin())-0.0018,focalYh[col][row]->GetBinCenter(focalYh[col][row]->GetMaximumBin())+0.0015);
				focalYh[col][row]->Draw();
				focal_y=focalYh[col][row]->GetFunction("gaus")->GetParameter(1);

				mainPatternCanvas->cd(2)->cd(4);

				chain->Project(focalPhih[col][row]->GetName(),Form("%s.tr.r_ph",HRS.Data()),sieveHoleCut);
				focalPhih[col][row]->GetXaxis()->SetRangeUser(focalPhih[col][row]->GetBinCenter(focalPhih[col][row]->GetMaximumBin())-0.01,focalPhih[col][row]->GetBinCenter(focalPhih[col][row]->GetMaximumBin())+0.01);
				focalPhih[col][row]->Fit("gaus","","",focalPhih[col][row]->GetBinCenter(focalPhih[col][row]->GetMaximumBin())-0.0023,focalPhih[col][row]->GetBinCenter(focalPhih[col][row]->GetMaximumBin())+0.0023);
				focalPhih[col][row]->Draw();
				focal_ph=focalPhih[col][row]->GetFunction("gaus")->GetParameter(1);

				//write the BPM informations
				mainPatternCanvas->cd(3)->cd(1);
				chain->Project(bpmXinforh[col][row]->GetName(),Form("targx"),sieveHoleCut);
				bpmXinforh[col][row]->GetXaxis()->SetRangeUser(bpmXinforh[col][row]->GetBinCenter(bpmXinforh[col][row]->GetMaximumBin())-1,bpmXinforh[col][row]->GetBinCenter(bpmXinforh[col][row]->GetMaximumBin())+1);
				bpmXinforh[col][row]->Fit("gaus","","",bpmXinforh[col][row]->GetBinCenter(bpmXinforh[col][row]->GetMaximumBin())-1,bpmXinforh[col][row]->GetBinCenter(bpmXinforh[col][row]->GetMaximumBin())+1);
				bpmXinforh[col][row]->Draw();
				bpmX=bpmXinforh[col][row]->GetFunction("gaus")->GetParameter(1);

				mainPatternCanvas->cd(3)->cd(2);
				chain->Project(bpmYinforh[col][row]->GetName(),Form("targy"),sieveHoleCut);
			    bpmYinforh[col][row]->GetXaxis()->SetRangeUser(bpmYinforh[col][row]->GetBinCenter(bpmYinforh[col][row]->GetMaximumBin())-1,bpmYinforh[col][row]->GetBinCenter(bpmYinforh[col][row]->GetMaximumBin())+1);
			    bpmYinforh[col][row]->Fit("gaus","","",bpmYinforh[col][row]->GetBinCenter(bpmYinforh[col][row]->GetMaximumBin())-1,bpmYinforh[col][row]->GetBinCenter(bpmYinforh[col][row]->GetMaximumBin())+1);
			    bpmYinforh[col][row]->Draw();
				bpmY=bpmYinforh[col][row]->GetFunction("gaus")->GetParameter(1);

				// draw the theta and phi on the canvas
				mainPatternCanvas->cd(3)->cd(3);
				SieveThetaPhiCuthh=new TH2F(Form("th_vs_ph_cut_Run%d_kine%d_col%d_row%d",runID,KineID,col,row),Form("th_vs_ph_cut_Run%d_kine%d_col%d_row%d",runID,KineID,col,row),
						1000,-0.03,0.03,1000,-0.045,0.05);
				chain->Project(SieveThetaPhiCuthh->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),sieveMomCut);
				SieveThetaPhiCuthh->Draw("zcol");

				mainPatternCanvas->Update();
				//mainPatternCanvas->Write(Form("sieve_kine%d_col%d_row%d",KineID,col,row));
				f51OutPut<<uid<<"	"<<focal_x<<" "<<focal_th<<" "<<focal_y<<" "<<focal_ph<<" "<<0.00000<<" "<<bpmX/1000.0<<" "<<bpmY/1000.0<<" "<<0.0<<std::endl;

			}
		}
	}
	f51OutPut.close();
	mainPatternCanvas->Close();
	return 1;
}

// Used check the cut
//input, the root file, cut root file
//TODO, need to input the size ID vs. scattered angle chart
Int_t OpticsCutCheck(UInt_t runID,TString cutFile, TString folder="/home/newdriver/Storage/Research/CRex_Experiment/RasterReplay/Replay/Result") {
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
		mainPatternCanvas=new TCanvas("cutPro","cutPro",1960,1080);
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
	//merge all the sieve holes cut together
	sieveAllHoleCut=sieveAllHoleCut+TCut(generalcut.Data());
	mainPatternCanvas->cd(2);
	TH2F *TargetThPh_SieveCutHH=(TH2F *)gROOT->FindObject("th_vs_ph_cut");
	if(TargetThPh_SieveCutHH) TargetThPh_SieveCutHH->Delete();
	TargetThPh_SieveCutHH=new TH2F("th_vs_ph_cut","th_vs_ph_cut",1000,-0.03,0.03,1000,-0.045,0.045);
	chain->Project(TargetThPh_SieveCutHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),sieveAllHoleCut);
	TargetThPh_SieveCutHH->Draw("zcol");
	mainPatternCanvas->Update();

	return 1;
}



/// used for add the first excited states in the fitting
//Recognize the save patter
// save the rec hole to the folder
// first excited state
void SavePatternHole_P1(double momentumSigmaCut=3.0){
	//search all the holes in this col and save in the folder
	std::cout<<std::endl<<std::endl;
	std::cout<<"*******Save process start ........*******"<<std::endl;
	std::cout<<"	Searching for holes in col ("<<col<<")"<<std::endl;
	std::cout<<"	CAUTION :: USING First Excited States !!!!!"<<std::endl;

	TCanvas *SaveCheckCanvas=(TCanvas *) gROOT->GetListOfCanvases()->FindObject("SieveSaveCheck");
	if(!SaveCheckCanvas){
		SaveCheckCanvas =new TCanvas("SieveSaveCheck","SieveSaveCheck",1600,1080);
	}else{
		SaveCheckCanvas->Clear();
	}
	SaveCheckCanvas->Divide(1,2);
	SaveCheckCanvas->cd(1)->Divide(2,1);
	SaveCheckCanvas->cd(2)->Divide(row_count,1);


	TString workdir_temp=WorkDir+"/FirstMomCut/";

	// check the existance of the folder, if not create the folder
	if(!boost::filesystem::is_directory(workdir_temp.Data())){
		std::cout<<"Folder :"<< workdir_temp.Data()<<"  Does Not EXIST\n Trying to Create the folder"<<std::endl;
		if(!boost::filesystem::create_directories(workdir_temp.Data())){
			std::cout<<"ERROR:: cannot creat folde, please check the permission!!!!"<<std::endl;
			exit(-1);
		}
	}

	TString CutFileName = workdir_temp + RootFileName + CutSuf;
	TString TempString(Form(CutDescFileSufSieve.Data(), FoilID, col));
	TString PlotDir(RootFileName + Form(".hcut_R_%d_%d/", FoilID, col));
	TString CutDescName = workdir_temp + RootFileName + TempString;
	// prepare the filename and folder
	//TString CutDescName = WorkDir + RootFileName + CutDescFileSufDp;
	//TString CutFileName = WorkDir + RootFileName + CutSuf; // used for save the root file and cut file
	TFile *f1=new TFile(CutFileName,"UPDATE");
	assert(f1);

	std::fstream cutdesc(CutDescName, std::ios_base::out);
	assert(cutdesc.is_open());

	SaveCheckCanvas->cd(1)->cd(1);
	// attached the root file and plot the canvas used for check
	TChain *chain = (TChain *) gROOT->FindObject("T");
	if(!chain) std::cout<<"[ERROR] CAN NOT FIND TREE"<<std::endl;
	TH2F *h=(TH2F*)gROOT->FindObject("th_vs_ph");
	if(!h)
		h=new TH2F("th_vs_ph1","th_vs_ph1",1000,-0.03,0.03,1000,-0.045,0.05);

	TH2F *SavesieveCheck = new TH2F("SaveSieveCheck", "SaveSieveCheck",
			h->GetXaxis()->GetNbins(), h->GetXaxis()->GetXmin(),
			h->GetXaxis()->GetXmax(), h->GetYaxis()->GetNbins(),
			h->GetYaxis()->GetXmin(), h->GetYaxis()->GetXmax());

	TString HRS("R");
	TString filename(chain->GetFile()->GetName());
	if (filename.Contains("RHRS")) {
	} else if (filename.Contains("LHRS")) {
		HRS = "L";
	}
	chain->Project(SavesieveCheck->GetName(),Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),Form("%s",generalcut.Data()));
	//chain->Draw(Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),Form("%s",generalcut.Data()),"zcol");
	SavesieveCheck->Draw("zcol");
	// search how many cut file exist in the root buffer
	for (int i = 0 ; i < row_min; i ++)cutdesc<<"fEvtHdr.fRun==0"<< std::endl;
	std::vector<int> sieveIDList;
	TH2F *sievehole[row_count];
	TH1F *sieveholemomentum[row_count];    // ground states momentum
	TF1  *sieveholemomentumGausFit[row_count];
	TF1  *sieveholemomentumGausFit_p1[row_count];
	TLatex * momentumInfor1[row_count];
	TCutG *cutg;
	for (int row_iter = row_min; row_iter<row_min+row_count;row_iter++){

		cutg=(TCutG *) gROOT->FindObject(Form("hcut_R_%d_%d_%d",FoilID,col,row_iter));
		if(cutg){
			sieveIDList.push_back(row_iter-row_min);
			sievehole[row_iter-row_min] = new TH2F(
					Form("hcut_R_%d_%d_%d_hh", FoilID, col, row_iter),
					Form("hcut_R_%d_%d_%d_hh", FoilID, col, row_iter),
					h->GetXaxis()->GetNbins(), h->GetXaxis()->GetXmin(),
					h->GetXaxis()->GetXmax(), h->GetYaxis()->GetNbins(),
					h->GetYaxis()->GetXmin(), h->GetYaxis()->GetXmax());
			SaveCheckCanvas->cd(2)->cd(row_iter - row_min + 1);
//			SaveCheckCanvas->cd(2)->cd(row_iter - row_min + 1)->SetLogy();
			chain->Project(sievehole[row_iter-row_min]->GetName(), Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),Form("%s && %s",cutg->GetName(),generalcut.Data()));

			sieveholemomentum[row_iter-row_min]=new TH1F(Form("hcut_R_%d_%d_%d_h_momentum", FoilID, col, row_iter),Form("hcut_R_%d_%d_%d_momentum", FoilID, col, row_iter),600,2.1,2.25);
			chain->Project(sieveholemomentum[row_iter-row_min]->GetName(),Form("%s.gold.p",HRS.Data()),Form("%s && %s",cutg->GetName(),generalcut.Data()));
			sieveholemomentumGausFit[row_iter-row_min]=new TF1(Form("1ststatesDpgaushcut_R_%d_%d_%d", FoilID, col, row_iter),"gaus",
					sieveholemomentum[row_iter-row_min]->GetXaxis()->GetBinCenter(sieveholemomentum[row_iter-row_min]->GetMaximumBin())-0.002,
					sieveholemomentum[row_iter-row_min]->GetXaxis()->GetBinCenter(sieveholemomentum[row_iter-row_min]->GetMaximumBin())+0.002);
			sieveholemomentumGausFit[row_iter-row_min]->SetParameter(1,sieveholemomentum[row_iter-row_min]->GetXaxis()->GetBinCenter(sieveholemomentum[row_iter-row_min]->GetMaximumBin()));
			sieveholemomentum[row_iter - row_min]->Fit(
					Form("1ststatesDpgaushcut_R_%d_%d_%d", FoilID, col,
							row_iter), "R", "ep",
					sieveholemomentumGausFit[row_iter-row_min]->GetXmin(),
					sieveholemomentumGausFit[row_iter-row_min]->GetXmax());

			sieveholemomentum[row_iter - row_min]->GetXaxis()->SetRangeUser(
					sieveholemomentum[row_iter - row_min]->GetXaxis()->GetBinCenter(
							sieveholemomentum[row_iter - row_min]->GetMaximumBin())
							- 0.009,
					sieveholemomentum[row_iter - row_min]->GetXaxis()->GetBinCenter(
							sieveholemomentum[row_iter - row_min]->GetMaximumBin())
							+ 0.004);
			sieveholemomentum[row_iter - row_min]->Draw();
			sieveholemomentumGausFit[row_iter-row_min]->Draw("same");

			auto groudpcenter=sieveholemomentumGausFit[row_iter-row_min]->GetParameter(1);
			auto groudpsigma=sieveholemomentumGausFit[row_iter-row_min]->GetParameter(2);

			momentumInfor1[row_iter - row_min] =
					new TLatex(groudpcenter + 2 * groudpsigma-0.007,
							sieveholemomentumGausFit[row_iter - row_min]->GetParameter(0),
							Form("P_{0}= %2.5f",
									groudpcenter));//, groudpsigma
			momentumInfor1[row_iter - row_min]->SetTextSize(0.055);
			momentumInfor1[row_iter - row_min]->SetTextAlign(12);
			momentumInfor1[row_iter - row_min]->SetTextColor(2);
			momentumInfor1[row_iter - row_min]->Draw("same");

			SaveCheckCanvas->cd(2)->cd(row_iter - row_min + 1)->Update();
			if(groudpsigma>0.0008)groudpsigma=0.0008;
			// plot the boundary of the cut
			TLine *leftboundary=new TLine(groudpcenter-momentumSigmaCut*groudpsigma,0,groudpcenter-momentumSigmaCut*groudpsigma,(0.9*SaveCheckCanvas->cd(2)->cd(row_iter - row_min + 1)->GetUymax()));
			leftboundary->SetLineColor(3);
			leftboundary->SetLineWidth(2);
			leftboundary->Draw("same");

			TLine *rightboundary=new TLine(groudpcenter+momentumSigmaCut*groudpsigma,0,groudpcenter+momentumSigmaCut*groudpsigma,(0.9*SaveCheckCanvas->cd(2)->cd(row_iter - row_min + 1)->GetUymax()));
			rightboundary->SetLineColor(3);
			rightboundary->SetLineWidth(2);
			rightboundary->Draw("same");

			// add the second excited state gaus fit
			TH1F *hSieveP1h=(TH1F *)sieveholemomentum[row_iter - row_min]->Clone("C12.p1");
//			hSieveP1h->GetXaxis()->SetRangeUser(sieveholemomentum[row_iter - row_min]->GetXaxis()->GetXmin()
//							,groudpcenter-groudpsigma*4);

			hSieveP1h->GetXaxis()->SetRangeUser(groudpcenter-0.006
							,groudpcenter-0.004);


			// get the bin center, and used this bin center as the fit center for the P1
			double_t p1_mean=hSieveP1h->GetXaxis()->GetBinCenter(hSieveP1h->GetMaximumBin());
			std::cout<<"===>"<<p1_mean<<std::endl;
			hSieveP1h->Delete();
			sieveholemomentumGausFit_p1[row_iter-row_min]=new TF1(Form("1ststatesDpgaushcut_R_p1_%d_%d_%d", FoilID, col, row_iter),"gaus",p1_mean-0.0009,
					p1_mean+0.0009);
			sieveholemomentum[row_iter - row_min]->Fit(
								Form("1ststatesDpgaushcut_R_p1_%d_%d_%d", FoilID, col,
										row_iter), "R", "ep",
										sieveholemomentumGausFit_p1[row_iter-row_min]->GetXmin(),
										sieveholemomentumGausFit_p1[row_iter-row_min]->GetXmax());
			sieveholemomentumGausFit_p1[row_iter-row_min]->Draw("same");
			double_t p1guasMeam=sieveholemomentumGausFit_p1[row_iter-row_min]->GetParameter(1);
			double_t p1guasSigma=sieveholemomentumGausFit_p1[row_iter-row_min]->GetParameter(2);


			TLine *p1leftboundary=new TLine(p1guasMeam-(momentumSigmaCut-1)*p1guasSigma,0,p1guasMeam-(momentumSigmaCut-1)*p1guasSigma,(0.9*SaveCheckCanvas->cd(2)->cd(row_iter - row_min + 1)->GetUymax()));
			p1leftboundary->SetLineColor(5);
			p1leftboundary->SetLineWidth(2);
			p1leftboundary->Draw("same");

			TLine *p1rightboundary=new TLine(p1guasMeam+(momentumSigmaCut-1)*p1guasSigma,0,p1guasMeam+(momentumSigmaCut-1)*p1guasSigma,(0.9*SaveCheckCanvas->cd(2)->cd(row_iter - row_min + 1)->GetUymax()));
			p1rightboundary->SetLineColor(5);
			p1rightboundary->SetLineWidth(2);
			p1rightboundary->Draw("same");


			SaveCheckCanvas->cd(1)->cd(1);
			cutg->SetName(Form("hcut_R_%d_%d_%d", FoilID, col, row_iter));
			cutg->SetVarX(Form("%s.gold.ph",HRS.Data()));
			cutg->SetVarY(Form("%s.gold.th",HRS.Data()));
			cutg->SetLineColor(kMagenta);
			cutg->SetLineWidth(2);
			cutg->Draw("PL same");
			SaveCheckCanvas->cd(1)->cd(1)->SetGridx(20);
			SaveCheckCanvas->cd(1)->cd(1)->SetGridy(20);
			//plot the momentum and apply cut on the momentum

			cutg->Write("", TObject::kOverwrite); // Overwrite old cut
			if(groudpsigma>0.0008)groudpsigma=0.0008;
			cutdesc << Form("hcut_R_%d_%d_%d", FoilID, col, row_iter) << " && ";
			cutdesc << (const char*)generalcut <<" && "
					<<Form("abs(%s.gold.p-%f)<%f*%f",HRS.Data(),p1guasMeam,momentumSigmaCut-1,p1guasSigma)
					<< std::endl;

			SaveCheckCanvas->cd(1)->cd(2);

			TH2F *sieveholetemp= new TH2F(
								Form("hcut_R_%d_%d_%d_hhg", FoilID, col, row_iter),
								Form("hcut_R_%d_%d_%d_hhg", FoilID, col, row_iter),
								h->GetXaxis()->GetNbins(), h->GetXaxis()->GetXmin(),
								h->GetXaxis()->GetXmax(), h->GetYaxis()->GetNbins(),
								h->GetYaxis()->GetXmin(), h->GetYaxis()->GetXmax());
			sieveholetemp->Draw("same");
			chain->Project(sieveholetemp->GetName(),Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),Form("%s && %s && abs(%s.gold.p-%f)<%f*%f",generalcut.Data(),cutg->GetName(),HRS.Data(),groudpcenter,momentumSigmaCut,groudpsigma));

			TH2F *sieveholetempp1= new TH2F(
											Form("hcut_R_%d_%d_%d_hhp1", FoilID, col, row_iter),
											Form("hcut_R_%d_%d_%d_hhp1", FoilID, col, row_iter),
											h->GetXaxis()->GetNbins(), h->GetXaxis()->GetXmin(),
											h->GetXaxis()->GetXmax(), h->GetYaxis()->GetNbins(),
											h->GetYaxis()->GetXmin(), h->GetYaxis()->GetXmax());
			chain->Project(sieveholetempp1->GetName(),Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),Form("%s && %s && abs(%s.gold.p-%f)<%f*%f",generalcut.Data(),cutg->GetName(),HRS.Data(),p1guasMeam,momentumSigmaCut-1,p1guasSigma));

			TLatex *eventCountLable=new TLatex(sievehole[row_iter-row_min]->GetMean(1) - 0.02,sievehole[row_iter-row_min]->GetMean(2)+0.003, Form("Entries(%d,%d):%2.0f->%2.0f p1:%2.0f",col,row_iter, (sievehole[row_iter-row_min]->GetEntries()),sieveholetemp->GetEntries(),sieveholetempp1->GetEntries()));
			eventCountLable->SetTextSize(0.03);
			eventCountLable->SetTextAlign(12);
			eventCountLable->SetTextColor(2);
			eventCountLable->Draw("same");
			SaveCheckCanvas->cd(1)->cd(2)->SetGridx(20);
			SaveCheckCanvas->cd(1)->cd(2)->SetGridy(20);
			//sievehole[row_iter-row_min]->Delete();
			cutg->Draw("same");
			SaveCheckCanvas->Update();
			sieveholetemp->Delete();
			sieveholetempp1->Delete();

		}else{
			//if the cut does not exist,then write the cut
			cutdesc << "fEvtHdr.fRun==0" << std::endl;
		}

	}

	for(int i = row_min+row_count; i < NSieveRow; i++)
		cutdesc << "fEvtHdr.fRun==0" << std::endl;
	SaveCheckCanvas->SetName(Form("CutProfcut_R_%d_%d",FoilID, col));
	SaveCheckCanvas->Write("", TObject::kOverwrite);
	SaveCheckCanvas->SaveAs(Form("%s/%s.hcut_R_%d_%d.jpg",workdir_temp.Data(),RootFileName.Data(),FoilID, col));

	for(auto i : sieveIDList){
//	for (unsigned int i = 0; i < row_count; i++) {
		if (!sievehole[i]->IsZombie()) {
			sievehole[i]->Delete();
		}
		if (!sieveholemomentum[i]->IsZombie()) {
			sieveholemomentum[i]->Delete();
		}
		if (!sieveholemomentumGausFit[i]->IsZombie()) {
			sieveholemomentumGausFit[i]->Delete();
			sieveholemomentumGausFit_p1[i]->Delete();
		}

	}
	if(!SavesieveCheck->IsZombie()){
		SavesieveCheck->Delete();
	}
	if(!h->IsZombie()){
		h->Delete();
	}

	f1->Write();
	f1->ls();
	f1->Close();
	cutdesc.close();
}


void CurrentStatus(){
	std::cout<<"***  's' save the current cut to file *****"<<std::endl;
	std::cout<<"***  '-' decrease one hole ID         *****"<<std::endl;
	std::cout<<"***  '+' increase one hole ID         *****"<<std::endl;
	std::cout<<"Currently working on ::"<<std::endl;
	std::cout<<"	hole col :"<<col<<std::endl;
	std::cout<<"    hole row :"<<row<< "  minID:"<<row_min<<"  count:"<<row_count<<std::endl;
	std::cout<<">>>Please click the center of the hole ("<<col<<" ,"<<row<<" ) ...."<<std::endl;
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
	TFile *tempfile=new TFile("temp.root","RECREATE");

	// link the root tree and check which HRS we are working on
	TChain *chain = (TChain *) gROOT->FindObject("T");
	TString HRS("R");
	TString filename(chain->GetFile()->GetName());
	if (filename.Contains("RHRS")) {
	} else if (filename.Contains("LHRS")) {
		HRS = "L";
	}

	// check the input keys
	if(event == kKeyPress) {

		std::cout<<"Key Pressed::"<<(gPad->GetEventX())<<std::endl;
		switch ((int) (gPad->GetEventX())) {
		case '+':
			row++;
			CurrentStatus();
			break;
		case '-':
			row--;
			CurrentStatus();
			break;
		case 's':
			std::cout << "Save Button Clicked" << std::endl;
//			SavePatternHole();
			SavePatternHole(300000); // with out the groud momentum cut
//			SavePatternHole_P1();
			break;
		case 'q':
			{std::cout << "Quit Button Clicked" << std::endl;
			gApplication->Clear();
			gApplication->Terminate();
			break;}
		default:
			std::cout<<(char)(gPad->GetEventX())<<std::endl;
		}
	}else
	if (event==kButton1Down) {
		TH2 *h = (TH2*) select;
		gPad->GetCanvas()->FeedbackMode(kTRUE);


		// if the button is clicked
		//Rec the sieve pattern
		// get the mouse click position in histogram
		double_t x = (gPad->PadtoX(gPad->AbsPixeltoX(gPad->GetEventX())));
		double_t y = (gPad->PadtoY(gPad->AbsPixeltoY(gPad->GetEventY())));

		// before load to contour algorithm, get the more accurate center first
		// create new canvas
		TCanvas *SieveRecCanvas = (TCanvas*) gROOT->GetListOfCanvases()->FindObject("SieveRecCanvas");
		if(SieveRecCanvas){
			SieveRecCanvas->Clear();
			delete SieveRecCanvas->GetPrimitive("Projection");
		}else
			SieveRecCanvas = new TCanvas("SieveRecCanvas","Projection Canvas", 1000,1000);

			SieveRecCanvas->Divide(1,2);
			SieveRecCanvas->cd(1)->Divide(2,1);
			SieveRecCanvas->cd(2)->Divide(4,1);

			SieveRecCanvas->cd(1)->cd(2)->Divide(1,3);

			SieveRecCanvas->cd(1)->cd(2)->cd(1);
			//preCut
			TH2F *selectedSievePreCuthh = (TH2F *) gROOT->FindObject(
					"Sieve_Selected_th_ph_PreCut");
			if (selectedSievePreCuthh) {
				selectedSievePreCuthh->Clear();
			} else {
				selectedSievePreCuthh= new TH2F("Sieve_Selected_th_ph_PreCut",
						"Sieve_Selected_th_ph_PreCut", 100, h->GetXaxis()->GetXmin(),
						h->GetXaxis()->GetXmax(), 100, h->GetYaxis()->GetXmin(),
						h->GetYaxis()->GetXmax());
			}

			chain->Project(selectedSievePreCuthh->GetName(),
					Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),
					Form("sqrt((%s.gold.th-%f)^2+ (%s.gold.ph-%f)^2)<0.003 && %s ",
							HRS.Data(), y, HRS.Data(), x, generalcut.Data()));
			selectedSievePreCuthh->GetXaxis()->SetTitle(Form("%s.gold.ph",HRS.Data()));
			selectedSievePreCuthh->GetYaxis()->SetTitle(Form("%s.gold.th",HRS.Data()));
			//project to theta and phi, and start fit, get  more accurate position before pass to the counter
			auto projectxPreCut = selectedSievePreCuthh->ProjectionX();
			auto projectyPreCut = selectedSievePreCuthh->ProjectionY();
			selectedSievePreCuthh->Draw("zcol");

			SieveRecCanvas->cd(1)->cd(2)->cd(2);
			projectxPreCut->Draw();
			SieveRecCanvas->cd(1)->cd(2)->cd(3);
			projectyPreCut->Draw();
			//get the fit and update the position information
			projectxPreCut->Fit("gaus","","");
			projectyPreCut->Fit("gaus","","");

			// get the updated informations
			x=projectxPreCut->GetFunction("gaus")->GetParameter(1); //phi
			y=projectyPreCut->GetFunction("gaus")->GetParameter(1);  //theta


			//get the hsitogram and start rec
			SieveRecCanvas->cd(2)->cd(1);

			TH2F *selectedSievehh=(TH2F *)  gROOT->FindObject("Sieve_Selected_th_ph");
			if(selectedSievehh){
				selectedSievehh->Clear();
			}else{
			selectedSievehh = new TH2F("Sieve_Selected_th_ph",
					"Sieve_Selected_th_ph",
					100,
					h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax(),
					100,
					h->GetYaxis()->GetXmin(),h->GetYaxis()->GetXmax());
			}

			chain->Project(selectedSievehh->GetName(),
					Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),
					Form("sqrt((%s.gold.th-%f)^2+ (%s.gold.ph-%f)^2)<0.003 && %s ",
							HRS.Data(), y, HRS.Data(), x,generalcut.Data()));
			selectedSievehh->SetContour(10);
			selectedSievehh->GetXaxis()->SetTitle(Form("%s.gold.ph",HRS.Data()));
			selectedSievehh->GetYaxis()->SetTitle(Form("%s.gold.th",HRS.Data()));
			selectedSievehh->Draw("CONT LIST");

			SieveRecCanvas->Update(); // update the canvas to let the pattern buffer in root

			// extract the contour
			TObjArray *conts = (TObjArray*) gROOT->GetListOfSpecials()->FindObject(
					"contours");
			if (!conts)
				return;
			TList *lcontour1 = (TList*) conts->At(1);
			if (!lcontour1)
				return;
			TGraph *gc1 = (TGraph*) lcontour1->First();
			if (!gc1)
				return;
			if (gc1->GetN() < 10)
				return;

			//TODO need to change the name of
			TCutG *cutg = new TCutG(Form("hcut_R_%d_%d_%d", FoilID, col, row), gc1->GetN(), gc1->GetX(), gc1->GetY());
			cutg->SetLineColor(kRed);
			cutg->SetName(Form("hcut_R_%d_%d_%d", FoilID, col, row));
			cutg->SetVarX(Form("%s.gold.ph",HRS.Data()));
			cutg->SetVarY(Form("%s.gold.th",HRS.Data()));
			cutg->Draw("same");

			SieveRecCanvas->cd(2)->cd(2);
			auto projectx = selectedSievehh->ProjectionX();
			projectx->Draw();
			projectx->Fit("gaus");

			SieveRecCanvas->cd(2)->cd(3);
			auto projecty = selectedSievehh->ProjectionY();
			projecty->Draw();
			projecty->Fit("gaus");

			// plot the cut on the canvas
			SieveRecCanvas->cd(1)->cd(1);

			TH2F *patternCheck=(TH2F *)  gROOT->FindObject("Sieve_Pattern_Check");
			if(patternCheck){
				patternCheck->Clear();
			}
			patternCheck = new TH2F("Sieve_Pattern_Check",
					"Sieve_Pattern_Check", h->GetXaxis()->GetNbins(),
					h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax(),
					h->GetYaxis()->GetNbins(), h->GetYaxis()->GetXmin(),
					h->GetYaxis()->GetXmax());
			chain->Project(patternCheck->GetName(),
					Form("%s.gold.th:%s.gold.ph", HRS.Data(), HRS.Data()),Form("%s",generalcut.Data()));
			patternCheck->Draw("zcol");
			cutg->Draw("same");
			SieveRecCanvas->Update(); // update the canvas to let the pattern buffer in root

			TLatex *label=new TLatex(selectedSievehh->GetMean(1),selectedSievehh->GetMean(2),Form("(%d %d)",col,row));
			label->SetTextSize(0.04);
			label->SetTextColor(2);
			label->Draw("same");

			row++;
			SieveRecCanvas->Update();

			SieveRecCanvas->cd(2)->cd(4);
			TH1F *sieveholemomentum=new TH1F(Form("hcut_R_%d_%d_%d_h_momentum_check", FoilID, col, row),Form("hcut_R_%d_%d_%d_momentum_check", FoilID, col, row),600,2.1,2.25);
			chain->Project(sieveholemomentum->GetName(),Form("%s.gold.p",HRS.Data()),Form("%s && %s",cutg->GetName(),generalcut.Data()));
			sieveholemomentum->GetXaxis()->SetRangeUser(
					sieveholemomentum->GetXaxis()->GetBinCenter(
							sieveholemomentum->GetMaximumBin())
							- 0.009,
					sieveholemomentum->GetXaxis()->GetBinCenter(
							sieveholemomentum->GetMaximumBin())
							+ 0.004);
			sieveholemomentum->Draw();
			SieveRecCanvas->Update();
			SieveRecCanvas->Write();
	}
	tempfile->Write();
	tempfile->Close();
}

