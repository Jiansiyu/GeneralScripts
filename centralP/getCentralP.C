/*
 * getCentralP.C
 *
 *  Created on: May 23, 2020
 *      Author: newdriver
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
//#include <sys/stat.h>

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

// central momentum information
void getCentralP(UInt_t runID,TString folder="/home/newdriver/Storage/Research/CRex_Experiment/RasterReplay/Replay/Result/"){
	TString HRS="R";
	if(runID<20000){
		HRS="L";
	}



	std::cout<<"Working on HRS :"<<HRS.Data() <<"("<<runID<<")"<<std::endl;
	// load the data
	auto chain= LoadrootFile(runID,folder);

	TCanvas *centralMomCanv=new TCanvas("Central Momentum Diagnose","Central Momentum Diagnose",1960,1080);
	centralMomCanv->Divide(2,1);

	centralMomCanv->Draw();
	centralMomCanv->cd(1);

	//for the HRS
	TH1F *HRSCentralPDetHH;

	if(HRS == "L"){
		std::cout<<"LHRS using the Hall Probe value "<<std::endl;
		HRSCentralPDetHH = new TH1F(Form("HallLProb_%d",runID),Form("HallLProb_%d",runID), 1000, -0.81, 0.82);
		chain->Project(HRSCentralPDetHH->GetName(),
							"HacL_D_LS450_FLD_DATA", generalcut.Data());

		HRSCentralPDetHH->Draw();

		if (HRSCentralPDetHH->GetEntries() != 0) {
			double CentralP = std::abs((HRSCentralPDetHH->GetMean()) * 0.95282 / 0.33930);

			HRSCentralPDetHH->GetXaxis()->SetRangeUser(HRSCentralPDetHH->GetMean()-0.005,HRSCentralPDetHH->GetMean()+0.005);
			TPaveText  *text=new TPaveText(0.1,0.6,0.4,0.9,"NDC");

			text->AddText(Form("LHRS     Probe: %f",HRSCentralPDetHH->GetMean()));
			text->AddText(Form("LHRS Central P: %f",CentralP));
			text->AddText(Form("LHRS Equation: Probe*0.95282/0.33930"));
			text->Draw("same");
			std::cout << "CentralMomentum is ::" << (CentralP) << std::endl;

		}else{
			std::cout << "\033[1;33m [Warning]\033[0m Missing HallLProb:"
									<<runID<< std::endl;
		}
	}
	else{
		std::cout<<"RHRS using the NMR value"<<std::endl;
		//HacR_D1_NMR_SIG
		double CentralP;
		TH1F *HRSCentralPDetHH = new TH1F(Form("HallR_NMR_%d",runID),Form("HallR_NMR_%d",runID), 1000, 0.2, 0.9);
		chain->Project(HRSCentralPDetHH->GetName(), "HacR_D1_NMR_SIG",
				generalcut.Data());
		HRSCentralPDetHH->Draw();
		if (HRSCentralPDetHH->GetEntries()) {
			double Mag = HRSCentralPDetHH->GetMean();
			CentralP = 2.702 * (Mag) - 1.6e-03 * (Mag) * (Mag) * (Mag);
			HRSCentralPDetHH->GetXaxis()->SetRangeUser(HRSCentralPDetHH->GetMean()-0.001,HRSCentralPDetHH->GetMean()+0.001);

			TPaveText *text=new TPaveText(0.1,0.6,0.4,0.9,"NDC");
			text->SetFillColor(3025);
			text->AddText(Form("RHRS       NMR: %f",Mag));
			text->AddText(Form("RHRS CenntralP: %f",CentralP));
			text->AddText(Form("RHRS  Equation: %s","2.702 * (Mag) - 1.6e-03 * (Mag)^{3} "));
//			text->SetTextColor(kRed);
			text->Draw("same");
		} else {
			std::cout << "\033[1;33m [Warning]\033[0m Missing HallR_NMR:"
					<< std::endl;
		}
	}

	// get the bpm informations
	double beamposX;
	double beamposY;

	centralMomCanv->cd(2)->Divide(1,2);
	{
		centralMomCanv->cd(2)->cd(1);
		TH1F *targx=new TH1F(Form("targx_%d",runID),Form("targx_%d",runID),500,-3,3);
		chain->Project(targx->GetName(),Form("targx"),generalcut.Data());
		targx->Draw();
		targx->Fit("gaus");
		beamposX=targx->GetFunction("gaus")->GetParameter(1);
		TLatex *txtPosx=new TLatex(targx->GetFunction("gaus")->GetParameter(1),targx->GetFunction("gaus")->GetParameter(0),Form("X:%f",beamposX));
		txtPosx->Draw("same");

		centralMomCanv->cd(2)->cd(2);
		TH1F *targy=new TH1F(Form("targy_%d",runID),Form("targy_%d",runID),500,-3,3);
		chain->Project(targy->GetName(),Form("targy"),generalcut.Data());
		targy->Draw();
		targy->Fit("gaus");
		beamposY=targy->GetFunction("gaus")->GetParameter(1);

		TLatex *txtPosy=new TLatex(targy->GetFunction("gaus")->GetParameter(1),targy->GetFunction("gaus")->GetParameter(0),Form("X:%f",beamposY));
		txtPosy->Draw("same");

	}



	centralMomCanv->Update();
	centralMomCanv->SaveAs(Form("DiagnosePlot/CentralP_%d.png",runID));


	// write the beam position infomation to file
	std::ofstream outfile;
	outfile.open("file.dat", std::ios::app);

	std::string outPut(Form("if(inputFilename.find(\"%d\")!=std::string::npos){\n d[5]=%f;\n d[6]=%f;}\n\n",runID, beamposX,beamposY));

	outfile << outPut.c_str()<<std::endl;


	// get the start and the end time stamp for the run
}


void getBeamPos(UInt_t runID,TString folder="/home/newdriver/Storage/Research/CRex_Experiment/RasterReplay/Replay/Result/"){


}
