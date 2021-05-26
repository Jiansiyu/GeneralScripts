/*
 *Asymmmetry Calculation Algorithm with cross validation approch
 *
 */


#include <TString.h>
#include <TSystem.h>
#include <TRandom3.h>
#include <TChain.h>
#include <TROOT.h>
#include <TMath.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TFile.h>
#include "TLatex.h"
#include "CREXdata.h"
#include "TLine.h"
///
/// \param fname   name of the root files
/// \param crossValidationEvtCT,  cross validation event count for each run, if this number is smaller than 1, exp 0.1, it will take entries*crossValidationEvtCT
/// \cvRunCT crossValidation Run counts
/// \param ncores   number of concurrent cores
void getCrossValidationVal(Int_t  runID = 2052, Double_t crossValidationEvtCT = 50000,Int_t cvRunCT = 200, Int_t ncores=4){
    TString fname= Form("Qsq/Qsq_Qct25/prexLHRS_%d_*",runID);

    LoadTable("horpb.dat",0);//Lead table - sufficient

    double scale = 0.9524/0.9512;//Num - 1 MeV energy loss, peak pos
    double d2r = TMath::Pi()/180;
    double r2d = 1/d2r;
    double th0 = 4.74;  // TODO need to check the true value
    //th0 is for data
    double cth0 = TMath::Cos(th0*d2r); double sth0 = TMath::Sin(th0*d2r);
    double cthsim = TMath::Cos((4.74)*d2r);
    double sthsim = TMath::Sin((4.74)*d2r);

    // crosse validation configuration
    ROOT::EnableImplicitMT(ncores);

    // random sampling on the total event
    TRandom3 *randEvt =new TRandom3();
    randEvt->SetSeed(0); // use current time as seed

    // load the files and attach the branch
    TChain *chain = new TChain("T");
    chain ->Add(fname.Data());

    //For data, need cluster, adc,th,ph,p
    std::vector<double> Angle,Qsqdat, Asymdat, Momdat;
    double thisu1,thisv1,thisu2,thisv2,thisPdat, thisdPdat, thisThdat,thisPhdat,thisQsqdat,thisAsymdat,thisCosAngdat,thisAngdat,thisADC;
    double thisEvt;

    chain->SetBranchAddress("L.vdc.u1.nclust",&thisu1);
    chain->SetBranchAddress("L.vdc.v1.nclust",&thisv1);
    chain->SetBranchAddress("L.vdc.u2.nclust",&thisu2);
    chain->SetBranchAddress("L.vdc.v2.nclust",&thisv2);
    chain->SetBranchAddress("L.gold.th",&thisThdat);
    chain->SetBranchAddress("L.gold.ph",&thisPhdat);
    chain->SetBranchAddress("L.gold.p",&thisPdat);
    chain->SetBranchAddress("P.upQadcL",&thisADC);
    chain->SetBranchAddress("L.gold.dp",&thisdPdat);
    chain->SetBranchAddress("P.evtypebits",&thisEvt);

    chain->SetImplicitMT(true);                      // enable concurrent the the tree
    Long64_t entries = chain->GetEntries();

#ifdef __DEBUG__
    // test function
    //apply cut on the event
    TH1F *asymh = new TH1F("debug asym","debug asym",150,0.0,1.0);
    for(Long64_t entry = 0; entry < entries and entry < 50000; entry ++) {

        if (entry %100 == 0)std::cout<<"\x1B[s"<<Form("\tProgress %2.1f",(double)entry*100/entries)<<"% "<<entries <<"\x1B[u" << std::flush;

        chain->GetEntry(entry);
        thisPdat *= scale;
        int thisevent = (int) thisEvt;

        if(thisu1 == 1 && thisv1 == 1 && thisu2 == 1 && thisv2 == 1 && thisADC > 0.0
           && abs(thisThdat)<0.08 && abs(thisPhdat)<0.05 && thisPdat >  0.9534*0.96 && thisPdat < 0.9534*1.002 && ((thisevent&2)==2)  )
        {

            thisCosAngdat =
                    (cth0 - thisPhdat * sth0) / (TMath::Sqrt(1 + thisThdat * thisThdat + thisPhdat * thisPhdat));
            thisAngdat = r2d * TMath::ACos(thisCosAngdat);

            //Using a hardcoded beam energy -- run dependent, yes? --Beam Energy 953.4
            thisQsqdat = 2 * 0.9534 * thisPdat * (1 - thisCosAngdat);
            thisAsymdat = 1e6 * Interpolate(thisPdat * 1000, thisAngdat, 0, 1);

            asymh->Fill(thisAsymdat);
            }
        }
    asymh->Draw();
#endif

    TFile *outputFile = new TFile(Form("asym_cross_vali_run%d_%d.root",runID,cvRunCT),"RECREATE");
    gDirectory->mkdir("cvRun");

    TH1F *cvMeanh = new TH1F(Form("asym_mean"),Form("asym_mean"),300,0.4,0.7);
    // repeat the cross validations to extract the runs
    for (Long64_t cvID = 0; cvID < cvRunCT; cvID ++) {

        Long64_t cvEvtCT = Long64_t (crossValidationEvtCT);
        if ( crossValidationEvtCT < 1){
            cvEvtCT = Long64_t (entries*crossValidationEvtCT);
        }
        TH1F *asymCTh = new TH1F(Form("Calc_Asym_cvID%lld",cvID),Form("Calc_Asym_cvID%lld",cvID),150,0.0,1.0);
        std::vector<Long64_t> sampleArray;
        sampleArray.clear();
        for (Long64_t cvEvtIndex = 0; cvEvtIndex < cvEvtCT; cvEvtIndex++){
            sampleArray.push_back(randEvt->Integer(entries));
        }
        std::sort(sampleArray.begin(),sampleArray.end());
        for (auto entry : sampleArray){
            chain->GetEntry(entry);
            if (entry %100 == 0)std::cout<<"\x1B[s"<<Form("\tProgress %2.1f",(double)entry*100/entries)<<"% "<<cvID<<"/"<<cvRunCT <<"\x1B[u" << std::flush;
            thisPdat *= scale;
            int thisevent = (int) thisEvt;
            //apply cut on the event
            if(thisu1 == 1 && thisv1 == 1 && thisu2 == 1 && thisv2 == 1 && thisADC > 0.0
            && abs(thisThdat)<0.08 && abs(thisPhdat)<0.05 && thisPdat >  0.9534*0.96 && thisPdat < 0.9534*1.002 && ((thisevent&2)==2)  ){

                thisCosAngdat = (cth0-thisPhdat*sth0)/(TMath::Sqrt(1+thisThdat*thisThdat+thisPhdat*thisPhdat));
                thisAngdat = r2d*TMath::ACos(thisCosAngdat);
                //Using a hardcoded beam energy -- run dependent, yes? --Beam Energy 953.4
                thisQsqdat = 2*0.9534*thisPdat*(1-thisCosAngdat);
                thisAsymdat = 1e6*Interpolate(thisPdat*1000,thisAngdat,0,1);
                asymCTh->Fill(thisAsymdat);
            }
        }
        outputFile->cd("cvRun");
        asymCTh->Write();
        cvMeanh->Fill(asymCTh->GetMean());
        asymCTh->Delete();
    }
    outputFile->cd();
    cvMeanh->Write();
    outputFile->Write();
    outputFile->Close();
}

