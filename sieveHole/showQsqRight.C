{

// Macro to Qsq 

   gROOT->Reset();

   gStyle->SetOptStat(1);
   gStyle->SetStatH(0.3);
   gStyle->SetStatW(0.3);
   gStyle->SetTitleH(0.09);
   gStyle->SetTitleW(0.3);
   gStyle->SetLabelSize(0.04,"x");
   gStyle->SetLabelSize(0.04,"y");
   gROOT->ForceStyle();

// "gold" track is at the target.  For more info, see
// http://hallaweb/podd/doc/variables.html

   char ctrig1[50],ctrig5[50];
   char vdccut[200],tgtcut[200],tgty[200];
   char ccut1[800],ccut5[800],cand[5];
   char qtz[200],momc[200];

// Trigger choice : 
//   T1 = scintillator S0 trigger
//   T2 = upper scintillator (above quartz) 

 sprintf(ctrig1,"fEvtHdr.fEvtType==%d",1);

// HRS = R.* or L.*
   TFile *_file0 = TFile::Open("AfileR.root");

// only one cluster in each VDC plane
sprintf(vdccut,"R.vdc.u1.nclust==1&&R.vdc.v1.nclust==1&&R.vdc.u2.nclust==1&&R.vdc.v2.nclust==1");

// track points to target; avoid wildly wrong tracks
sprintf(tgtcut,"R.gold.th>-0.08&&R.gold.th<0.08&&R.gold.ph>-0.05&&R.gold.ph<0.05");

sprintf(qtz,"P.loQadcR>620");
sprintf(momc,"R.gold.dp>-0.01");

sprintf(cand,"&&");

// Build the total cut1 and cut2 

strcpy(ccut1,ctrig1);
strcat(ccut1,cand);
strcat(ccut1,vdccut);
strcat(ccut1,cand);
strcat(ccut1,tgtcut);
strcat(ccut1,cand);
strcat(ccut1,qtz);
strcat(ccut1,cand);
strcat(ccut1,momc);

cout << "Cut  : "<<endl;
cout << ccut1<<endl;

// We are more interested in trigger 1 for PREX-II

 TH1F *hqsq = new TH1F("hqsq","Right HRS Qsq",100,0.0012,0.016);

 // T->Draw("P.q2R>>hqsq",ccut1);

  TCanvas *c1 = new TCanvas("c1","c1");
  T->Draw("EK_R.Q2>>hqsq",ccut1);
  c1->Print("qsqdat.png");



}
