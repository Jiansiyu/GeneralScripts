/*
 *
 *
 *
 *
 */

#include "iostream"
#include "TChain.h"
#include "TSystem.h"

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

void vdcConstCheck(){






}