/*
 * pointingCheck.C
 *
 * Used for generate the plot for Pionting Measurement
 *
 *  Created on: Aug 24, 2020
 *      Author: Siyu Jian
 *              jiansiyu@gmail.com
 */

/*
 * plotTemp.C
 *
 *  Created on: Jan 11, 2020
 *      Author: newdriver
 */
#include <TROOT.h>
#include <TStyle.h>
#include "TSystem.h"

#include <TCanvas.h>
#include <TString.h>
#include <TChain.h>
#include <TCut.h>
#include <TPad.h>
#include <TMath.h>
#include <TH1.h>

#include <TPaveText.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <map>
#include <vector>
#include <random>
#include <iostream>
//#include <sys/stat.h>
#include <TLegend.h>
#include <TLatex.h>
#include "fstream"
#include "iostream"
#include "string"
//#include "i"
inline Bool_t IsFileExist (const std::string& name) {
    return !gSystem->AccessPathName(name.c_str());
//	  struct stat buffer;
//	  return (stat (name.c_str(), &buffer) == 0);
}

// plot cut
void plotTemp(UInt_t runID,TString folder="/home/newdriver/Storage/Research/CRex_Experiment/optReplay/Result", double_t groundp=0.9476, double_t firstp=0.94325){

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

	//plot check the plot
	TH1F *bpmAX=new TH1F("BeamPosAX","BeamPosAX",100,-0.8,0.8);  // beam position  on beam A
	TH1F *bpmAY=new TH1F("BeamPosAY","BeamPosAY",100,-0.8,0.8);

	TH1F *bpmBX = new TH1F("BeamPosBX", "BeamPosBX", 100, -0.8, 0.8); // beam position  on beam A
	TH1F *bpmBY = new TH1F("BeamPosBY", "BeamPosBY", 100, -0.8, 0.8);

	TH1F *bpmCX = new TH1F("BeamPosCX", "BeamPosCX", 100, -0.8, 0.8); // beam position  on beam A
	TH1F *bpmCY = new TH1F("BeamPosCY", "BeamPosCY", 100, -0.8, 0.8);

	TH1F *bpmDX = new TH1F("BeamPosDX", "BeamPosDX", 100, -0.8, 0.8); // beam position  on beam A
	TH1F *bpmDY = new TH1F("BeamPosDY", "BeamPosDY", 100, -0.8, 0.8);


	TH1F *bpmEX=new TH1F("BeamPosEX","BeamPosEX",100,-0.8,0.8);  // beam position  on beam E
	TH1F *bpmEY=new TH1F("BeamPosEY","BeamPosEY",100,-0.8,0.8);

	TH1F *bpmTargetX=new TH1F("BeamTargetX","BeamTargetX",100,-0.5,0.5);  // beam position  on beam E
//	TH1F *bpmTargetY=new TH1F("BeamTargetEY","BeamPosEY",100,-0.5,0.5);


	chain->Project(bpmAX->GetName(),"IPM1H04A.XPOS");
	chain->Project(bpmAY->GetName(),"IPM1H04A.YPOS");

	chain->Project(bpmBX->GetName(),"IPM1H04B.XPOS");

	chain->Project(bpmCX->GetName(),"IPM1H04C.XPOS");

	chain->Project(bpmDX->GetName(),"IPM1H04D.XPOS");


	chain->Project(bpmEX->GetName(),"IPM1H04E.XPOS");

	chain->Project(bpmEY->GetName(),"IPM1H04E.YPOS");

	chain->Project(bpmTargetX->GetName(), "IPM1H04A.XPOS-IPM1H04E.XPOS");

	bpmAX->SetLineColor(2);
	bpmBX->SetLineColor(3);
	bpmCX->SetLineColor(4);
	bpmDX->SetLineColor(5);
	bpmEX->SetLineColor(6);
//	bpmAX->SetLineColor(2);
//	bpmAY->SetLineColor(3);
//	bpmEX->SetLineColor(4);
//	bpmEY->SetLineColor(6);

	TCanvas *beamPosition = new TCanvas("Beam Pos check", "Beam Pos check", 1100,1000);
	beamPosition->cd();
	bpmAX->Draw();
	bpmBX->Draw("same");
	bpmCX->Draw("same");
	bpmDX->Draw("same");
	bpmEX->Draw("same");

	beamPosition->Draw();
	gPad->BuildLegend();

}


