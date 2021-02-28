{ 
// Macro to plot angles at target region

   gROOT->Reset();

   gStyle->SetOptStat(0);
   gStyle->SetStatH(0.3);
   gStyle->SetStatW(0.3);
   gStyle->SetTitleH(0.09);
   gStyle->SetTitleW(0.3);
   gStyle->SetLabelSize(0.05,"x");
   gStyle->SetLabelSize(0.05,"y");
   gROOT->ForceStyle();

// "gold" track is at the target.  For more info, see
// http://hallaweb/podd/doc/variables.html

 char ctrig[200],vdccut[200],tgtcut[200],tgty[200],ccut[800],cand[5];

 sprintf(ctrig,"fEvtHdr.fEvtType==1");

// HRS = R.* or L.*
 TFile *_file0 = TFile::Open("AfileR.root");

// only one cluster in each VDC plane
sprintf(vdccut,"R.vdc.u1.nclust==1&&R.vdc.v1.nclust==1&&R.vdc.u2.nclust==1&&R.vdc.v2.nclust==1");

sprintf(cand,"&&");

// Build the total cut

strcpy(ccut,ctrig);
strcat(ccut,cand);
strcat(ccut,vdccut);

cout << "Cuts used : "<<endl;

 TCanvas c0;

cout << ccut << endl;

TH1F *htgth = new TH1F("htgth","Target theta (tg_th)",100,-0.08,0.06);

 TH1F *htgph = new TH1F("htgph","Target phi (tg_ph)",100,-0.04,0.04);

 TCanvas c1;
 c1.Divide(1,2);
 c1.cd(1);

 T->Draw("R.gold.th>>htgth",ccut);

 c1.cd(2);

 T->Draw("R.gold.ph>>htgph",ccut);


  c1.Print("TargAngs.png"); 


 // htgthph->Draw("box");


}
