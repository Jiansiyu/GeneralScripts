{ 
// Macro to plot sieve pattern

   gROOT->Reset();

   gStyle->SetOptStat(0);
   gStyle->SetStatH(0.3);
   gStyle->SetStatW(0.3);
   gStyle->SetTitleH(0.09);
   gStyle->SetTitleW(0.3);
   gStyle->SetLabelSize(0.04,"x");
   gStyle->SetLabelSize(0.04,"y");
   gROOT->ForceStyle();

// "gold" track is at the target.  For more info, see
// http://hallaweb/podd/doc/variables.html

 char ctrig[200],vdccut[200],tgtcut[200],tgty[200],ccut[800],cand[5];

 sprintf(ctrig,"fEvtHdr.fEvtNum<150000&&(fEvtHdr.fEvtType==1||fEvtHdr.fEvtType==5)");

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

//TH2F *htgthph = new TH2F("htgthph","Target theta-phi (theta vertical)",100,-0.06,0.06,100,-0.1,0.1);

TH2F *htgthph = new TH2F("htgthph","Target theta-phi (theta vertical)",100,-0.03,0.03,100,-0.05,0.05);

T->Draw("R.tr.tg_th:R.tr.tg_ph>>htgthph",ccut);


// TCanvas c1;
 // c1->Divide(2,1);
 // c1->cd(1);

  //htgthph->Draw("box");

TCanvas *c1 = new TCanvas("c1","c1");
htgthph->Draw();
c1->Print("SieveRight.png");


}