int plotErrorsL(){
	auto hrsangleCanv=new TCanvas("HRS Angle","HRS Angle",200,10,600,400);
	  TMultiGraph *mg = new TMultiGraph();
	   mg->SetTitle("Exclusion graphs");

	TLegend *lgend=new TLegend(0.3,0.3);
	double prex_x[]={-1,0,1};
	double prex_y[]={4.819,4.792,4.756};
	double prex_ex[]={0.0,0.0,0.0};
	double prex_ey[]={0.153,0.151,0.151};

	double crex_x[]={-1+0.1,0+0.1,1+0.1};
	double crex_y[]={4.755,4.779,4.791};
	double crex_ex[]={0.0,0.0,0.0};
	double crex_ey[]={0.029,0.029,0.029};


	double crex2_x[]={0-0.2,0-0.1,0+0.2,0+0.3,0+0.4};
	double crex2_y[]={4.775, 4.835, 4.831,4.710,4.779};
	double crex2_ex[]={0.0,0.0,0.0,0.0,0.0};
	double crex2_ey[]={0.036,0.034,0.037,0.037,0.037};

    double crex3_x[]={0-0.25, 0-0.15, 0+0.25, 0+0.35, 0+0.45, 0+0.55, 1+0.2, -1.7 };
    double crex3_y[]={4.717,  4.830,  4.831,  4.728,  4.760,  4.787,  4.800, 4.737 };
    double crex3_ex[]={0.0,   0.0,    0.0,    0.0,    0.0,    0.0,    0.0,   0.0};
    double crex3_ey[]={0.036, 0.034,  0.037,  0.037,  0.037,  0.037,  0.037, 0.037};



    auto geprex=new TGraphErrors(5,crex_x,crex_y,crex_ex,crex_ey);
	geprex->GetYaxis()->SetRangeUser(4.5,5.1);
	geprex->GetXaxis()->SetRangeUser(-2,2);

	geprex->SetTitle("PRex/CRex LHRS Pointing Measurement");
	geprex->GetXaxis()->SetTitle("Dp Scan");
	geprex->GetYaxis()->SetTitle("HRS Angle(Degree)");
	geprex->SetLineWidth(2);
	geprex->SetLineColor(6);
	geprex->SetMarkerStyle(20);
	geprex->SetMarkerColor(6);
	geprex->Draw("ap");
	lgend->AddEntry(geprex,Form("CRex LHRS"));

	TLine *line=new TLine(-2,4.7469,2,4.7469);
	line->SetLineWidth(2);
	line->SetLineColor(3);
	line->Draw("same");


	TLatex *text1= new TLatex(-2,4.7469+0.07,Form("Survey: 4.7469 + 0.06#circ"));

	TLatex *text2= new TLatex(-2,4.7469-0.09,Form("Survey: 4.7469 - 0.06#circ"));

	text1->Draw("same");
	text2->Draw("same");

	TLine *line1=new TLine(-2,4.7469+0.06,2,4.7469+0.06);
		line1->SetLineWidth(2);
		line1->SetLineColor(93);
		line1->Draw("same");

		TLine *line2=new TLine(-2,4.7469-0.06,2,4.7469-0.06);
			line2->SetLineWidth(2);
			line2->SetLineColor(93);
			line2->Draw("same");

	auto gecrex=new TGraphErrors(5,prex_x,prex_y,prex_ex,prex_ey);
	gecrex->GetYaxis()->SetRangeUser(4.5,5.1);
	gecrex->SetLineWidth(2);
	gecrex->SetLineColor(46);
	gecrex->SetMarkerStyle(20);
	gecrex->SetMarkerColor(46);
	gecrex->Draw("p same");
	lgend->AddEntry(gecrex,Form("PRex LHRS"));
	lgend->Draw("same");


	auto gecrex2=new TGraphErrors(5,crex2_x,crex2_y,crex2_ex,crex2_ey);
	gecrex2->GetYaxis()->SetRangeUser(4.5,5.1);
	gecrex2->SetLineWidth(2);
	gecrex2->SetLineColor(56);
	gecrex2->SetMarkerStyle(20);
	gecrex2->SetMarkerColor(56);
	gecrex2->Draw("p same");
	lgend->AddEntry(gecrex2,Form("CRex2 LHRS"));
	lgend->Draw("same");


	int number = sizeof(crex3_y)/sizeof(double );
    auto gecrex3=new TGraphErrors(8,crex3_x,crex3_y,crex3_ex,crex3_ey);
    gecrex3->GetYaxis()->SetRangeUser(4.5,5.1);
    gecrex3->SetLineWidth(2);
    gecrex3->SetLineColor(70);
    gecrex3->SetMarkerStyle(20);
    gecrex3->SetMarkerColor(70);
    gecrex3->Draw("p same");
    lgend->AddEntry(gecrex3,Form("CRex3 LHRS"));
    lgend->Draw("same");


	hrsangleCanv->Update();

return 1;
}


