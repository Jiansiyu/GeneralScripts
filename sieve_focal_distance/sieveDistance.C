#include "TH1F.h"
#include "TChain.h"
#include "TString.h"
#include "map"
#include "vector"
#include "iostream"
#include "TSystem.h"
#include "TCanvas.h"

class sieveInfor{
public:
    sieveInfor(){};
    ~ sieveInfor(){};
    sieveInfor(Int_t UID,
               float focal_X,
               float focal_sigma_X,
               float focal_Y,
               float focal_sigma_Y,
               float focal_theta,
               float focal_sigma_theta,
               float focal_phi,
               float focal_sigma_phi
               ){
        // buffer the information into the data
        this->UID = UID;
        this->focal_X = focal_X;
        this->focal_sigma_X = focal_sigma_X;
        this->focal_Y = focal_Y;
        this->focal_sigma_Y = focal_sigma_Y;
        this->focal_theta = focal_theta;
        this->focal_sigma_theta = focal_sigma_theta;
        this->focal_phi = focal_phi;
        this->focal_sigma_phi = focal_sigma_phi;

        this->cutID = UID%10000;
        this->cutID = UID/10000;
    };

private:
    int UID = 0;
    int runID=0;
    int cutID=0;
    int sieveColID;
    int sieveRowID;

    float focal_X;
    float focal_sigma_X;
    float focal_Y;
    float focal_sigma_Y;
    float focal_theta;
    float focal_sigma_theta;
    float focal_phi;
    float focal_sigma_phi;

public:

    ///Diagnose the result
    void Print(){
        std::cout<<"RunID ::"<<runID<<std::endl;
        std::cout<<"\t X: ("<< focal_X<<","<<focal_sigma_X<<")  Y: ("<<
            focal_Y<<", "<<focal_sigma_Y<<")  Th : ("<<focal_theta<<", "<<
            focal_sigma_theta<<")  Phi : ("<<focal_phi<<", "<<focal_sigma_phi<<")"<<std::endl;
    };

    float distance(sieveInfor *infor){

        float d_sqX  = (this->focal_X - infor->focal_X)*(this->focal_X - infor->focal_X)/std::sqrt(this->focal_sigma_X*this->focal_sigma_X + infor->focal_sigma_X*infor->focal_sigma_X);
        float d_sqY  = (this->focal_Y - infor->focal_Y)*(this->focal_Y - infor->focal_Y)/std::sqrt(this->focal_sigma_Y*this->focal_sigma_Y + infor->focal_sigma_Y*infor->focal_sigma_Y);
        float d_sqTh = (this->focal_theta - infor->focal_theta) * (this->focal_theta - infor->focal_theta)/std::sqrt(this->focal_sigma_theta*this->focal_sigma_theta + infor->focal_sigma_theta * infor->focal_sigma_theta);
        float d_sqPh = (this->focal_phi   - infor->focal_phi) *(this->focal_phi - infor->focal_phi)/std::sqrt(this->focal_sigma_phi*this->focal_sigma_phi + infor->focal_sigma_phi*infor->focal_sigma_phi);

        float distance= std::sqrt(d_sqX + d_sqY + d_sqTh + d_sqPh);
        return  distance;
    }

    float* distance2(sieveInfor *infor){
        float distance[2];
        float d_sqX  = (this->focal_X - infor->focal_X)*(this->focal_X - infor->focal_X);
        float d_sqY  = (this->focal_Y - infor->focal_Y)*(this->focal_Y - infor->focal_Y);
        float d_sqTh = (this->focal_theta - infor->focal_theta) * (this->focal_theta - infor->focal_theta);
        float d_sqPh = (this->focal_phi   - infor->focal_phi) *(this->focal_phi - infor->focal_phi);


        return  distance;
    }



};

