{

// Macro to show focal plane distributions 

   gROOT->Reset();

   gStyle->SetOptStat(0);
   gStyle->SetStatH(0.3);
   gStyle->SetStatW(0.3);
   gStyle->SetTitleH(0.15);
   gStyle->SetTitleW(0.5);
   gStyle->SetLabelSize(0.044,"x");
   gStyle->SetLabelSize(0.044,"y");
   gROOT->ForceStyle();

// For more info on track variables, see
// http://hallaweb/podd/doc/variables.html

   char ctrig1[50],ctrig5[50];
   char vdccut[200],tgtcut[200],tgty[200];
   char ccut1[800],ccut5[800],cand[5];
   char qtz[200],momc[200];

// Trigger choice : 
//   T1 = scintillator S0 trigger


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
//strcat(ccut1,cand);
//strcat(ccut1,qtz);
//strcat(ccut1,cand);
//strcat(ccut1,momc);

cout << "Cut  : "<<endl;
cout << ccut1<<endl;

 TCanvas c1;
 c1.Divide(2,2);
 c1.cd(1);

   TH1F *hx = new TH1F("hx","X distribution at detector plane",100,-0.8,0.1);

   T->Draw("R.tr.x+0.9*R.tr.th>>hx",ccut1);

   c1.cd(2);

   TH1F *hy = new TH1F("hy","Y distribution at detector plane",100,-0.05,0.05);
   T->Draw("R.tr.y+0.9*R.tr.ph>>hy",ccut1);

   c1.cd(3);

   TH1F *hth = new TH1F("hth","transport theta at detectors",100,-0.1,0.03);
 
   T->Draw("R.tr.th>>hth",ccut1);

   c1.cd(4);

   TH1F *hph = new TH1F("hph","transport phi at detectors",100,-0.045,0.045);
   T->Draw("R.tr.ph>>hph",ccut1);
   



}
