#include "CollimatorR.C"
#include "UpPlane.C"
#include "DownPlane.C"
#include "CREXdata.h"

//Author Ryan Richards 
//Shift polar angle in sim to match data and compute Q^2, Asym


//Run number, sept scan(0.0 - nominal), ADC, central angle for data, dp cut off,  offset in degrees(1 clearly no shift)

void AsymR(int run,double sept,double ADC,double th0, double dp, double off){

//gStyle->SetOptStat(111100);

char targpos[3][20] = { "-5mm","0mm","+5mm" }; 

char TargPos[3][20] = {"Minus5","Zero","Plus5"};

LoadTable("horpb.dat",0);//Lead table - sufficient

double d2r = TMath::Pi()/180;
double r2d = 1/d2r;

double scale = 0.9524/0.9512;//Num - 1 MeV energy loss, peak pos

//th0 is for data 
double cth0 = TMath::Cos(th0*d2r); double sth0 = TMath::Sin(th0*d2r);

double cthsim = TMath::Cos((4.74/off)*d2r);
double sthsim = TMath::Sin((4.74/off)*d2r);


TH1F *lab[2],*qsq[2], *asym[2], *hmom[2];
int color[2] = { 2, 4 };

for(int ii = 0; ii < 2; ii++){ 

   if(ii == 0){ qsq[ii] = new TH1F(Form("qsq[%i]",ii),"RHRS Apparent Q^{2} Data",150,0.0,0.015);
                lab[ii] = new TH1F(Form("lab[%i]",ii),"RHRS Apparent #theta_{lab} Data",150,1.0,9.0);
                asym[ii] = new TH1F(Form("asym[%i]",ii),"RHRS Apparent Asymmetry Data",150,0.0,1.0);
                hmom[ii] = new TH1F(Form("hmom[%i]",ii),"RHRS Apparent Asymmetry Data",150,0.938,0.955);
    } else{
    qsq[ii] = new TH1F(Form("qsq[%i]",ii),Form("RHRS Apparent Q^{2} Septum Detuned %.1f",sept),150,0.0,0.015);
    lab[ii] = new TH1F(Form("lab[%i]",ii),Form("RHRS Apparent #theta_{lab} Septum Detuned %.1f",sept),150,1.0,9.0);
    asym[ii] = new TH1F(Form("asym[%i]",ii),Form("RHRS Apparent Asymmetry Septum Detuned %.1f",sept),150,0.0,1.0);
    hmom[ii] = new TH1F(Form("hmom[%i]",ii),Form("RHRS Apparent Momentum Septum Detuned %.1f",sept),150,0.938,0.955);
    }

   asym[ii]->SetLineColor(color[ii]); qsq[ii]->SetLineColor(color[ii]); lab[ii]->SetLineColor(color[ii]); hmom[ii]->SetLineColor(color[ii]);

}
// simulation vertex distribution

TH1F *qsq_v = new TH1F("qsq_v",Form("RHRS sim vertex Q^{2} Septum Detuned %.1f",sept),150,0.0,0.015);
TH1F *lab_v = new TH1F("lab_v",Form("RHRS sim vertex #theta_{lab} Septum Detuned %.1f",sept),150,1.0,9.0);
TH1F *asym_v = new TH1F("asym_v",Form("RHRS sim vertex Asymmetry Septum Detuend %.1f",sept),150,0.0,1.0);


//For simulation
vector <double> Rate,Qsq,Asym,Lab, Mom;
vector <double> Qsq_v, Asym_v, Lab_v, Rate_v;
double thisRate, thisTh, thisPh, thisQsq, thisXcol, thisYcol, thisXvdc, thisAsym, thisAng,thisCosAng;
double thisE,thisP, thismom;
double thisxu1,thisxu2,thisyu1,thisyu2;
double thisxd1,thisxd2,thisxd3,thisxd4,thisxd5,thisxd6,thisxd7,thisxd8,thisxd9;
double thisyd1,thisyd2,thisyd3,thisyd4,thisyd5,thisyd6,thisyd7,thisyd8,thisyd9;
double thisAng1;
double thisQsq_v, thisAsym_v, thisLab_v;
int thisN; //For the vertex quantities

//For data, need cluster, adc,th,ph,p
vector <double> Angle,Qsqdat, Asymdat, Momdat;
double thisu1,thisv1,thisu2,thisv2,thisPdat, thisdPdat, thisThdat,thisPhdat,thisQsqdat,thisAsymdat,thisCosAngdat,thisAngdat,thisADC;
double thisEvt;



  TChain *T = new TChain("T");
//  T->Add(Form("../prex2Rootfiles/prexRHRS_%i_-1.root",run));
  T->Add(Form("prexRHRS_%i_-1.root",run));

  TChain *T1 = new TChain("T");
  //for(int c = 1; c < 6; c++){ T1->Add(Form("../septmistune/Rootfiles/Ebeam953/Minus1_SandwichRHRS_PREX_%.1f_%i.root",sept,c)); }
  //for(int c = 1; c < 6; c++){ T1->Add(Form("../septmistune/Rootfiles/Ebeam953/Zero1_SandwichRHRS_PREX_%.1f_%i.root",sept,c)); }
  for(int c = 1; c < 6; c++){ T1->Add(Form("../septmistune/Rootfiles/Ebeam953/Plus1_SandwichRHRS_PREX_%.1f_%i.root",sept,c)); }

//  T1->Add(Form("../septmistune/Rootfiles/Minus_SandwichRHRS_PREX_%.1f_1.root",sept));
//T1->Add(Form("../septmistune/Rootfiles/Zero_SandwichRHRS_PREX_%.1f_1.root",sept));
 //T1->Add(Form("../septmistune/Rootfiles/Plus_SandwichRHRS_PREX_%.1f_1.root",sept));


  T->SetBranchAddress("R.vdc.u1.nclust",&thisu1); T->SetBranchAddress("R.vdc.v1.nclust",&thisv1);
  T->SetBranchAddress("R.vdc.u2.nclust",&thisu2); T->SetBranchAddress("R.vdc.v2.nclust",&thisv2);
  T->SetBranchAddress("R.gold.th",&thisThdat); T->SetBranchAddress("R.gold.ph",&thisPhdat);
  T->SetBranchAddress("R.gold.p",&thisPdat); T->SetBranchAddress("P.upQadcR",&thisADC);
  T->SetBranchAddress("R.gold.dp",&thisdPdat); T->SetBranchAddress("P.evtypebits",&thisEvt);
  
  long n = T->GetEntries();
  
  //Looping over tree in data
  for(long i = 0; i < n; i++){
     T->GetEntry(i);
  
      thisPdat *= scale;
   int thisevent = (int) thisEvt;
  
   if(thisu1 == 1 && thisv1 == 1 && thisu2 == 1 && thisv2 == 1 && thisADC > ADC && abs(thisThdat)<0.08 && abs(thisPhdat)<0.05 && thisPdat >  0.9534*0.96 && thisPdat < 0.9534*1.002 && ((thisevent&2)==2)  ){
  
 
      thisCosAngdat = (cth0+thisPhdat*sth0)/(TMath::Sqrt(1+thisThdat*thisThdat+thisPhdat*thisPhdat));
      thisAngdat = r2d*TMath::ACos(thisCosAngdat);
 
       
     //  std::cout << thisAngdat << std::endl;     
      
      //Using a hardcoded beam energy -- run dependent, yes? --Beam Energy 953.4
       thisQsqdat = 2*0.9534*thisPdat*(1-thisCosAngdat);
       
       thisAsymdat = 1e6*Interpolate(thisPdat*1000,thisAngdat,0,1);
  
      // std::cout <<  thisAngdat  << "   "  << thisPdat*1000  << "  " << thisAsymdat << std::endl; 


       Angle.push_back(thisAngdat);
       Qsqdat.push_back(thisQsqdat);     
       Asymdat.push_back(thisAsymdat);
       Momdat.push_back(thisPdat);

  } }

 
 T1->SetBranchAddress("rate",&thisRate); T1->SetBranchAddress("x_vdc_tr",&thisXvdc);
 T1->SetBranchAddress("x_col_tr",&thisXcol); T1->SetBranchAddress("y_col_tr",&thisYcol);
 T1->SetBranchAddress("th_ztarg_tr",&thisTh); T1->SetBranchAddress("ph_ztarg_tr",&thisPh);
 T1->SetBranchAddress("ev.beamp",&thisP); T1->SetBranchAddress("ev.ep",&thisE);
 T1->SetBranchAddress("p_ztarg_tr",&thismom);

 T1->SetBranchAddress("x_zup1",&thisxu1); T1->SetBranchAddress("y_zup1",&thisyu1);
 T1->SetBranchAddress("x_zup2",&thisxu2); T1->SetBranchAddress("y_zup2",&thisyu2);
 T1->SetBranchAddress("x_zdown1",&thisxd1); T1->SetBranchAddress("y_zdown1",&thisyd1);
 T1->SetBranchAddress("x_zdown2",&thisxd2); T1->SetBranchAddress("y_zdown2",&thisyd2);
 T1->SetBranchAddress("x_zdown3",&thisxd3); T1->SetBranchAddress("y_zdown3",&thisyd3);
 T1->SetBranchAddress("x_zdown4",&thisxd4); T1->SetBranchAddress("y_zdown4",&thisyd4);
 T1->SetBranchAddress("x_zdown5",&thisxd5); T1->SetBranchAddress("y_zdown5",&thisyd5);
 T1->SetBranchAddress("x_zdown6",&thisxd6); T1->SetBranchAddress("y_zdown6",&thisyd6);
 T1->SetBranchAddress("x_zdown7",&thisxd7); T1->SetBranchAddress("y_zdown7",&thisyd7);
 T1->SetBranchAddress("x_zdown8",&thisxd8); T1->SetBranchAddress("y_zdown8",&thisyd8);
 T1->SetBranchAddress("x_zdown9",&thisxd9); T1->SetBranchAddress("y_zdown9",&thisyd9);

  //vertex asym, Q2 and theta
  T1->SetBranchAddress("ev.A",&thisAsym_v);
  T1->SetBranchAddress("ev.Q2",&thisQsq_v);
  T1->SetBranchAddress("ev.Th",&thisLab_v); //deg
  T1->SetBranchAddress("ev.nuclA",&thisN); //cut on lead events for Vertex 
 

 int m = T1->GetEntries();


 //Now loop over sim tree
 for(int j = 0; j < m; j++){ 
 T1->GetEntry(j);

  

  //This has momentum cut -- needs to be evaluated
  if( thisXvdc!=-333. && CollimatorR(thisXcol,thisYcol) && UpPlane(thisxu1,thisyu1,thisxu2,thisyu2,0) &&
  DownPlane(thisxd1,thisyd1,thisxd2,thisyd2,thisxd3,thisyd3,thisxd4,thisyd4,thisxd5,thisyd5,thisxd6,thisyd6,thisxd7,thisyd7,thisxd8,thisyd8,thisxd9,thisyd9,0) && ((thismom -50*thisTh) > dp) ){
 thisCosAng = (cthsim+thisPh*sthsim)/TMath::Sqrt(1+thisPh*thisPh+thisTh*thisTh);  
  
  //We will have to be careful here  
  thisAng = r2d*TMath::ACos(thisCosAng);
  //This is the Angle in degrees, now adding offset in degrees - difference in central value btwn hists, add/subtract event by event
  thisAng1 = thisAng;


  //Now with this, compute Q^2 and Asymmetry
  thisQsq = 2*0.9534*(thismom/1000)*(1-TMath::Cos(thisAng1*d2r));
  thisAsym = 1e6*Interpolate(thismom,thisAng1,0,1);

  // std::cout << thisAng1 << "   " << thisQsq << "   " << thisAsym << std::endl;


  Rate.push_back(thisRate); Qsq.push_back(thisQsq);
  Lab.push_back(thisAng1); Asym.push_back(thisAsym);
  Mom.push_back(thismom/1000);


  if(thisN == 208){  Qsq_v.push_back(thisQsq_v); Asym_v.push_back(thisAsym_v/1000);  Lab_v.push_back(thisLab_v); Rate_v.push_back(thisRate);  }

  }



 } 

 //Now fill histograms
 //data
 for(int l = 0; l < Qsqdat.size(); l++){ qsq[0]->Fill(Qsqdat[l]); lab[0]->Fill(Angle[l]); asym[0]->Fill(Asymdat[l]); hmom[0]->Fill(Momdat[l]); }
 //simulation
 for(int k = 0; k < Qsq.size(); k++){ qsq[1]->Fill(Qsq[k],Rate[k]); lab[1]->Fill(Lab[k],Rate[k]); asym[1]->Fill(Asym[k],Rate[k]); hmom[1]->Fill(Mom[k],Rate[k]); } 
 //simulation vertex 
 for(int m = 0; m < Qsq_v.size();m++){ qsq_v->Fill(Qsq_v[m],Rate_v[m]); asym_v->Fill(Asym_v[m],Rate_v[m]); lab_v->Fill(Lab_v[m],Rate_v[m]);      }




  auto leg = new TLegend(0.1,0.75,0.4,0.9);
  leg->AddEntry(lab[0],"Data","l");
  leg->AddEntry(lab[1],Form("Sim, Sept %.1f Per, TargPos %s",sept,targpos[1]),"l");
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
 



 TCanvas *c1 = new TCanvas();
 lab[0]->Draw("HIST sames");
 lab[1]->Draw("HIST sames");
 lab[1]->Scale(lab[0]->Integral()/lab[1]->Integral());
 leg->Draw();
 gPad->Update();
 TPaveStats* la11 = (TPaveStats*)lab[0]->FindObject("stats");
 TPaveStats* la12 = (TPaveStats*)lab[1]->FindObject("stats");
 la11->SetY2NDC(0.90);
 la11->SetY1NDC(0.75);
 la12->SetY2NDC(0.75);
 la12->SetY1NDC(0.60);
 la11->SetTextColor(1);
 la12->SetTextColor(2);
 //c1->SaveAs(Form("%s_PolarAngle1RHRS_%.1fper.png",TargPos[1],sept));

 TCanvas *c2 = new TCanvas();
 qsq[0]->Draw("HIST sames"); 
 qsq[1]->Draw("HIST sames");
 qsq[1]->Scale(qsq[0]->Integral()/qsq[1]->Integral());
 leg->Draw();
 gPad->Update();
 TPaveStats* qs1 = (TPaveStats*)qsq[0]->FindObject("stats");
 TPaveStats* qs2 = (TPaveStats*)qsq[1]->FindObject("stats");
 qs1->SetY2NDC(0.90);
 qs1->SetY1NDC(0.75);
 qs2->SetY2NDC(0.75);
 qs2->SetY1NDC(0.60);
 qs1->SetTextColor(1);
 qs2->SetTextColor(2);
// c2->SaveAs(Form("%s_Qsq1RHRS_%.1fper.png",TargPos[1],sept));



 TCanvas *c3 = new TCanvas();
 asym[0]->Draw("HIST sames");
 asym[1]->Draw("HIST sames");
 asym[1]->Scale(asym[0]->Integral()/asym[1]->Integral());
 leg->Draw();
 gPad->Update();
 TPaveStats* as1 = (TPaveStats*)asym[0]->FindObject("stats");
 TPaveStats* as2 = (TPaveStats*)asym[1]->FindObject("stats");
 as1->SetY2NDC(0.90);
 as1->SetY1NDC(0.75);
 as2->SetY2NDC(0.75);
 as2->SetY1NDC(0.60);
 as1->SetTextColor(1);
 as2->SetTextColor(2);
// c3->SaveAs(Form("%s_Asym1RHRS_%.1fper.png",TargPos[1],sept));
 

 
 //You can look at this for different target positions
 TCanvas *c4 = new TCanvas();
 c4->Divide(2,2);
 c4->cd(1);
 lab_v->Draw("HIST");
 c4->cd(2);
 qsq_v->Draw("HIST");
 c4->cd(3);
 asym_v->Draw("HIST");
// c4->SaveAs(Form("%s_Vertex1RHRS_%.1fper.png",TargPos[0],sept));

 /*
 TCanvas *c5 = new TCanvas();
 hmom[0]->Draw("HIST sames");
 hmom[1]->Draw("HIST sames");
 hmom[1]->Scale(hmom[0]->Integral()/hmom[1]->Integral());

 c5->SaveAs("Mom.pdf");  
*/



//  std::cout << "Data Lab theta mean: " << lab[0]->GetMean() << "  " << "Data Lab theta rms: " << lab[0]->GetRMS() << std::endl;
   std::cout << "Sim Lab theta mean: " << lab[1]->GetMean() << "   " << "Sim Lab theta rms: " << lab[1]->GetRMS() << std::endl;
//  std::cout << "Data  Qsq mean: " << qsq[0]->GetMean() << "  " << "Data  Qsq rms: " << qsq[0]->GetRMS() << std::endl;
   std::cout << "Sim  Qsq mean: " << qsq[1]->GetMean() << "   " << "Sim  Qsq rms: " << qsq[1]->GetRMS() << std::endl;
//  std::cout << "Data Asymmetry  mean: " << asym[0]->GetMean() << "  " << "Data Asymmetry  rms: " << asym[0]->GetRMS() << std::endl;
   std::cout << "Sim Asymmetry  mean: " << asym[1]->GetMean() << "   " << "Sim Asymmetry  rms: " << asym[1]->GetRMS() << std::endl;



  std::cout<<"-------- vertex ----------"<<std::endl;
  std::cout << "Sim Lab theta mean: " << lab_v->GetMean() << "   " << "Sim Lab theta rms: " << lab_v->GetRMS() << std::endl;
  std::cout << "Sim  Qsq mean: " << qsq_v->GetMean() << "   " << "Sim  Qsq rms: " << qsq_v->GetRMS() << std::endl;
  std::cout << "Sim Asymmetry  mean: " << asym_v->GetMean() << "   " << "Sim Asymmetry  rms: " << asym_v->GetRMS() << std::endl;








 }