std::map<Int_t,TH1F *>getRunAsym(TString fname){
    // load the files and attach the branch

    LoadTable("horpb.dat",0);//Lead table - sufficient

    double scale = 0.9524/0.9512;//Num - 1 MeV energy loss, peak pos
    double d2r = TMath::Pi()/180;
    double r2d = 1/d2r;
    double th0 = 4.74;  // TODO need to check the true value
    //th0 is for data
    double cth0 = TMath::Cos(th0*d2r); double sth0 = TMath::Sin(th0*d2r);
    double cthsim = TMath::Cos((4.74)*d2r);
    double sthsim = TMath::Sin((4.74)*d2r);

    // crosse validation configuration
    ROOT::EnableImplicitMT(8);

    TChain *chain = new TChain("T");
    chain ->Add(fname.Data());
    //For data, need cluster, adc,th,ph,p
    std::vector<double> Angle,Qsqdat, Asymdat, Momdat;
    double thisu1,thisv1,thisu2,thisv2,thisPdat, thisdPdat, thisThdat,thisPhdat,thisQsqdat,thisAsymdat,thisCosAngdat,thisAngdat,thisADC;
    double thisEvt;

    chain->SetBranchAddress("L.vdc.u1.nclust",&thisu1);
    chain->SetBranchAddress("L.vdc.v1.nclust",&thisv1);
    chain->SetBranchAddress("L.vdc.u2.nclust",&thisu2);
    chain->SetBranchAddress("L.vdc.v2.nclust",&thisv2);
    chain->SetBranchAddress("L.gold.th",&thisThdat);
    chain->SetBranchAddress("L.gold.ph",&thisPhdat);
    chain->SetBranchAddress("L.gold.p",&thisPdat);
    chain->SetBranchAddress("P.upQadcL",&thisADC);
    chain->SetBranchAddress("L.gold.dp",&thisdPdat);
    chain->SetBranchAddress("P.evtypebits",&thisEvt);

    chain->SetImplicitMT(true);                      // enable concurrent the the tree
    Long64_t entries = chain->GetEntries();
    if (entries > 50000) entries = 100000;

    TH1F *asymCTh = new TH1F(Form("Calc_Asym"),Form("Calc_Asym"),150,0.0,1.0);
    TH1F *qsq = new TH1F(Form("qsq"),"LHRS Apparent Q^{2} Data",150,0.0,0.015);
    TH1F *lab = new TH1F(Form("lab"),"LHRS Apparent #theta_{lab} Data",150,1.0,9.0);

    for (Long64_t entry = 0; entry < entries ; entry ++){
        chain->GetEntry(entry);

        if (entry %100 == 0)std::cout<<"\x1B[s"<<Form("\tProgress %2.1f",(double)entry*100/entries)<<"% "<<entry<<"/"<<entries <<"\x1B[u" << std::flush;
        thisPdat *= scale;
        int thisevent = (int) thisEvt;
        //apply cut on the event
        if(thisu1 == 1 && thisv1 == 1 && thisu2 == 1 && thisv2 == 1 && thisADC > 480
           && abs(thisThdat)<0.08 && abs(thisPhdat)<0.05 && thisPdat >  0.9534*0.96 && thisPdat < 0.9534*1.002 && ((thisevent&2)==2)  ){

            thisCosAngdat = (cth0-thisPhdat*sth0)/(TMath::Sqrt(1+thisThdat*thisThdat+thisPhdat*thisPhdat));
            thisAngdat = r2d*TMath::ACos(thisCosAngdat);
            //Using a hardcoded beam energy -- run dependent, yes? --Beam Energy 953.4
            thisQsqdat = 2*0.9534*thisPdat*(1-thisCosAngdat);
            thisAsymdat = 1e6*Interpolate(thisPdat*1000,thisAngdat,0,1);
            asymCTh->Fill(thisAsymdat);
            lab->Fill(thisAngdat);
            qsq->Fill(thisQsqdat);
        }
    }

    std::map<Int_t,TH1F* > res;
    res[0] = asymCTh;
    res[1] = lab;
    res[2] = qsq;
    return res;
}

