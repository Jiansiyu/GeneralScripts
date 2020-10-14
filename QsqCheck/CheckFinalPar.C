/*
 *
 */
#include "TROOT.h"
#include "TSystem.h"
#include "iostream"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TString.h"
#include "TLegend.h"
#include "TRandom.h"

unsigned int ColorRequestCounter=0;
int requestColor(){
    int color[]={6,46,56};
    ColorRequestCounter=ColorRequestCounter+1;
    return color[ColorRequestCounter-1];
}

 void CheckFinalPar(TString folder="./Final/Qsq_run_%d.root"){
    std::vector<UInt_t> runIDs;
    runIDs.push_back(1983);
    runIDs.push_back(1996);
    runIDs.push_back(2052);
    runIDs.push_back(2199);
    runIDs.push_back(2320);
    runIDs.push_back(2322);

    TLegend *lgend=new TLegend(0.3,0.3);

    TCanvas *canv =new TCanvas("Check Para","Check Para",1960,1080);
    canv->cd();
    TString thetaLeafName="Theta";
    TRandom *rand=new TRandom();
    rand->SetSeed(100);
    for (auto runID : runIDs){
        // load the root canvas
        TString rootfname=Form(folder.Data(),runID);
        if (gSystem->AccessPathName(rootfname.Data())) continue;

        TFile *file=new TFile(rootfname.Data());
        file->GetListOfKeys()->Contains(thetaLeafName);
        auto plot=(TH1F *) file->Get(thetaLeafName);
        plot->SetLineWidth(2);
        int color = (int) ((113-51)*rand->Rndm()+51);
        plot->SetLineColor(color);

        double scale=1/(plot->Integral());
        plot->Scale(scale);
        plot->GetYaxis()->SetRangeUser(0,0.06);
        plot->Draw("same HIST");
        lgend->AddEntry(plot,Form("%s_run%d, mean %1.2f",plot->GetName(),runID,plot->GetMean(1)));

    }
    lgend->Draw("same");
}

