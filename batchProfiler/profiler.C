#include "TCanvas.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TString.h"
#include "TFile.h"
#include "TPad.h"
#include "vector"
#include "iostream"
#include "TSystem.h"
#include "TLine.h"
#include "TLink.h"
#include "TChain.h"
#include "TPaveText.h"
#include "TROOT.h"
#include <iostream>
#include <fstream>

bool getsieveResidualCut(std::map<Int_t,TH2F *>sieveThResidualhh,double residualThres = 0.001,Int_t countCut = 5){
    /// apply cut the on the residual
//    for(auto indexer = sieveThResidualhh.begin(); indexer != sieveThResidualhh.end(); indexer++){
//        auto id = indexer->first;
//        auto histo = indexer->second;
//
//        Int_t  outSideRangeCounter = 0;
//        //count the number of the sieveholes that outside of the range
//        for (int binIndex = 0; binIndex< histo->GetXaxis()->GetNbins(); binIndex++){
//            auto val =histo->GetBinContent(binIndex);
//            if ((val > residualThres) || (val < residualThres*(-1.0))){
//                outSideRangeCounter += 1;
//            }
//        }
//        if (outSideRangeCounter > countCut) return false;
//    }
    return true;
}

void getScanResult(TString canvSaveName ="./Result/test_%06d.jpg",TString fnameTemplate="./"){
    TCanvas *canv=new TCanvas("DefaultCanv","DefaultCanv",4000,1960);
    // load the root file
    Int_t _runs[7]={2239,2240,2241,2244,2245,2256,2257};
    std::vector<Int_t> CarbonOptRuns(_runs, _runs + sizeof(_runs)/sizeof(Int_t));
    Int_t LeadRunID = 2322;

    std::map<Int_t,TH2F *>sieveThResidualhh;
    std::map<Int_t,TVirtualPad *>sieveThPhiPad;
    std::map<Int_t,TH2F *> sieveThPhihh;
    std::map<Int_t,TH2F *> sieveThPhiRealhh;

//    TCanvas *canv = new TCanvas("ScanCanvas","ScanCanvas",4000,1960);
    canv->Divide(6,4);
    TCanvas *sieveRescanv;
    for (int i = 0; i < CarbonOptRuns.size(); ++i) {
        auto runID = CarbonOptRuns.at(i);
        TString subfoldername = Form("%s/Sieve._%d_p4.f51_reform/CheckSieve_Report.root",fnameTemplate.Data(),runID);

        if (!gSystem->AccessPathName(subfoldername.Data())){
            TFile *fileio = new TFile(subfoldername.Data());
            sieveRescanv = (TCanvas *) fileio->Get("SieveCheck");
            sieveThPhiPad[i] = sieveRescanv->GetPad(3);
            sieveThPhiPad[i]->SetPad(0,0,1,1);

            // get the sieve residual plot
            sieveThResidualhh[i] = (TH2F *) fileio->Get("SievePhiResiduals")->Clone(Form("Sieve #Phi Residual %d",runID));
            sieveThResidualhh[i]->SetName(Form("Sieve #Phi Residual",runID));
            sieveThResidualhh[i]->SetTitle(Form("Sieve #Phi Residual %d",runID));
            sieveThResidualhh[i]->SetDirectory(0);

            //get the TH2F Canvas plot
            sieveThPhihh[i] = (TH2F *) fileio->Get("Sieve_Foil_Measured_ThetaPhi0");
            sieveThPhihh[i] ->SetName(Form("Sieve #Theta-#Phi",runID));
            sieveThPhihh[i] ->SetTitle(Form("Sieve #Theta-#Phi run%d",runID));
            sieveThPhihh[i]->SetDirectory(0);

            // get the readl plot
            sieveThPhiRealhh[i] = (TH2F *) fileio->Get("Sieve_Foil_realThetaPhi0");
            sieveThPhiRealhh[i]->SetName(Form("Sieve Real #Theta-#Phi",runID));
            sieveThPhiRealhh[i]->SetTitle(Form("Sieve Real #Theta-#Phi run%d",runID));
            sieveThPhiRealhh[i]->SetDirectory(0);

            Int_t baseCanvIndex = Int_t (i/2)*6 + i%2*3;
            canv->cd(baseCanvIndex + 1);
            sieveThPhihh[i]->Draw("zcol");
            sieveThPhiRealhh[i]->SetMarkerStyle(41);
            sieveThPhiRealhh[i]->SetMarkerColor(kRed);
            sieveThPhiRealhh[i]->Draw("same");

            canv->cd(baseCanvIndex + 2);
            sieveThPhiPad[i]->DrawClone();
            canv->cd(baseCanvIndex + 3);
            sieveThResidualhh[i]->Draw();

            TLine *line_central =new TLine(sieveThResidualhh[i]->GetXaxis()->GetXmin(),0.0,sieveThResidualhh[i]->GetXaxis()->GetXmax(),0.0);
            line_central->SetLineColor(kGreen);
            line_central->SetLineWidth(2);
            line_central->Draw("same");

            TLine *line_upper =new TLine(sieveThResidualhh[i]->GetXaxis()->GetXmin(),0.001,sieveThResidualhh[i]->GetXaxis()->GetXmax(),0.001);
            line_upper->SetLineColor(kMagenta);
            line_upper->SetLineWidth(2);
            line_upper->Draw("same");

            TLine *line_lower =new TLine(sieveThResidualhh[i]->GetXaxis()->GetXmin(),-0.001,sieveThResidualhh[i]->GetXaxis()->GetXmax(),-0.001);
            line_lower->SetLineColor(kMagenta);
            line_lower->SetLineWidth(2);
            line_lower->Draw("same");

            // memory
            sieveRescanv->Close();
            fileio->Close("R");
        }
    }

    // add the lead target run into the canvas
    TH2F *leadThPhChh = (TH2F *) gROOT->FindObject(Form("Pb run %d", LeadRunID));
    {
        if (leadThPhChh) {
            leadThPhChh->Clear();
        }
        leadThPhChh = new TH2F(Form("Pb run %d", LeadRunID), Form("Pb run %d", LeadRunID), 1000, -0.045, 0.045, 1000,
                                     -0.045, 0.045);

        TString subfoldername = Form("%s/Sieve._%d_p4.f51_reform/CheckSieve_Report.root",fnameTemplate.Data(),LeadRunID);
        if (!gSystem->AccessPathName(subfoldername.Data())){
            TChain *chain = new TChain("OptRes");
            chain->AddFile(subfoldername.Data());
            chain->Project(leadThPhChh->GetName(),"targProjTh:targProjPh");
            canv->cd(22);
            canv->cd(22)->SetGridx();
            canv->cd(22)->SetGridy();
            leadThPhChh->Draw("zcol");

            delete chain;
        }
        canv->cd(23);
        TPaveText *pt = new TPaveText(.05,.1,.95,.8);
        TObjArray *tx = fnameTemplate.Tokenize("/");
        Int_t txtIndex = tx->GetEntries()-1;
        pt->AddText(((TObjString *)(tx->At(txtIndex)))->String());
        pt->Draw();
    }
    canv->Modified();
    canv->Update();
    // apply the cut on the sieve plot
    canv->SaveAs(canvSaveName.Data());
    delete canv;


    //delete the mem
    std::cout<<"Start Deleting the sieveThResidualhh"<<std::endl;
    for (auto iter = sieveThResidualhh.begin(); iter != sieveThResidualhh.end(); iter++){
        delete iter->second;
    }
    std::cout<<"Start Deleting the sieveThPhihh"<<std::endl;
    for (auto iter = sieveThPhihh.begin(); iter!=sieveThPhihh.end(); iter++) {
        delete iter->second;
    }
    std::cout<<"Start Deleting the sieveThPhiRealhh"<<std::endl;
    for (auto iter = sieveThPhiRealhh.begin(); iter!=sieveThPhiRealhh.end(); iter++){
        delete iter->second;
    }
    delete leadThPhChh;

    sieveThResidualhh.clear();
    sieveThPhiPad.clear();
    sieveThPhihh.clear();
    sieveThPhiRealhh.clear();

}

