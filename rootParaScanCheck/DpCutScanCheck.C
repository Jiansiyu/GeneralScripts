/*
 *
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
#include <TLegend.h>
#include <TApplication.h>
#include <TArrow.h>
#include "TBenchmark.h"
//////////////////////////////////////////////////////////////////////////////
// Work Directory
// cut options
// Need to change
//////////////////////////////////////////////////////////////////////////////
TString prepcut;
TString generalcut;
TString generalcutR="R.tr.n==1 && R.vdc.u1.nclust==1&& R.vdc.v1.nclust==1 && R.vdc.u2.nclust==1 && R.vdc.v2.nclust==1 && R.gold.p > 2.14 && R.gold.p < 2.2  ";
TString generalcutL="L.tr.n==1 && L.vdc.u1.nclust==1&& L.vdc.v1.nclust==1 && L.vdc.u2.nclust==1 && L.vdc.v2.nclust==1  && L.gold.p > 2.14 && L.gold.p < 2.2";

inline Bool_t IsFileExist (const std::string& name) {
        return !gSystem->AccessPathName(name.c_str());
}


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
    return chain;
}

void DpCutScanCheck(unsigned int runID, TString folder="/home/newdriver/Storage/Research/CRex_Experiment/RasterReplay/Replay/Result"){
     auto chain = LoadrootFile(runID,folder.Data());

     TString HRS("L");
     if (runID>20000) HRS="R";
     // scan
     TCanvas *canv=new TCanvas(Form("canv_%d",runID),Form("canv_%d",runID),1960,1080);

     int qadcThreshold=470;
     // generate the plot
    TH2F *thDphh;
    TH2F *thDpCuthh;
    TH1F *qadch;
    TH1F *targx;
    gSystem->Unlink(Form("anim_run%d.gif",runID)); // delete old file
    gStyle->SetCanvasPreferGL(true);

    gBenchmark->Start("hsum");

    int counter=0;
    for (;qadcThreshold<520;qadcThreshold+=2)
    {
        canv->Divide(1,2);
        canv->cd(1)->Divide(2,1);
        canv->cd(2)->Divide(2,1);


        thDphh=new TH2F(Form("run%d_th.vs.dp",runID),Form("run%d_th.vs.dp",runID),200,-0.014,0.0,200,-0.1,0.1);
        thDphh->GetXaxis()->SetTitle("gold.dp");
        thDphh->GetYaxis()->SetTitle("gold.th");

        thDpCuthh=new TH2F(Form("run%d_th.vs.dp cut",runID),Form("run%d_th.vs.dp cut",runID),200,-0.014,0.0,200,-0.1,0.1);
        thDpCuthh->GetXaxis()->SetTitle("gold.dp");
        thDpCuthh->GetYaxis()->SetTitle("gold.th");

        qadch=new TH1F(Form("upQadc%d",runID),Form("upQadc%d",runID),200,400,600);

        targx=new TH1F(Form("targx"),Form("targx"),500,-5,5);

        chain->Project(targx->GetName(),"targx");
        targx->GetXaxis()->SetRangeUser(targx->GetBinCenter(targx->GetMaximumBin())-2,targx->GetBinCenter(targx->GetMaximumBin())+2);
        chain->Project(thDphh->GetName(),Form("%s.gold.th:%s.gold.dp",HRS.Data(),HRS.Data()));
        chain->Project(thDpCuthh->GetName(),Form("%s.gold.th:%s.gold.dp",HRS.Data(),HRS.Data()),Form("P.upQadc%s<%d",HRS.Data(),qadcThreshold));
        chain->Project(qadch->GetName(),Form("P.upQadc%s",HRS.Data()));

        canv->cd(1)->cd(2);
        thDphh->Draw("zcol");
        canv->cd(1)->cd(1);
        thDpCuthh->Draw("zcol");
        canv->cd(2)->cd(1);
        qadch->Draw();
        
        TLine *line=new TLine(qadcThreshold,0, qadcThreshold,qadch->GetMaximum());
        line->SetLineWidth(2);
        line->Draw("same");

        canv->cd(2)->cd(2);
        targx->Draw("same");

        canv->Update();
        if(gROOT->IsBatch())canv->Print(Form("anim_run%d.gif+%d",runID,counter));

        counter+=1;
        thDphh->~TH2F();
        thDpCuthh->~TH2F();
        qadch->~TH1F();
        targx->~TH1F();
        canv->Clear();
    }
    gBenchmark->Show("hsum");
}