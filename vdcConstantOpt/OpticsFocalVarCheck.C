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

// load the root file and  return the TChain
TChain *LoadrootFile(UInt_t runID,TString folder="/home/newdriver/pyQuant/PRex_Backups/PRex_CutBackup/Cut20210111_PRex/rootfile"){
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

//-----------------------------------------------------------------------------------
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

    if(momentumSpectro->GetEntries()<10) {
        std::cout<<"[ERROR]:: "<<"Fit Data empty"<<std::endl;
        exit(-1);
    }

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


TVector2 getBPM(UInt_t runID,TString csvfname="bpm_on_targ.csv"){
    if (gSystem->AccessPathName(csvfname.Data())){
        std::cout<<"\033[1;33m [Warning]\033[0m Missing csv file::"<<csvfname.Data()<<std::endl;
        exit(-1);
    }

    std::map<UInt_t,TVector2> bpmList;

    std::ifstream csvStream(csvfname.Data());
    std::string line;
    while (std::getline(csvStream,line)){
        std::istringstream s(line);
        std::string field;

        getline(s,field,',');
        TString title=field;
        if(title.Contains("run")) continue;

        UInt_t runs=std::stoi(field.c_str());

        getline(s,field,',');
        double_t bpmx=std::stof(field.c_str());

        getline(s,field,',');
        double_t bpmy=std::stof(field.c_str());

        TVector2 vec(bpmx,bpmy);

        bpmList[runs]=vec;
    }
    if(bpmList.find(runID) != bpmList.end()){
        return bpmList[runID];
    } else{
        std::cout<<"\033[1;33m [Warning]\033[0m Missing csv file::"<<csvfname.Data()<<std::endl;
        exit(-1);
    }
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

//__________________________________________________________________________________________________________________________________
Int_t OpticsFocalVarCheck(UInt_t runID,TString folder="/home/newdriver/pyQuant/PRex_Backups/PRex_CutBackup/Cut20210111_PRex/rootfile") {
    // prepare the data
    TString HRS="R";
    TChain *chain=LoadrootFile(runID,folder.Data());
    if (runID > 20000){
        HRS = "R";
        generalcut = generalcutR;
    } else{
        HRS = "L";
        generalcut = generalcutL;
    }

    TCanvas *mainPatternCanvas=(TCanvas *)gROOT->GetListOfCanvases()->FindObject("cutPro");
    if(!mainPatternCanvas){
        mainPatternCanvas=new TCanvas("cutPro","cutPro",600,600);
    }else{
        mainPatternCanvas->Clear();
    }
    mainPatternCanvas->Draw();
    TH2F *TargetThPhHH=(TH2F *)gROOT->FindObject("th_vs_ph");
    if(TargetThPhHH) TargetThPhHH->Delete();
    TargetThPhHH=new TH2F("th_vs_ph","th_vs_ph",1000,-0.03,0.03,1000,-0.05,0.05);

    chain->Project(TargetThPhHH->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()),generalcut.Data());
    TargetThPhHH->Draw("zcol");

    mainPatternCanvas->Update();
    mainPatternCanvas->ToggleEventStatus();
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


//    TFile *f1=new TFile("test_temp.root","RECREATE");
//    assert(f1);

    // link the root tree and check which HRS we are working on
    TChain *chain = (TChain *) gROOT->FindObject("T");
    TString HRS("R");
    TString filename(chain->GetFile()->GetName());
    if (filename.Contains("RHRS")) {
    } else if (filename.Contains("LHRS")) {
        HRS = "L";
    }

    // try to extract the hall prob if this is LHRS
    double CentralP=getCentralP(chain);

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
    } else
        SieveRecCanvas = new TCanvas("SieveRecCanvas", "Projection Canvas",1960, 1000);

    SieveRecCanvas->Divide(1, 3);
    SieveRecCanvas->cd(1)->Divide(4, 1);
    SieveRecCanvas->cd(2)->Divide(4, 1);
    SieveRecCanvas->cd(3)->Divide(4, 1);
    //get the hsitogram and start rec
    SieveRecCanvas->cd(1)->cd(2);

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
    SieveRecCanvas->cd(1)->cd(1);

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


    //get the focal plane variables
    double focal_x;
    double focal_y;
    double focal_th;
    double focal_ph;

    TH1F *focalThetaH;
    TH1F *focalPhiH;
    TH1F *focalYH;
    TH1F *focalXH;

    focalXH=new TH1F(Form("%s%d_focalX",HRS.Data(),runID),Form("%s%d_focalX",HRS.Data(),runID),
                               2000,-1,1);
    focalThetaH=new TH1F(Form("%s%d_focalTheta",HRS.Data(),runID),Form("%s%d_focalTheta",HRS.Data(),runID),
                                   2000,-0.3,0.3);
    focalYH=new TH1F(Form("%s%d_focalY",HRS.Data(),runID),Form("%s%d_focalY",HRS.Data(),runID),
                               2000,-0.2,0.2);
    focalPhiH=new TH1F(Form("%s%d_focalPhi",HRS.Data(),runID),Form("%s%d_focalPhi",HRS.Data(),runID),
                                 1000,-0.1,0.1);

    // project the Theta
    SieveRecCanvas->cd(2)->cd(1);
    chain->Project(focalXH->GetName(),Form("%s.tr.r_x",HRS.Data()),cutg->GetName());
    focalXH->GetXaxis()->SetRangeUser(focalXH->GetBinCenter(focalXH->GetMaximumBin())-0.02,focalXH->GetBinCenter(focalXH->GetMaximumBin())+0.02);
    focalXH->Fit("gaus","","",focalXH->GetBinCenter(focalXH->GetMaximumBin())-0.01,focalXH->GetBinCenter(focalXH->GetMaximumBin())+0.01);
    focalXH->Draw();
    focal_x=focalXH->GetFunction("gaus")->GetParameter(1);
    TLatex *focalXtxt =new TLatex (focalXH->GetFunction("gaus")->GetParameter(1),focalXH->GetFunction("gaus")->GetParameter(0),Form("%1.5f",focalXH->GetFunction("gaus")->GetParameter(1)));
    focalXtxt->Draw("same");


    // project Theta Var
    SieveRecCanvas->cd(2)->cd(2);
    chain->Project(focalThetaH->GetName(),Form("%s.tr.r_th",HRS.Data()),cutg->GetName());
    focalThetaH->GetXaxis()->SetRangeUser(focalThetaH->GetBinCenter(focalThetaH->GetMaximumBin())-0.015,focalThetaH->GetBinCenter(focalThetaH->GetMaximumBin())+0.015);
    focalThetaH->Fit("gaus","","",focalThetaH->GetBinCenter(focalThetaH->GetMaximumBin())-0.005,focalThetaH->GetBinCenter(focalThetaH->GetMaximumBin())+0.005);
    focalThetaH->Draw();
    focal_th=focalThetaH->GetFunction("gaus")->GetParameter(1);
    TLatex *focalThtxt = new TLatex(focalThetaH->GetFunction("gaus")->GetParameter(1),focalThetaH->GetFunction("gaus")->GetParameter(0),Form("%1.5f",focalThetaH->GetFunction("gaus")->GetParameter(1)));
    focalThtxt->Draw("same");


    // project Y
    SieveRecCanvas->cd(2)->cd(3);
    chain->Project(focalYH->GetName(),Form("%s.tr.r_y",HRS.Data()),cutg->GetName());
    focalYH->GetXaxis()->SetRangeUser(focalYH->GetBinCenter(focalYH->GetMaximumBin())-0.01,focalYH->GetBinCenter(focalYH->GetMaximumBin())+0.01);
    focalYH->Fit("gaus","","",focalYH->GetBinCenter(focalYH->GetMaximumBin())-0.003,focalYH->GetBinCenter(focalYH->GetMaximumBin())+0.003);
    focalYH->Draw();
    focal_y=focalYH->GetFunction("gaus")->GetParameter(1);
    auto focalYtxt = new TLatex(focalYH->GetFunction("gaus")->GetParameter(1),focalYH->GetFunction("gaus")->GetParameter(0),Form("%1.5f",focalYH->GetFunction("gaus")->GetParameter(1)));
    focalYtxt->Draw("same");

    //project Phi
    SieveRecCanvas->cd(2)->cd(4);
    chain->Project(focalPhiH->GetName(),Form("%s.tr.r_ph",HRS.Data()),cutg->GetName());
    focalPhiH->GetXaxis()->SetRangeUser(focalPhiH->GetBinCenter(focalPhiH->GetMaximumBin())-0.01,focalPhiH->GetBinCenter(focalPhiH->GetMaximumBin())+0.01);
    focalPhiH->Fit("gaus","","",focalPhiH->GetBinCenter(focalPhiH->GetMaximumBin())-0.003,focalPhiH->GetBinCenter(focalPhiH->GetMaximumBin())+0.003);
    focalPhiH->Draw();
    focal_ph=focalPhiH->GetFunction("gaus")->GetParameter(1);
    auto focalPhtxt = new TLatex(focalPhiH->GetFunction("gaus")->GetParameter(1),focalPhiH->GetFunction("gaus")->GetParameter(0),Form("%1.5f",focalPhiH->GetFunction("gaus")->GetParameter(1)));
    focalPhtxt->Draw("same");


    // project the TRCS variables
    TH1F *TRSCxH=new TH1F(Form("%s%d_TransX",HRS.Data(),runID),Form("%s%d_TransX",HRS.Data(),runID),
                     3000,-1,1);
    TH1F *TRSCyH=new TH1F(Form("%s%d_TransY",HRS.Data(),runID),Form("%s%d_TransY",HRS.Data(),runID),
                          4000,-1,1);
    TH1F *TRSCthH=new TH1F(Form("%s%d_TransTh",HRS.Data(),runID),Form("%s%d_TransTh",HRS.Data(),runID),
                          3000,-1,1);
    TH1F *TRSCphH=new TH1F(Form("%s%d_TransPh",HRS.Data(),runID),Form("%s%d_TransPh",HRS.Data(),runID),
                          3000,-1,1);

    // project the data
    SieveRecCanvas->cd(3)->cd(1);
    chain->Project(TRSCxH->GetName(),Form("%s.tr.x",HRS.Data()),cutg->GetName());
    TRSCxH->GetXaxis()->SetRangeUser(TRSCxH->GetBinCenter(TRSCxH->GetMaximumBin())-0.005,TRSCxH->GetBinCenter(TRSCxH->GetMaximumBin())+0.005);
    TRSCxH->Fit("gaus","","",TRSCxH->GetBinCenter(TRSCxH->GetMaximumBin())-0.005,TRSCxH->GetBinCenter(TRSCxH->GetMaximumBin())+0.005);
    TRSCxH->Draw();
    double TRSCx_val = TRSCxH -> GetFunction("gaus")->GetParameter(1);
    TLatex *TRSCxH_txt = new TLatex(TRSCxH -> GetFunction("gaus")->GetParameter(1), TRSCxH -> GetFunction("gaus")->GetParameter(0),Form("%1.5f",TRSCxH -> GetFunction("gaus")->GetParameter(1)));
    TRSCxH_txt->Draw("same");

//    // Project the theta parameter
    SieveRecCanvas->cd(3)->cd(2);
    chain->Project(TRSCthH->GetName(),Form("%s.tr.th",HRS.Data()),cutg->GetName());
    TRSCthH->GetXaxis()->SetRangeUser(TRSCthH->GetBinCenter(TRSCthH->GetMaximumBin())-0.01,TRSCthH->GetBinCenter(TRSCthH->GetMaximumBin())+0.01);
    TRSCthH->Fit("gaus","","",TRSCthH->GetBinCenter(TRSCthH->GetMaximumBin())-0.005,TRSCthH->GetBinCenter(TRSCthH->GetMaximumBin())+0.005);
    TRSCthH->Draw();
    TLatex *TRSCthH_txt = new TLatex(TRSCthH->GetFunction("gaus")->GetParameter(1),TRSCthH->GetFunction("gaus")->GetParameter(0),Form("%1.5f",TRSCthH->GetFunction("gaus")->GetParameter(1)));
    TRSCthH_txt->Draw("same");

    //    //Project the Y dimension
    SieveRecCanvas->cd(3)->cd(3);
    chain->Project(TRSCyH->GetName(),Form("%s.tr.y",HRS.Data()),cutg->GetName());
    TRSCyH->GetXaxis()->SetRangeUser(TRSCyH->GetBinCenter(TRSCyH->GetMaximumBin())-0.015,TRSCyH->GetBinCenter(TRSCyH->GetMaximumBin())+0.015);
    TRSCyH->Fit("gaus","","",TRSCyH->GetBinCenter(TRSCyH->GetMaximumBin())-0.005,TRSCyH->GetBinCenter(TRSCyH->GetMaximumBin())+0.005);
    TRSCyH->Draw();
    TLatex *TRSCyH_txt = new TLatex(TRSCyH->GetFunction("gaus")->GetParameter(1),TRSCyH->GetFunction("gaus")->GetParameter(0),Form("%1.5f",TRSCyH->GetFunction("gaus")->GetParameter(1)));
    TRSCyH_txt->Draw("same");


//    //Project the Phi Parameter
    SieveRecCanvas->cd(3)->cd(4);
    chain->Project(TRSCphH->GetName(),Form("%s.tr.ph",HRS.Data()),cutg->GetName());
    TRSCphH->GetXaxis()->SetRangeUser(TRSCphH->GetBinCenter(TRSCphH->GetMaximumBin())-4*TRSCphH->GetRMS(),TRSCphH->GetBinCenter(TRSCphH->GetMaximumBin())+4*TRSCphH->GetRMS());
    TRSCphH->Fit("gaus","","",TRSCphH->GetBinCenter(TRSCphH->GetMaximumBin())-2*TRSCphH->GetRMS(),TRSCphH->GetBinCenter(TRSCphH->GetMaximumBin())+2*TRSCphH->GetRMS());
    TRSCphH->Draw();
    TLatex * TRSCphH_txt = new TLatex(TRSCphH->GetFunction("gaus")->GetParameter(1),TRSCphH->GetFunction("gaus")->GetParameter(0),Form("%1.5f",TRSCphH->GetFunction("gaus")->GetParameter(1)));
    TRSCphH_txt->Draw("same");

    // load the bpm on target
    auto beamPos=getBPM(runID);

    // create file and write the information into the csv file
    std::ofstream txtfileio("./Carbon_focal_var.csv",std::ofstream::app);
    auto writeStr=Form("%d  %1.5f   %1.5f   %1.5f   %1.5f   %1.5f   %1.5f",runID,focal_x,focal_th,focal_y,focal_ph,beamPos.X(),beamPos.Y());
    txtfileio<<writeStr<<std::endl;
    txtfileio.close();
}