int plotErrorsCorrectL(){
    auto hrsangleCanv=new TCanvas("HRS Angle","HRS Angle",200,10,600,400);
    TMultiGraph *mg = new TMultiGraph();
    mg->SetTitle("Exclusion graphs");

    TLegend *lgend=new TLegend(0.3,0.3);
    double prex_x[]={-1,0,1};
    double prex_y[]={4.819,4.792,4.756};
    double prex_ex[]={0.0,0.0,0.0};
    double prex_ey[]={0.153,0.151,0.151};

//    double crex_x[]={-1+0.1,0+0.1,1+0.1};
//    double crex_y[]={4.755,4.779,4.791};
//    double crex_ex[]={0.0,0.0,0.0};
//    double crex_ey[]={0.029,0.029,0.029};


    double crex2_x[]={0-0.2,0-0.1,0+0.2,0+0.3,0+0.4,-1+0.1};
    double crex2_y[]={4.831, 4.834, 4.831,4.822,4.84,4.815};
    double crex2_ex[]={0.0,0.0,0.0,0.0,0.0,0.0};
    double crex2_ey[]={0.036,0.034,0.037,0.037,0.037,0.037};

    double crex_x[]={0-0.25, 0-0.15, 0+0.25, 0+0.35, 0+0.45,  1+0.2, -1.7 };
    double crex_y[]={4.831,  4.834,  4.829,  4.821,  4.831,    4.861, 4.798 };
    double crex_ex[]={0.0,   0.0,    0.0,    0.0,    0.0,      0.0,   0.0};
    double crex_ey[]={0.036, 0.034,  0.037,  0.037,  0.037,    0.037, 0.037};



    auto geprex=new TGraphErrors(8,crex_x,crex_y,crex_ex,crex_ey);
    geprex->GetYaxis()->SetRangeUser(4.5,5.1);
    geprex->GetXaxis()->SetRangeUser(-2,2);

    geprex->SetTitle("PRex/CRex LHRS Pointing Measurement");
    geprex->GetXaxis()->SetTitle("Dp Scan");
    geprex->GetYaxis()->SetTitle("HRS Angle(Degree)");
    geprex->SetLineWidth(2);
    geprex->SetLineColor(6);
    geprex->SetMarkerStyle(20);
    geprex->SetMarkerColor(6);
    geprex->Draw("ap");
    lgend->AddEntry(geprex,Form("CRex LHRS"));

    TLine *line=new TLine(-2,4.7469,1.5,4.7469);
    line->SetLineWidth(2);
    line->SetLineColor(3);
    line->Draw("same");


    TLatex *text1= new TLatex(-2,4.7469+0.07,Form("Survey: 4.7469 + 0.06#circ"));

    TLatex *text2= new TLatex(-2,4.7469-0.09,Form("Survey: 4.7469 - 0.06#circ"));

    text1->Draw("same");
    text2->Draw("same");

    TLine *line1=new TLine(-2,4.7469+0.06,1.5,4.7469+0.06);
    line1->SetLineWidth(2);
    line1->SetLineColor(93);
    line1->Draw("same");

    TLine *line2=new TLine(-2,4.7469-0.06,1.5,4.7469-0.06);
    line2->SetLineWidth(2);
    line2->SetLineColor(93);
    line2->Draw("same");

//    auto gecrex=new TGraphErrors(0,prex_x,prex_y,prex_ex,prex_ey);
//    gecrex->GetYaxis()->SetRangeUser(4.5,5.1);
//    gecrex->SetLineWidth(2);
//    gecrex->SetLineColor(46);
//    gecrex->SetMarkerStyle(20);
//    gecrex->SetMarkerColor(46);
//    gecrex->Draw("p same");
//    lgend->AddEntry(gecrex,Form("PRex LHRS"));
//    lgend->Draw("same");


    auto gecrex2=new TGraphErrors(5,crex2_x,crex2_y,crex2_ex,crex2_ey);
    gecrex2->GetYaxis()->SetRangeUser(4.5,5.1);
    gecrex2->SetLineWidth(2);
    gecrex2->SetLineColor(56);
    gecrex2->SetMarkerStyle(20);
    gecrex2->SetMarkerColor(56);
    gecrex2->Draw("p same");
    lgend->AddEntry(gecrex2,Form("CRex2 LHRS"));
    lgend->Draw("same");


/*    int number = sizeof(crex3_y)/sizeof(double );
    auto gecrex3=new TGraphErrors(8,crex3_x,crex3_y,crex3_ex,crex3_ey);
    gecrex3->GetYaxis()->SetRangeUser(4.5,5.1);
    gecrex3->SetLineWidth(2);
    gecrex3->SetLineColor(70);
    gecrex3->SetMarkerStyle(20);
    gecrex3->SetMarkerColor(70);
    gecrex3->Draw("p same");
    lgend->AddEntry(gecrex3,Form("CRex3 LHRS"));
    lgend->Draw("same");*/


    hrsangleCanv->Update();

    return 1;
}

