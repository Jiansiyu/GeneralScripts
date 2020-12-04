/*
 * vdc t y p correction script
 * author: Siyu Jian
 *         jiansiyu@gmail.com
 */

#include "TCanvas.h"
#include "TChain.h"
#include "TH1F.h"
#include "TSystem.h"
#include "iostream"
#include "stdarg.h"
#include "fstream"
#include "TVector2.h"
#include "map"
#include "TH2F.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TROOT.h"
#include "TGraph.h"
#include "TCutG.h"
#include "TFile.h"


TString generalcut;
TString generalcutR="R.tr.n==1 && R.vdc.u1.nclust==1&& R.vdc.v1.nclust==1 && R.vdc.u2.nclust==1 && R.vdc.v2.nclust==1";// && fEvtHdr.fEvtType==1";// && R.gold.p > 0.91 && R.gold.p < 0.98";
TString generalcutL="L.tr.n==1 && L.vdc.u1.nclust==1&& L.vdc.v1.nclust==1 && L.vdc.u2.nclust==1 && L.vdc.v2.nclust==1";// && fEvtHdr.fEvtType==1";//  && L.gold.p > 0.91 && L.gold.p < 0.98";

inline Bool_t IsFileExist (const std::string& name) {
    return !gSystem->AccessPathName(name.c_str());
}

TChain *LoadRootFile(UInt_t runID,UInt_t maxFile=1,TString folder="/home/newdriver/Storage/Research/PRex_Experiment/PRex_Replay/replay/Result"){

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
                if(split > maxFile) break;
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
                if(split > maxFile) break;
            }
        }else{
            std::cout<<"Looking file :"<<Form("%s/prexLHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
        }
    }
    return  chain;
}

///
/// \param runID
/// \param csvfname
/// \return
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

///
struct vdcFocalVar{
    UInt_t runID;
    TString HRS;
    TString experiment;
    double centralP;
    double fX;
    double fY;
    double fTh;
    double fPh;
};


// get the cut file
void vdcCorrection(UInt_t runID,UInt_t maxFile=2,TString folder="/home/newdriver/Storage/Research/PRex_Experiment/PRex_Replay/replay/Result"){
    // get load the data file
    // prepare the data
    TChain *chain=LoadRootFile(runID,maxFile,folder);
    TString rootDir(folder.Data());
    TString HRS="L";
    generalcut=generalcutL;
    if(runID > 20000){
        HRS="R";
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
    mainPatternCanvas->AddExec("ex", "DynamicCanvas()");
    std::cout<<"This is an test point"<<std::endl;
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


//    TFile *f1=new TFile("test_temp.root","recreate");
//    assert(f1);

    // link the root tree and check which HRS we are working on
    TChain *chain = (TChain *) gROOT->FindObject("T");
    TString HRS("R");
    TString filename(chain->GetFile()->GetName());
    if (filename.Contains("RHRS")) {
    } else if (filename.Contains("LHRS")) {
        HRS = "L";
    }

    UInt_t runID=chain->GetMaximum("fEvtHdr.fRun");
    double CentralP=getCentralP(chain);

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
//		delete SieveRecCanvas->GetPrimitive("Projection");
    } else
        SieveRecCanvas = new TCanvas("SieveRecCanvas", "Projection Canvas",
                                     1000, 1000);

    SieveRecCanvas->Divide(1, 3);
    SieveRecCanvas->cd(1)->Divide(4, 1);
    //get the hsitogram and start rec
    SieveRecCanvas->cd(1)->cd(1);

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


    //load the file and apply cut on each folder
    // load the focal plane variables






}