void getCompare(){
    Int_t runID = 2052;
    TH1F *asyComph=new TH1F(Form("Asym_run%d",runID),Form("Asym_run%d",runID),11,-1,10);
    asyComph->GetYaxis()->SetRangeUser(0.57,0.59);
    asyComph->SetMarkerStyle(8);
    asyComph->SetMarkerSize(2);
    asyComph->SetLineWidth(1);
    asyComph->SetLineColor(20);


    TH1F *qsqComph = new TH1F(Form("qsq_run%d",runID),"LHRS Apparent Q^{2} Data",11,-1,10);
    qsqComph->GetYaxis()->SetRangeUser(0.00625,0.0066);
    qsqComph->SetMarkerStyle(8);
    qsqComph->SetMarkerSize(2);
    qsqComph->SetLineWidth(1);
    qsqComph->SetLineColor(20);

    TH1F *labComph = new TH1F(Form("lab_run%d",runID),"LHRS Apparent #theta_{lab} Data",11,-1,10);
    labComph->GetYaxis()->SetRangeUser(4.7,4.9);
    labComph->SetMarkerStyle(8);
    labComph->SetMarkerSize(2);
    labComph->SetLineWidth(1);
    labComph->SetLineColor(20);


    double asym_dnp = 0;
    double theta_dnp = 0;
    double qsq_dnp = 0;
    TFile *fileio = new TFile("report.root","recreate");

    for (int scanIndex = 0; scanIndex <8; scanIndex ++){
        TString fname = Form("/home/newdriver/Storage/Research/PRex_Experiment/PRex_Replay/replay/Result/prexLHRS_UID%d_2052_-1*",scanIndex);

        auto res = getRunAsym(fname.Data());
        auto asymh = res[0];
        auto thetah = res[1];
        auto qsqh = res[2];

        asymh->SetName(Form("Asym_index%d",scanIndex));
        asymh->Write();
        thetah->SetName(Form("theta_index%d",scanIndex));
        thetah->Write();
        qsqh->SetName(Form("qsq_index%d",scanIndex));
        qsqh->Write();

        asyComph->Fill(scanIndex,asymh->GetMean());
        asyComph->SetBinError(scanIndex+2., 0.003);
        qsqComph->Fill(scanIndex,qsqh->GetMean());
        qsqComph->SetBinError(scanIndex+2., 0.00003);
        labComph->Fill(scanIndex,thetah->GetMean());
        labComph->SetBinError(scanIndex+2., 0.003);


        if (scanIndex == 0){
            asym_dnp = asymh->GetMean();
            theta_dnp = thetah->GetMean();
            qsq_dnp  = qsqh->GetMean();
        }
    }

    TCanvas *canv = new TCanvas("canv","canv",1960,1080);
    canv->Divide(2,2);
    canv->Draw();

    canv->cd(1);
    asyComph->Draw("E1");
    TLine *line = new TLine(-1,asym_dnp,10,asym_dnp);
    line->SetLineColor(3);
    line->Draw("same");
//    TLine *lineUpper = new TLine(-1,asym_dnp*0.98,10,asym_dnp*0.98);
//    lineUpper->SetLineColor(6);
//    lineUpper->Draw("same");
//    TLine *lineLower = new TLine(-1,asym_dnp*1.02,10,asym_dnp*1.02);
//    lineLower->SetLineColor(6);
//    lineLower->Draw("same");




    canv->cd(2);
    qsqComph->Draw("E1");

    TLine *lineq = new TLine(-1,qsq_dnp,10,qsq_dnp);
    lineq->SetLineColor(3);
    lineq->Draw("same");
//    TLine *lineqUpper = new TLine(-1,qsq_dnp*0.98,10,qsq_dnp*0.98);
//    lineqUpper->SetLineColor(6);
//    lineqUpper->Draw("same");
//    TLine *lineqLower = new TLine(-1,qsq_dnp*1.02,10,qsq_dnp*1.02);
//    lineqLower->SetLineColor(6);
//    lineqLower->Draw("same");


    canv->cd(3);
    labComph->Draw("E1");

    TLine *linet = new TLine(-1,theta_dnp,10,theta_dnp);
    linet->SetLineColor(3);
    linet->Draw("same");
//    TLine *lineUppert= new TLine(-1,theta_dnp*0.98,10,theta_dnp*0.98);
//    lineUppert->SetLineColor(6);
//    lineUppert->Draw("same");
//    TLine *lineLowert = new TLine(-1,theta_dnp*1.02,10,theta_dnp*1.02);
//    lineLowert->SetLineColor(6);
//    lineLowert->Draw("same");


    canv->Update();

}