int plotErrorsCorrectR(){
    auto hrsangleCanv=new TCanvas("HRS Angle","HRS Angle",200,10,600,400);
    TMultiGraph *mg = new TMultiGraph();
    mg->SetTitle("Exclusion graphs");

    TLegend *lgend=new TLegend(0.3,0.3);
    double prex_x[]={-1,0,1};
    double prex_y[]={4.748,4.84,4.846};
    double prex_ex[]={0.0,0.0,0.0};
    double prex_ey[]={0.153,0.151,0.151};

//    double crex_x[]={-1+0.1,0+0.1,1+0.1};
//    double crex_y[]={4.750,4.718,4.77};
//    double crex_ex[]={0.0,0.0,0.0};
//    double crex_ey[]={0.029,0.029,0.029};

    double crex2_x[]={0-0.2,0-0.1,0+0.2,0+0.3,0+0.4};
    double crex2_y[]={4.728, 4.731, 4.744,4.744,4.733};
    double crex2_ex[]={0.0,0.0,0.0,0.0,0.0};
    double crex2_ey[]={0.032,0.033,0.032,0.032,0.032};

    double crex_x[]={0-0.25, 0-0.15, 0+0.25, 0+0.35, 0+0.45, 1+0.2, -1.7 };
    double crex_y[]={4.732,  4.738,  4.729,  4.75,  4.732,  4.719,  4.74 };
    double crex_ex[]={0.0,   0.0,    0.0,    0.0,    0.0,    0.0,    0.0 };
    double crex_ey[]={0.036, 0.034,  0.037,  0.037,  0.037,  0.037,  0.037};


    auto geprex=new TGraphErrors(8,crex_x,crex_y,crex_ex,crex_ey);
    geprex->GetYaxis()->SetRangeUser(4.5,5.1);
    geprex->GetXaxis()->SetRangeUser(-2,2);
    geprex->SetTitle("PRex/CRex RHRS Pointing Measurement");
    geprex->GetXaxis()->SetTitle("Dp Scan");
    geprex->GetYaxis()->SetTitle("HRS Angle(Degree)");
    geprex->SetLineWidth(2);
    geprex->SetLineColor(6);
    geprex->SetMarkerStyle(20);
    geprex->SetMarkerColor(6);
    geprex->Draw("ap");
    lgend->AddEntry(geprex,Form("CRex HRS"));


//    auto gecrex=new TGraphErrors(0,prex_x,prex_y,prex_ex,prex_ey);
//    gecrex->GetYaxis()->SetRangeUser(4.5,5.1);
//    gecrex->SetLineWidth(2);
//    gecrex->SetLineColor(46);
//    gecrex->SetMarkerStyle(20);
//    gecrex->SetMarkerColor(46);
//    gecrex->Draw("p same");
//    lgend->AddEntry(gecrex,Form("PRex RHRS"));
//    lgend->Draw("same");


    auto gecrex2=new TGraphErrors(5,crex2_x,crex2_y,crex2_ex,crex2_ey);
    gecrex2->GetYaxis()->SetRangeUser(4.5,5.1);
    gecrex2->SetLineWidth(2);
    gecrex2->SetLineColor(56);
    gecrex2->SetMarkerStyle(20);
    gecrex2->SetMarkerColor(56);
    gecrex2->Draw("p same");
    lgend->AddEntry(gecrex2,Form("CRex2 RHRS"));
    lgend->Draw("same");

//    auto gecrex3=new TGraphErrors(8,crex3_x,crex3_y,crex3_ex,crex3_ey);
//    gecrex3->GetYaxis()->SetRangeUser(4.5,5.1);
//    gecrex3->SetLineWidth(2);
//    gecrex3->SetLineColor(70);
//    gecrex3->SetMarkerStyle(20);
//    gecrex3->SetMarkerColor(70);
//    gecrex3->Draw("p same");
//    lgend->AddEntry(gecrex3,Form("CRex3 RHRS"));
//    lgend->Draw("same");

    TLine *line=new TLine(-2,4.7572,1.5,4.7572);
    line->SetLineWidth(2);
    line->SetLineColor(3);
    line->Draw("same");


    TLatex *text1= new TLatex(-2,4.7572+0.07,Form("Survey: 4.7572 + 0.06"));

    TLatex *text2= new TLatex(-2,4.7572-0.09,Form("Survey: 4.7572 - 0.06"));

    text1->Draw("same");
    text2->Draw("same");

    TLine *line1=new TLine(-2,4.7572+0.06,1.5,4.7572+0.06);
    line1->SetLineWidth(2);
    line1->SetLineColor(93);
    line1->Draw("same");

    TLine *line2=new TLine(-2,4.7572-0.06,1.5,4.7572-0.06);
    line2->SetLineWidth(2);
    line2->SetLineColor(93);
    line2->Draw("same");

    hrsangleCanv->Update();

    return 1;
}