void getDistance(TString rootfilePath = "./data/LHRS/checkSieve_%d.root"){
    Int_t runList[]={2239,2240,2241};

    std::map<Int_t, sieveInfor> sieveInforList; // runID

    for(Int_t runIndex = 0; runIndex < sizeof(runList)/sizeof(Int_t);runIndex++){
        auto runID = runList[runIndex];
        TString rfname = Form(rootfilePath.Data(),runID);
        std::cout<<rfname.Data()<<std::endl;

        if(gSystem->AccessPathName(rfname.Data())){
            std::cout<<"File "<< rfname.Data()<<"  missing!!!"<<std::endl;
            exit(-1);
        }

        std::unique_ptr<TChain> chain(new TChain("OptRes"));
        chain->Add(rfname.Data());

        Int_t min_cutID = chain->GetMinimum("CutID");
        Int_t max_cutID = chain->GetMaximum("CutID");

        std::cout<<min_cutID<<","<<max_cutID<<std::endl;

        for(Int_t cutID = min_cutID; cutID <= max_cutID; cutID++){
            // get the information and project the datas
            //TH1F *focal_theta_temp = new TH1F(Form("focal_theta_temp"),Form("focal_theta_temp"),10000,-1,1);
            //TH1F *focal_phi_temp = new TH1F(Form("focal_phi_temp"),Form("focal_phi_temp"),10000,-1,1);
            //TH1F *focal_x_temp = new TH1F(Form("focal_x_temp"),Form("focal_x_temp"),10000,-1,1);
            //TH1F *focal_y_temp = new TH1F(Form("focal_y_temp"),Form("focal_y_temp"),10000,-1,1);

            std::unique_ptr<TH1F> focal_theta_temp( new TH1F(Form("focal_theta_temp"),Form("focal_theta_temp"),10000,-1,1));
            std::unique_ptr<TH1F> focal_phi_temp(new TH1F(Form("focal_phi_temp"),Form("focal_phi_temp"),10000,-1,1));
            std::unique_ptr<TH1F> focal_x_temp(new TH1F(Form("focal_x_temp"),Form("focal_x_temp"),10000,-1,1));
            std::unique_ptr<TH1F> focal_y_temp(new TH1F(Form("focal_y_temp"),Form("focal_y_temp"),10000,-1,1));

            //get the average information and  get the sigma information
            chain->Project(focal_x_temp->GetName(),"focalX", Form("CutID==%d",cutID));
            chain->Project(focal_y_temp->GetName(),"focalY", Form("CutID==%d",cutID));
            chain->Project(focal_theta_temp->GetName(),"focalTh",Form("CutID==%d",cutID));
            chain->Project(focal_phi_temp->GetName(),"focalPh",Form("CutID==%d",cutID));

//            std::cout<<"CutID "<<cutID<<std::endl;
//            std::cout<<"\t X " <<runID*10000 + cutID <<"  Entries :"<< focal_x_temp->GetEntries()<<"  Average :("<<
//                focal_x_temp->GetMean()<<","<<focal_x_temp->GetRMS()<<")"<<std::endl;
//
//            std::cout<<"\t Y "<<runID*10000 + cutID<<"  Entries :"<< focal_y_temp->GetEntries()<<"  Average :("<<
//                     focal_y_temp->GetMean()<<","<<focal_y_temp->GetRMS()<<")"<<std::endl;
//
//            std::cout<<"\t Th "<<runID*10000 + cutID<<"  Entries :"<< focal_theta_temp->GetEntries()<<"  Average :("<<
//                     focal_theta_temp->GetMean()<<","<<focal_theta_temp->GetRMS()<<")"<<std::endl;
//
//            std::cout<<"\t Ph "<<runID*10000 + cutID<<"  Entries :"<< focal_phi_temp->GetEntries()<<"  Average :("<<
//                     focal_phi_temp->GetMean()<<","<<focal_phi_temp->GetRMS()<<")"<<std::endl;

            if((focal_phi_temp->GetEntries() < 10) || (focal_x_temp->GetEntries()< 10) ||
               (focal_y_temp->GetEntries() < 10) || (focal_theta_temp->GetEntries() < 10)) continue;
            // buffer the data into the array
            Int_t UID = runID *10000+ cutID;
            auto infor = sieveInfor(UID,
                                    focal_x_temp->GetMean(),
                                    focal_x_temp->GetRMS(),
                                    focal_y_temp->GetMean(),
                                    focal_y_temp->GetRMS(),
                                    focal_theta_temp->GetMean(),
                                    focal_theta_temp->GetRMS(),
                                    focal_phi_temp->GetMean(),
                                    focal_phi_temp->GetRMS()
                                    );
            sieveInforList[UID] = infor;
//            sieveInforList[UID].Print();
        }
        std::cout<<"Total Number  ::"<<sieveInforList.size()<<std::endl;
    }
    //get the central sieve and get the distance
    std::vector<float> distanceArray;
    for (auto startIndex = sieveInforList.begin(); startIndex != sieveInforList.end(); startIndex++){
        for(auto endIndex = sieveInforList.begin();endIndex!=sieveInforList.end();endIndex++){
             auto value = startIndex->second.distance(& endIndex->second);
             std::cout<<startIndex->first <<"-->"<<endIndex->first<<"  ::  "<<value<<std::endl;
             if (startIndex->first == endIndex->first) continue;
             distanceArray.push_back(value);
        }
    }

    auto maxValue = std::max_element(distanceArray.begin(), distanceArray.end());
    auto minValue = std::max_element(distanceArray.begin(),distanceArray.end());

    TCanvas *canv = new TCanvas("Canvas","Canvas",1960,1080);
    canv->cd();
    canv->Draw();
    TH1F *disthist = new TH1F(Form("Distance_all_sieve"),Form("Distance_all_sieve"),100,(*minValue)*2-*maxValue, (*maxValue)*2 - *minValue);
    for (auto value : distanceArray){
        disthist->Fill(value);
    }
    disthist->Draw();
    canv->Update();
    canv->SaveAs("test.jpg");

}



int main(){

    getDistance();
    return 1;
}