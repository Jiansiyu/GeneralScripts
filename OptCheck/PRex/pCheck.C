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
#include "TVector2.h"
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
TString generalcutR="R.tr.n==1 && R.vdc.u1.nclust==1&& R.vdc.v1.nclust==1 && R.vdc.u2.nclust==1 && R.vdc.v2.nclust==1 ";
TString generalcutL="L.tr.n==1 ";//&& L.vdc.u1.nclust==1&& L.vdc.v1.nclust==1 && L.vdc.u2.nclust==1 && L.vdc.v2.nclust==1 ";//&& fEvtHdr.fEvtType==1

inline Bool_t IsFileExist (const std::string& name) {
    return !gSystem->AccessPathName(name.c_str());
}

TChain *LoadRootFile(UInt_t runID,UInt_t maxFile=1,TString folder="/home/newdriver/Storage/Research/PRex_Experiment/PRex_Replay/replay/Result"){
    TChain *chain=new TChain("T");
    TString rootDir(folder.Data());
    TString HRS="R";
    if (runID < 20000) HRS = "L";
    TString filePattern = Form("%s/prex%sHRS_%d_*",folder.Data(),HRS.Data(),runID);
    chain->Add(filePattern.Data());
    return  chain;
}

double getCentralP(TChain *chain, Bool_t drawFlag=false){

    // get the run Number
    int runID=(int)chain->GetMaximum("fEvtHdr.fRun");

    TString HRS="R";
    if(runID<20000){
        HRS="L";
    }

    //TODO need to check whether this is prex/crex experiment
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
        TH1F *HallR_NMR = new TH1F("HallR_NMR", "HallR_NMR", 1000, -0.7, 0.9);
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
    return CentralP;
}

int getRunID(TChain *chain){
    int runID=(int)chain->GetMaximum("fEvtHdr.fRun");
    return  runID;
}

void pCheck(UInt_t runID){
    auto chain = LoadRootFile(runID);
    auto centralP = getCentralP(chain);
    TString HRS = "R";
    if (runID<20000) HRS = "L";
    TH1F *mom = new TH1F(Form("%d p",runID),Form("%d p",runID),1000,0.8,0.99);

    chain->Project(mom->GetName(),Form("%s.gold.dp*%f+%f",HRS.Data(),centralP,centralP));
    mom->Draw();


}