int plotErrorsR(){
	auto hrsangleCanv=new TCanvas("HRS Angle","HRS Angle",200,10,600,400);
	TMultiGraph *mg = new TMultiGraph();
	mg->SetTitle("Exclusion graphs");

	TLegend *lgend=new TLegend(0.3,0.3);
	double prex_x[]={-1,0,1};
	double prex_y[]={4.748,4.84,4.846};
	double prex_ex[]={0.0,0.0,0.0};
	double prex_ey[]={0.153,0.151,0.151};

	double crex_x[]={-1+0.1,0+0.1,1+0.1};
	double crex_y[]={4.750,4.78,4.77};
	double crex_ex[]={0.0,0.0,0.0};
	double crex_ey[]={0.029,0.029,0.029};

	double crex2_x[]={0-0.2,0-0.1,0+0.2,0+0.3,0+0.4};
	double crex2_y[]={4.840, 4.730, 4.742,4.853,4.787};
	double crex2_ex[]={0.0,0.0,0.0,0.0,0.0};
	double crex2_ey[]={0.032,0.033,0.032,0.032,0.032};

    double crex3_x[]={0-0.25, 0-0.15, 0+0.25, 0+0.35, 0+0.45, 0+0.55, 1+0.2, -1.7 };
    double crex3_y[]={4.794,  4.730,  4.840,  4.795,  4.795,  4.812,  4.794, 4.737 };
    double crex3_ex[]={0.0,   0.0,    0.0,    0.0,    0.0,    0.0,    0.0,   0.0};
    double crex3_ey[]={0.036, 0.034,  0.037,  0.037,  0.037,  0.037,  0.037, 0.037};


    auto geprex=new TGraphErrors(5,crex_x,crex_y,crex_ex,crex_ey);
	geprex->GetYaxis()->SetRangeUser(4.5,5.1);
	geprex->GetXaxis()->SetRangeUser(-2,2);
	geprex->SetTitle("PRex/CRex RHRS Pointing Measurement");
	geprex->GetXaxis()->SetTitle("Dp Scan");
	geprex->GetYaxis()->SetTitle("HRS Angle(Degree)");
	geprex->SetLineWidth(2);
	geprex->SetLineColor(6);
	geprex->SetMarkerStyle(20);
	geprex->SetMarkerColor(6);
	geprex->Draw("ap");
	lgend->AddEntry(geprex,Form("CRex HRS"));


	auto gecrex=new TGraphErrors(5,prex_x,prex_y,prex_ex,prex_ey);
	gecrex->GetYaxis()->SetRangeUser(4.5,5.1);
	gecrex->SetLineWidth(2);
	gecrex->SetLineColor(46);
	gecrex->SetMarkerStyle(20);
	gecrex->SetMarkerColor(46);
	gecrex->Draw("p same");
	lgend->AddEntry(gecrex,Form("PRex RHRS"));
	lgend->Draw("same");


	auto gecrex2=new TGraphErrors(5,crex2_x,crex2_y,crex2_ex,crex2_ey);
	gecrex2->GetYaxis()->SetRangeUser(4.5,5.1);
	gecrex2->SetLineWidth(2);
	gecrex2->SetLineColor(56);
	gecrex2->SetMarkerStyle(20);
	gecrex2->SetMarkerColor(56);
	gecrex2->Draw("p same");
	lgend->AddEntry(gecrex2,Form("CRex2 RHRS"));
	lgend->Draw("same");

    auto gecrex3=new TGraphErrors(8,crex3_x,crex3_y,crex3_ex,crex3_ey);
    gecrex3->GetYaxis()->SetRangeUser(4.5,5.1);
    gecrex3->SetLineWidth(2);
    gecrex3->SetLineColor(70);
    gecrex3->SetMarkerStyle(20);
    gecrex3->SetMarkerColor(70);
    gecrex3->Draw("p same");
    lgend->AddEntry(gecrex3,Form("CRex3 LHRS"));
    lgend->Draw("same");

	TLine *line=new TLine(-2,4.7572,2,4.7572);
	line->SetLineWidth(2);
	line->SetLineColor(3);
	line->Draw("same");


	TLatex *text1= new TLatex(-2,4.7572+0.07,Form("Survey: 4.7572 + 0.06"));

	TLatex *text2= new TLatex(-2,4.7572-0.09,Form("Survey: 4.7572 - 0.06"));

	text1->Draw("same");
	text2->Draw("same");

	TLine *line1=new TLine(-2,4.7572+0.06,2,4.7572+0.06);
	line1->SetLineWidth(2);
	line1->SetLineColor(93);
	line1->Draw("same");

	TLine *line2=new TLine(-2,4.7572-0.06,2,4.7572-0.06);
	line2->SetLineWidth(2);
	line2->SetLineColor(93);
	line2->Draw("same");

	hrsangleCanv->Update();

return 1;
}