void createReRunCommand(Int_t statIndex, Int_t endIndex){

}

Int_t main(int argc, char *argv[]){
    TString BaseDir = "./Scan/DBScan_20210422";
    Int_t startScanIndex = 0;
    Int_t endScanIndex = 0;
    if (argc == 2){
        startScanIndex = atoi(argv[1]);
        endScanIndex = startScanIndex + 1000;

    } else if (argc == 3){
        startScanIndex = atoi(argv[1]);
        endScanIndex = atoi(argv[2]);
    } else{
        std::cout<<"Error command number"<<std::endl;
        std::cout<<"./runVirifer startRunID endRunID"<<std::endl;
        exit(-1);
    }
    std::ofstream log;
    log.open("./runlog.txt",std::ios::in);
    std::cout<<"Start Index:"<<startScanIndex<<"  -> "<<endScanIndex<<"/("<<endScanIndex-startScanIndex<<")"<<std::endl;
    for (Int_t folderIndex = startScanIndex; folderIndex <= endScanIndex; folderIndex++){
        TString foldername = Form("%s_%05d",BaseDir.Data(),folderIndex);
        //check the existance
        if (!gSystem->AccessPathName(Form("%s_%05d/Sieve._2322_p4.f51_reform/CheckSieve_Report.root",BaseDir.Data(),folderIndex))) {
            //TCanvas *canv = new TCanvas(Form("Canv%d", folderIndex), Form("Canv%d", folderIndex), 4000, 1960);
            std::cout << "Working on " << folderIndex << std::endl;
            TString canvSaveName = Form("./Result/test_%06d.jpg",folderIndex);
            getScanResult(canvSaveName.Data(),foldername.Data());

        } else{
            std::cout<<"[WARNING]:: can not find"<<foldername.Data()<<std::endl;
            log << foldername.Data()<<std::endl;
        }
    }
    exit(0);
}
