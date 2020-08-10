/*
 * checkVDCtdcCalib.C
 *
 *  Created on: May 20, 2020
 *      Author: newdriver
 */


#include <TROOT.h>
#include <TChain.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TCanvas.h>
#include <TSpectrum.h>
#include <TLine.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string.h>
#include <string>
#include <TStyle.h>
#include <fstream>

using namespace std;

TString generalcut;
TString generalcutR="R.tr.n==1 && R.vdc.u1.nclust==1&& R.vdc.v1.nclust==1 && R.vdc.u2.nclust==1 && R.vdc.v2.nclust==1 && R.gold.p > 2.14 && R.gold.p < 2.2  ";
TString generalcutL="L.tr.n==1 && L.vdc.u1.nclust==1&& L.vdc.v1.nclust==1 && L.vdc.u2.nclust==1 && L.vdc.v2.nclust==1  && L.gold.p > 2.14 && L.gold.p < 2.2";

void checkVDCtdcCalib(const char *arm = "R", const char* rootfilename = "Afile.root"){
	gROOT->SetStyle("Plain");
	gStyle->SetOptStat(0);
	gStyle->SetPalette(1);
	vector<string> rootfiles;
	rootfiles.push_back(string(rootfilename));

	// load the root files
	TChain *chain=new TChain("T");
	for (vector<string>::const_iterator it = rootfiles.begin();it!=rootfiles.end();++it){
		chain->Add((*it).c_str());
	}
	//chain->SetBranchStatus("*", kFALSE);
	vector<string> planes {"u1","v1","u2","v2"};

	TH1F *wireDistri[4];
	TH2I *htime[4];
	for( int i : {0,1,2,3} ) {
		wireDistri[i]=new TH1F(Form("R.vdc.%s.wire",planes[i].c_str()),Form("R.vdc.%s.wire",planes[i].c_str()),400,0,400);
		htime[i]=new  TH2I(Form("Wire vs time R.vdc.%s",planes[i].c_str()),Form("Wire vs time R.vdc.%s",planes[i].c_str()),300,-100e-9,500e-9,368,0,368);
		chain->Project(wireDistri[i]->GetName(),Form("R.vdc.%s.wire",planes[i].c_str()),generalcutR.Data());
		chain->Project(htime[i]->GetName(),Form("R.vdc.%s.wire:R.vdc.%s.time",planes[i].c_str(),planes[i].c_str()), generalcutR.Data());
	}
	TCanvas *vdcwirecanv = new TCanvas("cv", "VDC wire spectra");
	vdcwirecanv->Divide(2, 2);
	vdcwirecanv->Draw();
	for( int i : {0,1,2,3} ) {
		vdcwirecanv->cd(i+1);
		wireDistri[i]->Draw();
	}
	vdcwirecanv->Update();

	TCanvas *vdchtimecanv = new TCanvas("htime", "VDC wire vs. time");
	vdchtimecanv->Divide(2, 2);
	vdchtimecanv->Draw();
	TLine *line[4];
	for( int i : {0,1,2,3} ) {
		vdchtimecanv->cd(i+1);
		htime[i]->Draw("zcol");
		line[i]=new TLine(0,0,0,350);
		line[i]->SetLineWidth(3);
		line[i]->SetLineColor(2);
		line[i]->Draw("same");
	}
	vdchtimecanv->Update();

}

//new function used for check the T-0 check
//
void VDCtdcCalibCheck(const char *arm = "R", const char* rootfilename = "Afile.root"){
	TString HRS(arm);
	generalcut=generalcutL;
	if (HRS == "R"){
		generalcut=generalcutR;
	}

	gROOT->SetStyle("Plain");
	gStyle->SetOptStat(0);
	gStyle->SetPalette(1);
	vector<string> rootfiles;
	rootfiles.push_back(string(rootfilename));

	// load the root files
	TChain *chain=new TChain("T");
	for (vector<string>::const_iterator it = rootfiles.begin();it!=rootfiles.end();++it){
		chain->Add((*it).c_str());
	}
	//chain->SetBranchStatus("*", kFALSE);
	vector<string> planes {"u1","v1","u2","v2"};

	TH1F *wireDistri[4];
	TH2I *htime[4];
	for( int i : {0,1,2,3} ) {
		wireDistri[i]=new TH1F(Form("%s.vdc.%s.wire",arm,planes[i].c_str()),Form("%s.vdc.%s.wire",arm, planes[i].c_str()),400,0,400);
		htime[i]=new  TH2I(Form("Wire vs time %s.vdc.%s",arm , planes[i].c_str()),Form("Wire vs time %s.vdc.%s",arm, planes[i].c_str()),300,-100e-9,500e-9,368,0,368);
		chain->Project(wireDistri[i]->GetName(),Form("%s.vdc.%s.wire",arm, planes[i].c_str()),generalcut.Data());
		chain->Project(htime[i]->GetName(),Form("%s.vdc.%s.wire:%s.vdc.%s.time",arm, planes[i].c_str(),arm,planes[i].c_str()), generalcut.Data());
	}
	TCanvas *vdcwirecanv = new TCanvas("cv", "VDC wire spectra");
	vdcwirecanv->Divide(2, 2);
	vdcwirecanv->Draw();
	for( int i : {0,1,2,3} ) {
		vdcwirecanv->cd(i+1);
		wireDistri[i]->Draw();
	}
	vdcwirecanv->Update();

	TCanvas *vdchtimecanv = new TCanvas("htime", "VDC wire vs. time");
	vdchtimecanv->Divide(2, 2);
	vdchtimecanv->Draw();
	TLine *line[4];
	for( int i : {0,1,2,3} ) {
		vdchtimecanv->cd(i+1);
		htime[i]->Draw("zcol");
		line[i]=new TLine(0,0,0,350);
		line[i]->SetLineWidth(3);
		line[i]->SetLineColor(2);
		line[i]->Draw("same");
	}
	vdchtimecanv->Update();

}