int plotErrorsAverage(){

	auto hrsangleCanv=new TCanvas("HRS Angle","HRS Angle",200,10,600,400);
	TMultiGraph *mg = new TMultiGraph();
	mg->SetTitle("Exclusion graphs");

	TLegend *lgend=new TLegend(0.3,0.3);
	double prex_x[]={-1,0,1};
	double prex_y[]={4.748,4.84,4.846};
	for (int i = 0; i < 3 ; i ++){
		double temp[]={4.819,4.792,4.756};
		prex_y[i]=0.5*(prex_y[i]+temp[i]);
	}
	double prex_ex[]={0.0,0.0,0.0};
	double prex_ey[]={0.153,0.151,0.151};

	double crex_x[]={-1+0.1,0+0.1,1+0.1};
	double crex_y[]={4.750,4.78,4.77};
	for (int i = 0; i < 3 ; i ++){
			double temp[]={4.755,4.779,4.791};
			crex_y[i]=0.5*(crex_y[i]+temp[i]);
		}
	double crex_ex[]={0.0,0.0,0.0};
	double crex_ey[]={0.029,0.029,0.029};


	double crex2_x[]={0-0.2,0-0.1,0+0.2,0+0.3,0+0.4};
	double crex2_y[]={4.840, 4.730, 4.742,4.853,4.787};
	for (int i = 0; i < 5 ; i ++){
				double temp[]={4.717, 4.830, 4.831,4.710,4.779};
				crex2_y[i]=0.5*(crex2_y[i]+temp[i]);
			}
	double crex2_ex[]={0.0,0.0,0.0,0.0,0.0};
	double crex2_ey[]={0.032,0.033,0.032,0.032,0.032};


    double crex3_x[]={0-0.25, 0-0.15, 0+0.25, 0+0.35, 0+0.45, 0+0.55, 1+0.2, -1.7 };
    double crex3_y[]={4.794,  4.730,  4.840,  4.795,  4.795,  4.812,  4.794, 4.737 };
    for (int i = 0; i < 5 ; i ++){
        double temp[]={4.717,  4.830,  4.831,  4.728,  4.760,  4.787,  4.800, 4.737};
        crex3_y[i]=0.5*(crex2_y[i]+temp[i]);
    }
    double crex3_ex[]={0.0,   0.0,    0.0,    0.0,    0.0,    0.0,    0.0,   0.0};
    double crex3_ey[]={0.036, 0.034,  0.037,  0.037,  0.037,  0.037,  0.037, 0.037};



    auto geprex=new TGraphErrors(5,crex_x,crex_y,crex_ex,crex_ey);
	geprex->GetYaxis()->SetRangeUser(4.5,5.1);
	geprex->GetXaxis()->SetRangeUser(-2,2);
	geprex->SetTitle("PRex/CRex (RHRS+LHRS)/2 Pointing Measurement");
	geprex->GetXaxis()->SetTitle("Dp Scan");
	geprex->GetYaxis()->SetTitle("HRS Angle(Degree)");
	geprex->SetLineWidth(2);
	geprex->SetLineColor(6);
	geprex->SetMarkerStyle(20);
	geprex->SetMarkerColor(6);
	geprex->Draw("ap");
	lgend->AddEntry(geprex,Form("CRex Average"));


	auto gecrex=new TGraphErrors(5,prex_x,prex_y,prex_ex,prex_ey);
	gecrex->GetYaxis()->SetRangeUser(4.5,5.1);
	gecrex->SetLineWidth(2);
	gecrex->SetLineColor(46);
	gecrex->SetMarkerStyle(20);
	gecrex->SetMarkerColor(46);
	gecrex->Draw("p same");
	lgend->AddEntry(gecrex,Form("PRex Average"));
	lgend->Draw("same");


	auto gecrex2=new TGraphErrors(5,crex2_x,crex2_y,crex2_ex,crex2_ey);
	gecrex2->GetYaxis()->SetRangeUser(4.5,5.1);
	gecrex2->SetLineWidth(2);
	gecrex2->SetLineColor(56);
	gecrex2->SetMarkerStyle(20);
	gecrex2->SetMarkerColor(56);
	gecrex2->Draw("p same");
	lgend->AddEntry(gecrex2,Form("CRex2 Average"));
	lgend->Draw("same");


    auto gecrex3=new TGraphErrors(8,crex3_x,crex3_y,crex3_ex,crex3_ey);
    gecrex3->GetYaxis()->SetRangeUser(4.5,5.1);
    gecrex3->SetLineWidth(2);
    gecrex3->SetLineColor(70);
    gecrex3->SetMarkerStyle(20);
    gecrex3->SetMarkerColor(70);
    gecrex3->Draw("p same");
    lgend->AddEntry(gecrex3,Form("CRex3 LHRS"));
    lgend->Draw("same");


	TLine *line=new TLine(-2,4.7572,2,4.7572);
	line->SetLineWidth(2);
	line->SetLineColor(3);
	line->Draw("same");


	TLatex *text1= new TLatex(-2,4.7572+0.07,Form("Survey: 4.7572 + 0.06"));

	TLatex *text2= new TLatex(-2,4.7572-0.09,Form("Survey: 4.7572 - 0.06"));

	text1->Draw("same");
	text2->Draw("same");

	TLine *line1=new TLine(-2,4.7572+0.06,2,4.7572+0.06);
	line1->SetLineWidth(2);
	line1->SetLineColor(93);
	line1->Draw("same");

	TLine *line2=new TLine(-2,4.7572-0.06,2,4.7572-0.06);
	line2->SetLineWidth(2);
	line2->SetLineColor(93);
	line2->Draw("same");

	hrsangleCanv->Update();

return 1;

}

