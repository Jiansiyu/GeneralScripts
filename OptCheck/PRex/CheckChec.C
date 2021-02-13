/*
 *
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
TString prepcut;
TString generalcut;
TString generalcutR="R.tr.n==1 && R.vdc.u1.nclust==1&& R.vdc.v1.nclust==1 && R.vdc.u2.nclust==1 && R.vdc.v2.nclust==1";// && fEvtHdr.fEvtType==1";// && R.gold.p > 0.91 && R.gold.p < 0.98";
TString generalcutL="L.tr.n==1 && L.vdc.u1.nclust==1&& L.vdc.v1.nclust==1 && L.vdc.u2.nclust==1 && L.vdc.v2.nclust==1";// && fEvtHdr.fEvtType==1";//  && L.gold.p > 0.91 && L.gold.p < 0.98";

//#define CheckCheck(__VA_ARGS__, 1) mCheckCheck(...)

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
/*    if(runID>20000){ //RHRS
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
    }*/
    return  chain;
}

unsigned int ColorRequestCounter=0;
int requestColor(){
    int color[]={6,46,56};
    ColorRequestCounter=ColorRequestCounter+1;
    return color[ColorRequestCounter-1];
}

void  mCheckCheck(Int_t args,...)
{
    std::vector<Int_t> runList;
    va_list arguments;
    for(va_start(arguments,args); args !=-1;args=va_arg(arguments,Int_t)){
//        std::cout<<args<<std::endl;
        runList.push_back(args);
    }
    //get the runlist and assign the
    TCanvas *canv=new TCanvas(Form("Check Run Projection"),Form("Check Run Projection"),1960,1080);
    canv->SetGridx();
    canv->SetGridy();
    canv->Draw();

    double phiCutBoundaryUp=0.005;
    double phiCutBoundaryDown=0.005;

    for(auto runID:runList){
        auto chain=LoadRootFile(runID);
        TString HRS="L";
        if(runID > 20000) HRS="R";
        TH1F *TargPhih=new TH1F(Form("Targ_ph_run%d",runID),Form("Targ_ph_run%d",runID),1000,-0.03,0.03);
        chain->Project(TargPhih->GetName(),Form("%s.gold.ph",HRS.Data()),Form("%s.gold.th>%f $$ %s.gold.th<%f",HRS.Data(),phiCutBoundaryDown,HRS.Data(),phiCutBoundaryUp));
        TargPhih->SetLineColor(requestColor());

        if(runID == runList.at(0)){
            TargPhih->Draw();
        }else{
            TargPhih->Draw("same");
        }
    }
    canv->Update();
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

void CheckTargThPh(Int_t args,...){
    std::vector<Int_t> runList;
    va_list arguments;
    for(va_start(arguments,args); args !=-1;args=va_arg(arguments,Int_t)){
        runList.push_back(args);
    }

    TCanvas *canv=new TCanvas(Form("Canv"),Form("Canv"),1960,1080);
    int splitCanv=int(std::sqrt(runList.size()));
    if(splitCanv*splitCanv < runList.size()) splitCanv+=1;

    canv->Divide(splitCanv,splitCanv);

    canv->SetGridx();
    canv->SetGridy();
    int CanvCounter=1;
    for (UInt_t runID : runList){
        canv->cd(CanvCounter++);

        TString HRS="L";
        if(runID > 20000) HRS="R";
        auto chain=LoadRootFile(runID);
        TH2F *TargThPhhh=new TH2F(Form("Targ_th_ph_run%d",runID),Form("Targ_th_ph_run%d",runID),1000,-0.03,0.03,1000,-0.045,0.045);
        chain->Project(TargThPhhh->GetName(),Form("%s.gold.th:%s.gold.ph",HRS.Data(),HRS.Data()));

        TargThPhhh->Draw("zcol");

        // get the beam position and draw the expected value on the targ
        double Zpos=979.0;
        if(HRS == "R") Zpos=979.23;


        auto beamPosTarg=getBPM(runID);

        TLatex *beamPosText=new TLatex(-0.03,0.005,Form("Beam X: %1.4f, Beam Y: %1.4f",beamPosTarg.X(),beamPosTarg.Y()));
        beamPosText->Draw("same");
//        TPaveText *beamPosTargText = new TPaveText(0.1,0.8,0.3,0.9,"NDC");
//        beamPosTargText->AddText(Form("Beam X: %1.4f, Beam Y: %1.4f",beamPosTarg.X(),beamPosTarg.Y()));
//        beamPosTargText->Draw("same");

        TLine *linePhi=new TLine(-beamPosTarg.X()/Zpos,TargThPhhh->GetYaxis()->GetXmin(),-beamPosTarg.X()/Zpos,TargThPhhh->GetYaxis()->GetXmax());
        linePhi->SetLineWidth(2);
        linePhi->SetLineColor(45);
        linePhi->Draw("same");

        TLine *lineTheta=new TLine(TargThPhhh->GetXaxis()->GetXmin(),beamPosTarg.Y()/Zpos,TargThPhhh->GetXaxis()->GetXmax(),beamPosTarg.Y()/Zpos);
        lineTheta->SetLineColor(6);
        lineTheta->SetLineWidth(2);
        lineTheta->Draw("same");
    }
    canv->Update();
}