///
//void getCompare(Int_t runID){
//
//    TCanvas *canv = new TCanvas("Canv","Canv",1960,1080);
//    canv->cd();
//    canv->Draw();
//    TH1F *asyComph=new TH1F(Form("Asym_run%d",runID),Form("Asym_run%d",runID),11,-1,10);
//    asyComph->GetYaxis()->SetRangeUser(0.5,0.65);
//    asyComph->Fill(0.,0.5751);
//    asyComph->SetBinError(2,0.003);
//    asyComph->Draw("E1");
//
//    TLatex *txt0 = new TLatex(0.,0.58,Form("DNP"));
//    txt0->Draw("same");
//
//    TLine *line = new TLine(-1,0.5751,10,0.5751);
//    line->SetLineColor(3);
//    line->Draw("same");
//
//    TLine *lineUpper = new TLine(-1,0.5751*0.95,10,0.5751*0.95);
//    lineUpper->SetLineColor(6);
//    lineUpper->Draw("same");
//
//    TLine *lineLower = new TLine(-1,0.5751*1.05,10,0.5751*1.05);
//    lineLower->SetLineColor(6);
//    lineLower->Draw("same");
//
//    for (int scanIndex = 1; scanIndex <6; scanIndex ++){
//     TString fname = Form("/home/newdriver/Storage/Research/PRex_Experiment/PRex_Replay/replay/Result/prexLHRS_UID%d_2052_-1*",scanIndex);
//     auto histo = getRunAsym(fname.Data());
//     histo->SetName(Form("Calc_Asym_%d",scanIndex));
//     asyComph->Fill(scanIndex , histo->GetMean());
//     asyComph->SetBinError(2+scanIndex ,0.003);
//    }
//
//    // project the indexer
////    asyComph->Draw("E1");
//    asyComph->SetMarkerStyle(8);
//    asyComph->SetMarkerSize(2);
//    asyComph->SetLineWidth(1);
//    asyComph->SetLineColor(20);
//
//    canv->Update();
//
//
//}

/// test function for the cross validation
void tester_cv(){
    getCrossValidationVal();
}