int plotErrors(){
	auto hrsangleCanv=new TCanvas("HRS Angle","HRS Angle",200,10,600,400);
	TMultiGraph *mg = new TMultiGraph();
	mg->SetTitle("Exclusion graphs");

	TLegend *lgend=new TLegend(0.3,0.3);
	double prex_x[]={-1,0,1};
	double prex_y[]={4.748,4.84,4.846};
	double prex_ex[]={0.0,0.0,0.0};
	double prex_ey[]={0.153,0.151,0.151};

	double crex_x[5];
	double crex_y[5];
	double crex_ex[5];
	double crex_ey[5];


	double crex2R_x[]={0-0.2,0-0.1,0+0.2,0+0.3,0+0.4};
	double crex2R_y[]={4.840, 4.730, 4.742,4.853,4.787};
	double crex2R_ex[]={0.0,0.0,0.0,0.0,0.0};
	double crex2R_ey[]={0.032,0.033,0.032,0.032,0.032};

	double crex2L_x[]={0-0.2,0-0.1,0+0.2,0+0.3,0+0.4};
	double crex2L_y[]={4.717, 4.830, 4.831,4.710,4.779};
	double crex2L_ex[]={0.0,0.0,0.0,0.0,0.0};
	double crex2L_ey[]={0.036,0.034,0.037,0.037,0.037};

	for(int i =0; i < 5; i ++){
		crex_x[i]=crex2L_x[i];
		crex_y[i]=(crex2L_y[i]+crex2R_y[i])/2.0;
		crex_ex[i]=crex2R_ex[i];
		crex_ey[i]=crex2R_ey[i];
	}

	auto geprex=new TGraphErrors(5,crex_x,crex_y,crex_ex,crex_ey);
	geprex->GetYaxis()->SetRangeUser(4.5,5.1);
	geprex->GetXaxis()->SetRangeUser(-2,2);
	geprex->SetTitle("PRex/CRex RHRS Pointing Measurement");
	geprex->GetXaxis()->SetTitle("Dp Scan");
	geprex->GetYaxis()->SetTitle("HRS Angle(Degree)");
	geprex->SetLineWidth(2);
	geprex->SetLineColor(6);
	geprex->SetMarkerStyle(20);
	geprex->SetMarkerColor(6);
	geprex->Draw("ap");
	lgend->AddEntry(geprex,Form("CRex HRS"));


//	auto gecrex=new TGraphErrors(5,prex_x,prex_y,prex_ex,prex_ey);
//	gecrex->GetYaxis()->SetRangeUser(4.5,5.1);
//	gecrex->SetLineWidth(2);
//	gecrex->SetLineColor(46);
//	gecrex->SetMarkerStyle(20);
//	gecrex->SetMarkerColor(46);
//	gecrex->Draw("p same");
//	lgend->AddEntry(gecrex,Form("PRex RHRS"));
//	lgend->Draw("same");
//
//
//	auto gecrex2=new TGraphErrors(5,crex2_x,crex2_y,crex2_ex,crex2_ey);
//	gecrex2->GetYaxis()->SetRangeUser(4.5,5.1);
//	gecrex2->SetLineWidth(2);
//	gecrex2->SetLineColor(56);
//	gecrex2->SetMarkerStyle(20);
//	gecrex2->SetMarkerColor(56);
//	gecrex2->Draw("p same");
//	lgend->AddEntry(gecrex2,Form("CRex2 RHRS"));
//	lgend->Draw("same");
//


	TLine *line=new TLine(-2,4.7572,2,4.7572);
	line->SetLineWidth(2);
	line->SetLineColor(3);
	line->Draw("same");


	TLatex *text1= new TLatex(-2,4.7572+0.07,Form("Survey: 4.7572 + 0.06"));

	TLatex *text2= new TLatex(-2,4.7572-0.09,Form("Survey: 4.7572 - 0.06"));

	text1->Draw("same");
	text2->Draw("same");

	TLine *line1=new TLine(-2,4.7572+0.06,2,4.7572+0.06);
	line1->SetLineWidth(2);
	line1->SetLineColor(93);
	line1->Draw("same");

	TLine *line2=new TLine(-2,4.7572-0.06,2,4.7572-0.06);
	line2->SetLineWidth(2);
	line2->SetLineColor(93);
	line2->Draw("same");

	hrsangleCanv->Update();

return 1;
}

unsigned int ColorRequestCounter=0;
int requestColor(){
    int color[]={6,46,56};
    ColorRequestCounter=ColorRequestCounter+1;
    return color[ColorRequestCounter-1];
}

TCanvas *plotHRSCanv(std::map<std::string,std::vector<std::vector<double>>> data,TString HRS="RHRS"){

    auto hrsangleCanv=new TCanvas("HRS Angle","HRS Angle",200,10,600,400);
    TMultiGraph *mg = new TMultiGraph();
    mg->SetTitle("Exclusion graphs");


    TLegend *lgend=new TLegend(0.3,0.3);

    for (auto iter=data.begin(); iter!=data.end();iter++){
        TString title=iter->first;
        const int elemenCount=iter->second.size();
        double x_id[elemenCount];
        double x_err[elemenCount];
        double y_value[elemenCount];
        double y_err[elemenCount];


        for(unsigned int counter=0;counter<iter->second.size();counter++){
            x_id[counter]=iter->second[counter][0];
            x_err[counter]=iter->second[counter][1];
            y_value[counter]=iter->second[counter][2];
            y_err[counter]=iter->second[counter][3];
        }

        for (int i =0 ; i < iter->second.size();i++){
           std::cout<<x_id[i]<<" ";
        }
        std::cout<<std::endl;

        for (int i =0 ; i < iter->second.size();i++){
            std::cout<<x_err[i]<<" ";
        }
        std::cout<<std::endl;


        auto prexplot=new TGraphErrors(iter->second.size(),x_id,y_value,x_err,y_err);
        prexplot->GetYaxis()->SetRangeUser(4.5,5.1);
        prexplot->GetXaxis()->SetLimits(-2,2);


        prexplot->GetXaxis()->SetTitle("Dp Scan");
        prexplot->GetYaxis()->SetTitle("HRS Angle(Degree)");
        prexplot->SetLineWidth(2);
        auto color=requestColor();
        prexplot->SetLineColor(color);
        prexplot->SetMarkerStyle(20);
        prexplot->SetMarkerColor(color);
        lgend->AddEntry(prexplot,title.Data());
        if(iter==data.begin())
        {
            prexplot->SetTitle(Form("PRex/CRex %s Pointing Measurement",HRS.Data()));
            prexplot->Draw("ap");
        }else{
            prexplot->Draw("p same");
        }
    }

    {
        double survey = 4.7469;
        if (HRS == "RHRS"){
            survey=4.7572;
        }
        TLine *line=new TLine(-1.9,survey,1.9,survey);
        line->SetLineWidth(2);
        line->SetLineColor(3);
        line->Draw("same");


        TLatex *text1= new TLatex(-2,survey+0.07,Form("Survey: %f + 0.06#circ",survey));

        TLatex *text2= new TLatex(-2,survey-0.09,Form("Survey: %f - 0.06#circ",survey));

        text1->Draw("same");
        text2->Draw("same");

        TLine *line1=new TLine(-1.9,survey+0.06,1.9,survey+0.06);
        line1->SetLineWidth(2);
        line1->SetLineColor(93);
        line1->Draw("same");

        TLine *line2=new TLine(-1.9,survey-0.06,1.9,survey-0.06);
        line2->SetLineWidth(2);
        line2->SetLineColor(93);
        line2->Draw("same");
    }

    lgend->Draw("same");
    return hrsangleCanv;
}

int plotError(TString csvfname="./crex_pointing.csv"){

    if (gSystem->AccessPathName(csvfname.Data())){
        std::cout<<"\033[1;33m [Warning]\033[0m Missing csv file::"<<csvfname.Data()<<std::endl;
        return  -1;
    }

    //load the data information
    std::ifstream csvStream(csvfname.Data());

    std::string line;

    std::map<std::string,std::vector<std::vector<double>>> pointDataR;
    std::map<std::string,std::vector<std::vector<double>>> pointDataL;

    while (std::getline(csvStream,line)){
        std::istringstream  s(line);
        std::string field;

        std::vector<double> LData;
        std::vector<double> RData;

        getline(s,field,',');
        TString  title=field;
        if(title.Contains("CRex_")) {
            getline(s, field, ',');
            LData.push_back( std::stof(field.c_str()));
            RData.push_back(std::stof(field.c_str()));

            getline(s, field, ',');
            LData.push_back( std::stof(field.c_str()));
            RData.push_back(std::stof(field.c_str()));

            getline(s, field, ',');
            LData.push_back( std::stof(field.c_str()));

            getline(s, field, ',');
            LData.push_back( std::stof(field.c_str()));

            getline(s, field, ',');
            RData.push_back(std::stof(field.c_str()));

            getline(s, field, ',');
            RData.push_back(std::stof(field.c_str()));
        }

//        std::cout<<title.Data()<<","<<LData[0]<<","<<LData[1]<<","<<LData[2]<<","<<LData[3]<<"::"<<RData[0]<<","<<RData[1]<<","<<RData[2]<<","<<RData[3]<<std::endl;
        if(title.Contains("CRex_")) {
        pointDataR[title.Data()].push_back(RData);
        pointDataL[title.Data()].push_back(LData);
        }
    }


    plotHRSCanv(pointDataL,"LHRS");
//    plotHRSCanv(pointDataR,"RHRS");



    return  1;

}