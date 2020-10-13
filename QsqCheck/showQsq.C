#include <TROOT.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TF1.h>
#include <TStyle.h>
#include <TColor.h>
#include <TGaxis.h>
#include <TCanvas.h>
#include <TPaveLabel.h>
#include <TPaveText.h>
#include <TGraphErrors.h>
#include <fstream>
#include <iostream>
#include "./systematic/CREXdata.h"
#include "TChain.h"
#include "TMath.h"
#include "TCut.h"
#include "TLatex.h"
#include "TPaveStats.h"
void showQsq(int run, double E){ 

  gROOT->Reset();
  //gStyle->SetOptStat(1002211);
  gStyle->SetOptStat(2211);
  gStyle->SetOptFit(1111);
  gStyle->SetTitleYOffset(1.3);
  gROOT->ForceStyle();
  TGaxis::SetMaxDigits(3);

  //Loads the Horowitz tables
  LoadTable("horpb.dat",0); //unstretched 
  LoadTable("horpb1.dat",1); //stretched

  //double E = 0.953774;//beam energy
  double xCut = -0.076;//beam energy
 
  TCanvas *c1 = new TCanvas("c1","c1",1000,600);
   //ROOT 6 memory management makes me use vectors
   vector <double> Qsq, Qsq1, Angle, ASYM, ASYM_ST, Sens,TgtY; 
 

      double thisThtg, thisPhtg, thisP, thisDet;
      double thisu1, thisu2, thisv1, thisv2;
      double thisXvdc[10], thisThvdc[10], thisYvdc[10], thisPhvdc[10];
      double thisAsym, thisAsymSt;
     
      double xq,yq;//For quartz projection
      double thisAngle; //for reconstructed angle
      double thisCosAng; // for cos angle  
      double thisQsq,thisQsq1;//for Qsq
      double frac;//for Qsq
      double thisEvt;//for eventtype
      double thisdp;//for dp
      double thistgty;//for tgty

  //int run_num = (int)T->GetMaximum("fEvtHdr.fRun");
  int run_num = run;
  double dummycut = 497.8; 
  TString outfile = Form("./plots/Final/Qsq_run_%d.pdf",run_num);
  TFile f1(Form("./QsqRoot/Final/Qsq_run_%d.root",run_num),"RECREATE");
  //std::cout<<" outputfile "<<outfile<<std::endl;
  c1->Print(Form("%s[",outfile.Data()),"pdf");
  TChain *T = new TChain("T");
  TChain *E1 = new TChain("E");
  TChain *TSRight = new TChain("TSRight");
  TChain *TSLeft = new TChain("TSLeft");
  if(run_num < 10000){
      //LHRS
      double upADCcut_approx = 485;
      double x_edge = -0.076;
        T->Add(Form("finalrootfiles/prexLHRS_%d_-1*.root",run_num));
        E1->Add(Form("finalrootfiles/prexLHRS_%d_-1*.root",run_num));
        TSLeft->Add(Form("finalrootfiles/prexLHRS_%d_-1*.root",run_num));
//      T->Add(Form("prex_counting/prexLHRS_%d_200000.root",run_num));
      gStyle->SetStatX(0.90);
      gStyle->SetStatY(0.90);
      gStyle->SetStatW(0.25);
      gStyle->SetStatH(0.10);

      TH1F* huq = new TH1F("huq", "P.upQadcL;ADC CH;Events/CH", 170, 430, 600);
      T->Draw("P.upQadcL>>huq");
      TH1F* huqCpy = (TH1F*)huq->Clone("huq");
      huqCpy->GetXaxis()->SetRangeUser(upADCcut_approx-10,upADCcut_approx+10);
      double upADCcut = huqCpy->GetXaxis()->GetBinCenter(huqCpy->GetMinimumBin());
      cout<<"ADC cut: "<<upADCcut<<endl;

      TLine* line = new TLine(upADCcut,0.0,upADCcut,huq->GetMaximum());
      line->SetLineColor(kMagenta);
      line->Draw(); 
      c1->Print(Form("%s",outfile.Data()),"pdf");

//      TCut cut_wadc = Form("((P.evtypebits&2)==2&&L.vdc.u1.nclust==1&&L.vdc.v1.nclust==1&&L.vdc.u2.nclust==1&&L.vdc.v2.nclust==1&&L.gold.th>-0.08&&L.gold.th<0.08&&L.gold.ph>-0.05&&L.gold.ph<0.05&&P.upQadcL>%f)",upADCcut);
      //TCut cut_wadc = Form("((P.evtypebits&2)==2&&L.vdc.u1.nclust==1&&L.vdc.v1.nclust==1&&L.vdc.u2.nclust==1&&L.vdc.v2.nclust==1&&L.gold.th>-0.08&&L.gold.th<0.08&&L.gold.ph>-0.05&&L.gold.ph<0.05&&L.gold.dp>-0.006&&L.gold.dp<-0.002&&P.upQadcL>%f)",upADCcut);
      TCut cut_wadc = Form("((P.evtypebits&2)==2&&L.vdc.u1.nclust==1&&L.vdc.v1.nclust==1&&L.vdc.u2.nclust==1&&L.vdc.v2.nclust==1&&P.upQadcL>%f)",upADCcut);

      TCut cut_trig = Form("(P.evtypebits&2)==2");
      //TCut cut_trig = Form("2==2");
      TCut cut_cluster = Form("L.vdc.u1.nclust==1&&L.vdc.v1.nclust==1&&L.vdc.u2.nclust==1&&L.vdc.v2.nclust==1");
      //TCut cut_cluster = Form("1==1");
      TCut cut_wildtrack = Form("L.gold.th>-0.08 && L.gold.th<0.08 && L.gold.ph>-0.05 && L.gold.ph<0.05 && L.gold.dp>-0.04 && L.gold.dp<-0.002");
      //TCut cut_wildtrack = Form("1==1");
      TCut cut_sig = Form("P.upQadcL>%f",upADCcut);
      TCut cut_ped = Form("P.upQadcL<=%f",upADCcut);
      TCut x_cut = Form("(L.tr.x[0]+1.3*L.tr.th[0]) > %f",x_edge);


      TH1F* QsqL = new TH1F("QsqL", Form("LHRS Qsq from T-tree using T1 trigger (run%d)",run_num), 100, 0.002, 0.015);

      T->Draw("EK_L.Q2>>QsqL", cut_trig+cut_cluster+cut_wildtrack+cut_sig);
      QsqL->SetXTitle("Qsq(GeV/c)^{2}");
      QsqL->SetLineColor(kBlue);
      QsqL->SetTitle("Qsq from analyzer");
      QsqL->Draw();
      c1->Print(Form("%s",outfile.Data()),"pdf");



      TH1F* hy1 = new TH1F("hy1", "Projected y on detector plane", 200, -0.08, 0.02);
      TH1F* hyup2 = new TH1F("hyup2", "Projected y on detector plane + adc>cut", 200, -0.08, 0.02);
      TH1F* hyup3 = new TH1F("hyup3", "Projected y on detector plane + adc<cut", 200, -0.08, 0.02);
      T->Project(hy1->GetName(), "L.tr.y[0]+1.3*L.tr.ph[0]", cut_trig+cut_cluster+cut_wildtrack+x_cut);
      T->Project(hyup2->GetName(), "L.tr.y[0]+1.3*L.tr.ph[0]", cut_trig+cut_cluster+cut_sig+cut_wildtrack+x_cut);
      T->Project(hyup3->GetName(), "L.tr.y[0]+1.3*L.tr.ph[0]", cut_trig+cut_cluster+cut_wildtrack+cut_ped+x_cut);
      hy1->SetXTitle("L.tr.y[0]+1.3*L.tr.ph[0] (m)");
      hy1->SetLineColor(1);
      hyup2->SetLineColor(2);
      hyup3->SetLineColor(4);
      hy1->Draw();
      hyup2->Draw("same");
      hyup3->Draw("same");

      TLatex latex;
      latex.SetNDC();
      latex.SetTextSize(0.05);
      latex.SetTextColor(1);
      latex.DrawLatex(0.20,0.85,"All events");
      latex.SetTextColor(2);
      latex.DrawLatex(0.20,0.80,"US accepted");
      latex.SetTextColor(4);
      latex.DrawLatex(0.20,0.75,"US missed");
      c1->Print(Form("%s",outfile.Data()),"pdf");


      TH1F* hx1 = new TH1F("hx1", "Projected x on detector plane", 300, -0.14, 0.03);
      TH1F* hxup2 = new TH1F("hxup2", "Projected x on detector plane + adc>cut", 300, -0.14, 0.03);
      TH1F* hxup3 = new TH1F("hxup3", "Projected x on detector plane + adc<cut", 300, -0.14, 0.03);

      T->Project(hx1->GetName(), "L.tr.x[0]+1.3*L.tr.th[0]", cut_trig+cut_cluster+cut_wildtrack);
      T->Project(hxup2->GetName(), "L.tr.x[0]+1.3*L.tr.th[0]", cut_trig+cut_cluster+cut_wildtrack+cut_sig);
      T->Project(hxup3->GetName(), "L.tr.x[0]+1.3*L.tr.th[0]", cut_trig+cut_cluster+cut_wildtrack+cut_ped);
      hx1->SetXTitle("L.tr.x[0]+1.3*L.tr.th[0] (m)");
      hx1->SetLineColor(1);
      hxup2->SetLineColor(2);
      hxup3->SetLineColor(4);
      hx1->Draw();
      hxup2->Draw("same");
      hxup3->Draw("same");
      TF1* fit_x = new TF1("fit_x","gaus",hxup2->GetBinCenter(hxup2->GetMaximumBin())-0.01,hxup2->GetBinCenter(hxup2->GetMaximumBin())+0.01);
      hxup2->Fit(fit_x,"R");

      latex.SetTextColor(1);
      latex.DrawLatex(0.20,0.85,"All events");
      latex.SetTextColor(2);
      latex.DrawLatex(0.20,0.80,"US accepted");
      latex.SetTextColor(4);
      latex.DrawLatex(0.20,0.75,"US missed");
      latex.SetTextColor(6);
      latex.DrawLatex(0.20,0.70,Form("Peak = %1.4f",fit_x->GetParameter(1)));
      latex.DrawLatex(0.20,0.65,Form("Edge = %1.4f",x_edge));
      latex.SetTextColor(49);
      /*latex.DrawLatex(0.20,0.60,Form("x_cut = %1.3f",xCut));     

      TLine* xCut_line = new TLine(xCut,0.0,xCut,hx1->GetMaximum()/2.0);
      xCut_line->SetLineColor(49);
      xCut_line->SetLineWidth(2);
      xCut_line->Draw();*/
      c1->Print(Form("%s",outfile.Data()),"pdf");
    
      //Central Angle
      double th0 = 4.84*TMath::Pi()/180;
      double cth0 = TMath::Cos(th0);
      double sth0 = TMath::Sin(th0);
    
      T->SetBranchAddress("P.upQadcL",&thisDet);
      T->SetBranchAddress("P.evtypebits",&thisEvt);
      T->SetBranchAddress("L.gold.y",&thistgty);
      T->SetBranchAddress("L.gold.th",&thisThtg);
      T->SetBranchAddress("L.gold.ph",&thisPhtg);
      T->SetBranchAddress("L.gold.p",&thisP);
      T->SetBranchAddress("L.gold.dp",&thisdp);
      T->SetBranchAddress("L.vdc.u1.nclust",&thisu1);
      T->SetBranchAddress("L.vdc.v1.nclust",&thisv1);
      T->SetBranchAddress("L.vdc.u2.nclust",&thisu2);
      T->SetBranchAddress("L.vdc.v2.nclust",&thisv2);
      
      // --- This segfaults when I loop it for some reason
      T->SetBranchAddress("L.tr.x",&thisXvdc[0]);
      T->SetBranchAddress("L.tr.y",&thisYvdc[0]);
      T->SetBranchAddress("L.tr.th",&thisThvdc[0]);
      T->SetBranchAddress("L.tr.ph",&thisPhvdc[0]);
     
      //Asymmetry table is in degrees so we need to convert from radians to degrees
      double radtodeg = 180/TMath::Pi();
    
      long n = T->GetEntries();
    
      for(long i = 0; i < n; i++){ 
 //     for(long i = 0; i < n && i<43000; i++){ 
    
        T->GetEntry(i);
      
        xq = thisXvdc[0]+1.3*thisThvdc[0]; yq = thisYvdc[0]+1.3*thisPhvdc[0];
        int thisevent = (int)thisEvt; 
        //VDC cut and x cut
    //     if( thisu1==1 && thisv1==1 && thisu2==1 && thisv2==1 && thisThtg>-0.08 && thisThtg<0.08&& thisPhtg>-0.05 && thisPhtg<0.05 &&thisdp>-0.04 && thisdp<-0.002 && thisDet>upADCcut ){
         if( thisu1==1 && thisv1==1 && thisu2==1 && thisv2==1 && thisThtg>-0.08 && thisThtg<0.08&& thisPhtg>-0.05 && thisPhtg<0.05 &&thisdp>-0.04 && thisdp<-0.002 && thisDet>upADCcut && ((thisevent&2)==2) ){
         //if( thisu1==1 && thisv1==1 && thisu2==1 && thisv2==1 && thisDet>upADCcut && ((thisevent&2)==2) ){
     
         thisCosAng = (cth0 - thisPhtg*sth0)/TMath::Sqrt(1+thisThtg*thisThtg+thisPhtg*thisPhtg);
         thisAngle = radtodeg*TMath::ACos(thisCosAng);
        
         thisQsq = 2*E*thisP*(1-thisCosAng);
         frac = 1./(1.+(E/(208*0.931494))*(1-thisCosAng));
         thisQsq1 = 2*E*E*frac*(1-thisCosAng);
    
          Qsq.push_back(thisQsq); 
          Qsq1.push_back(thisQsq1); 
          Angle.push_back(thisAngle);
          TgtY.push_back(thistgty);
          //Energy table in MeV so need to be careful here
          thisAsym = 1e6*Interpolate(thisP*1000,thisAngle,0,1);//A (ppm)
          thisAsymSt = 1e6*Interpolate(thisP*1000,thisAngle,1,1); // Stretched A (ppm); 
    
          ASYM.push_back(thisAsym);ASYM_ST.push_back(thisAsymSt);//Stretched A (ppm)
          Sens.push_back(fabs(thisAsym-thisAsymSt)/thisAsym);//Sensitivity - Why not?
      }
     }
    
     //Histograms 
     TH1F *Qsq_1 = new TH1F("Qsq_1","Q^{2} from dp, E and angle:  (GeV/c)^{2}",100,0.002,0.015);
     TH1F *Qsq_2 = new TH1F("Qsq_2","Q^{2} from incident energy and angle (GeV/c)^{2}",100,0.002,0.015);
     TH1F *Theta = new TH1F("Theta","#theta_{lab} (deg)",150,2,8);
     TH1F *Asym = new TH1F("Asym","Asymmetry (ppm)",150,0,1);
     TH1F *SAsym = new TH1F("SAsym","Stretched Asymmetry (ppm)",150,0,1);
     TH1F *sens = new TH1F("sens","Sensitivity",150,0,0.05);
     TH1F *htgY = new TH1F("htgY","Target Y ",200,0.0002,0.001);
   /*
     Q2->SetTitle(Form("Q^{2} (GeV/c)^{2}, xCut = %1.3f m",xCut));
     Theta->SetTitle(Form("#theta_{lab} (deg), xCut = %1.3f m",xCut));
     Asym->SetTitle(Form("Asymmetry (ppm), xCut = %1.3f m",xCut));
     SAsym->SetTitle(Form("Stretched Asym. (ppm),  xCut = %1.3f m",xCut));
     sens->SetTitle(Form("Sensitivity, xCut = %1.3f m",xCut));
    */
     htgY->GetXaxis()->SetTitle("Target Y (m)");
     Qsq_1->GetXaxis()->SetTitle("Q^{2} (GeV/c)^{2} from E, dp and angle");
     Qsq_2->GetXaxis()->SetTitle("Q^{2} (GeV/c)^{2} from E and angle");
     Theta->GetXaxis()->SetTitle("#theta_{lab} (deg)");
     Asym->GetXaxis()->SetTitle("Asymmetry (ppm)");
     SAsym->GetXaxis()->SetTitle("Stretched Asym. (ppm)");
    
     for(int k = 0; k < Qsq.size(); k++){
    
     Qsq_1->Fill(Qsq[k]);
     Qsq_2->Fill(Qsq1[k]);
     Theta->Fill(Angle[k]);
     Asym->Fill(ASYM[k]);
     SAsym->Fill(ASYM_ST[k]);
     sens->Fill(Sens[k]);
     htgY->Fill(TgtY[k]);
     }
      long QsqEntry = Qsq_1->GetEntries();
      int binmax = htgY->GetMaximumBin();
      double Tgt_bin = htgY->GetXaxis()->GetBinCenter(binmax);
      std::cout<<"run_numi  Qsq_1->GetMean(1) Qsq_1->GetStdDev()/QsqEntry Asym->GetMean(1) Asym->GetStdDev()/sqrt(1.*QsqEntry) sens->GetMean(1) sens->GetStdDev()/sqrt(1.*QsqEntry) Theta->GetMean(1) Theta->GetStdDev()/sqrt(1.*QsqEntry) Tgt_Y err(tgtY) Tgt_bin(peak)"<<std::endl;
      std::cout<<run_num<<" "<<Qsq_1->GetMean(1)<<" "<<Qsq_1->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<Asym->GetMean(1)<<" "<<Asym->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<sens->GetMean(1)<<" "<<sens->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<Theta->GetMean(1)<<" "<<Theta->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<1000*htgY->GetMean(1)<<" "<<1000*htgY->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<1000*Tgt_bin<<std::endl;
      //Qsq_1
      Qsq_1->Draw();
      Qsq_1->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");
      std::cout<<"Run : "<<run_num<<" Entry "<<QsqEntry<<" Qsq "<<Qsq_1->GetMean(1)<<" Qsq error "<<Qsq_1->GetStdDev(1)<<" Qsq err/sqrt(entry) "<<Qsq_1->GetStdDev()/sqrt(1.*QsqEntry)<<std::endl;

      //Qsq_2
      Qsq_2->Draw();
      Qsq_2->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //Scattering angle
      Theta->Draw();
      Theta->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //Asymmetry
      Asym->Draw();
      Asym->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");
      //std::cout<<"Run : "<<run_num<<" Entry "<<QsqEntry<<" Asym "<<Asym->GetMean(1)<<" Asym error "<<Asym->GetStdDev(1)<<" Asym err/sqrt(entry) "<<Asym->GetStdDev()/sqrt(1.*QsqEntry)<<std::endl;

      //sAsymmetry
      SAsym->Draw();
      SAsym->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //sens
      sens->Draw();
      sens->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //Tht Y
      htgY->Draw();
      htgY->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");


      //Momentum
      TH1F *p_unGated = new TH1F("p_unGated","momentum distributions ungated",200,0.91,0.97);
      TH1F *p_SigGated = new TH1F("p_SigGated","momentum distributions with Signal gated",200,0.91,0.97);
      TH1F *p_PedGated = new TH1F("p_PedGated","momentum distributions with pedestal gated",200,0.91,0.97);
      T->Draw("L.gold.p>>p_unGated",cut_trig+cut_cluster+cut_wildtrack,"hist");
      T->Draw("L.gold.p>>p_SigGated",cut_trig+cut_cluster+cut_wildtrack+cut_sig,"hist sames");
      T->Draw("L.gold.p>>p_PedGated",cut_trig+cut_cluster+cut_wildtrack+cut_ped,"hist sames");
      p_unGated->SetLineColor(1);
      p_SigGated->SetLineColor(2);
      p_PedGated->SetLineColor(4);

      latex.SetTextColor(1);
      latex.DrawLatex(0.20,0.85,"All events");
      latex.SetTextColor(2);
      latex.DrawLatex(0.20,0.80,"US accepted");
      latex.SetTextColor(4);
      latex.DrawLatex(0.20,0.75,"US missed");
      gPad->Update();
 
      TPaveStats *p_unGated_pad = (TPaveStats*)p_unGated->FindObject("stats");
      TPaveStats *p_SigGated_pad = (TPaveStats*)p_SigGated->FindObject("stats");
      TPaveStats *p_PedGated_pad = (TPaveStats*)p_PedGated->FindObject("stats");
      p_unGated_pad->SetY2NDC(0.90);
      p_unGated_pad->SetY1NDC(0.75);
      p_SigGated_pad->SetY2NDC(0.75);
      p_SigGated_pad->SetY1NDC(0.60);
      p_PedGated_pad->SetY2NDC(0.60);
      p_PedGated_pad->SetY1NDC(0.45);
      p_unGated_pad->SetTextColor(1);
      p_SigGated_pad->SetTextColor(2);
      p_PedGated_pad->SetTextColor(4);
      gPad->Modified();
      c1->Print(Form("%s",outfile.Data()),"pdf");
 
      //Momentum -dp
      TH1F *dp_unGated = new TH1F("dp_unGated","dp ungated",100,-0.045,0.02);
      TH1F *dp_SigGated = new TH1F("dp_SigGated","dp with Signal gated",100,-0.045,0.02);
      TH1F *dp_PedGated = new TH1F("dp_PedGated","dp with pedestal gated",100,-0.045,0.02);
      T->Draw("L.gold.dp>>dp_unGated",cut_trig+cut_cluster+cut_wildtrack,"hist");
      T->Draw("L.gold.dp>>dp_SigGated",cut_trig+cut_cluster+cut_wildtrack+cut_sig,"hist sames");
      T->Draw("L.gold.dp>>dp_PedGated",cut_trig+cut_cluster+cut_wildtrack+cut_ped,"hist sames");
      dp_unGated->SetLineColor(1);
      dp_SigGated->SetLineColor(2);
      dp_PedGated->SetLineColor(4);

      latex.SetTextColor(1);
      latex.DrawLatex(0.20,0.85,"All events");
      latex.SetTextColor(2);
      latex.DrawLatex(0.20,0.80,"US accepted");
      latex.SetTextColor(4);
      latex.DrawLatex(0.20,0.75,"US missed");
      gPad->Update();
 
      TPaveStats *dp_unGated_pad = (TPaveStats*)dp_unGated->FindObject("stats");
      TPaveStats *dp_SigGated_pad = (TPaveStats*)dp_SigGated->FindObject("stats");
      TPaveStats *dp_PedGated_pad = (TPaveStats*)dp_PedGated->FindObject("stats");
      dp_unGated_pad->SetY2NDC(0.90);
      dp_unGated_pad->SetY1NDC(0.75);
      dp_SigGated_pad->SetY2NDC(0.75);
      dp_SigGated_pad->SetY1NDC(0.60);
      dp_PedGated_pad->SetY2NDC(0.60);
      dp_PedGated_pad->SetY1NDC(0.45);
      dp_unGated_pad->SetTextColor(1);
      dp_SigGated_pad->SetTextColor(2);
      dp_PedGated_pad->SetTextColor(4);
      gPad->Modified();
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //theta
      TH1F *theta = new TH1F("theta","L.tr.th",100,-0.2,0.1);
      T->Draw("L.tr.th>>theta");
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //phi
      TH1F *phi = new TH1F("phi","L.tr.ph",100,-0.1,0.1);
      T->Draw("L.tr.ph>>phi");
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //Target theta phi
      TH2F *tgt_ThetaPhi = new TH2F("tgt_ThetaPhi","Target theta vs phi",100,-0.05,0.1,100,-0.1,0.1);
      T->Draw("L.tr.tg_th:L.tr.tg_ph>>tgt_ThetaPhi",cut_trig+cut_cluster+cut_wildtrack,"colz");
      c1->Print(Form("%s",outfile.Data()),"pdf");

     //Qsq vs tg_y
     TH2F *Qsq_tgY = new TH2F("Qsq_tgY","Qsq vs Tgt Y",100,0.0002,0.0012,100,0.002,0.015);
      T->Draw("EK_L.Q2:L.tr.tg_y>>Qsq_tgY",cut_trig+cut_cluster+cut_wildtrack,"colz");
      c1->Print(Form("%s",outfile.Data()),"pdf");


      E1->Draw("IPM1H04B.XPOS:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      E1->Draw("IPM1H04B.YPOS:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      E1->Draw("IPM1H04D.XPOS:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      E1->Draw("IPM1H04D.YPOS:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      
      TSLeft->Draw("LeftT1_r:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      TSLeft->Draw("LeftT2_r:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      TSLeft->Draw("LeftT3_r:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      TSLeft->Draw("LeftT6_r:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");

    }else{
      // RHRS
      double upADCcut_approx = 503;
      double x_edge = -0.076;
      T->Add(Form("finalrootfiles/prexRHRS_%d_-1*.root",run_num));
      E1->Add(Form("finalrootfiles/prexRHRS_%d_-1*.root",run_num));
      TSRight->Add(Form("finalrootfiles/prexRHRS_%d_-1*.root",run_num));
//      T->Add(Form("prex_counting/prexRHRS_%d_200000.root",run_num));
      gStyle->SetStatX(0.90);
      gStyle->SetStatY(0.90);
      gStyle->SetStatW(0.25);
      gStyle->SetStatH(0.10);

      TH1F* huq = new TH1F("huq", "P.upQadcR;ADC CH;Events/CH", 170, 430, 600);
      T->Draw("P.upQadcR>>huq");
      TH1F* huqCpy = (TH1F*)huq->Clone("huq");
      huqCpy->GetXaxis()->SetRangeUser(upADCcut_approx-10,upADCcut_approx+10);
      double upADCcut = huqCpy->GetXaxis()->GetBinCenter(huqCpy->GetMinimumBin());
      //upADCcut = dummycut;
      cout<<"ADC cut: "<<upADCcut<<endl;
      TLine* line = new TLine(upADCcut,0.0,upADCcut,huq->GetMaximum());
      line->SetLineColor(kMagenta);
      line->Draw(); 
      c1->Print(Form("%s",outfile.Data()),"pdf");
   /*   
      TF1 *fped=new TF1("fped","gaus",465,500);
      fped->SetLineWidth(2);
      fped->SetLineColor(kRed);
      huq->Fit("fped","R");
      Double_t fpedpar[3];
      fped->GetParameters(&fpedpar[0]);
      char labelped[64];
      sprintf(labelped,"Pedeatal= %4.2f",fpedpar[1]);
      TPaveLabel *pt = new TPaveLabel(0.60,0.46,0.75,0.52,labelped,"NDC");
      pt->SetBorderSize(0);
      pt->SetTextColor(kRed);
      pt->SetTextSize(0.75);
      pt->SetFillColor(0);
      pt->Draw();
      char labelcut[64];
      sprintf(labelcut,"ADC_Cut = %3.0f",upADCcut);
      TPaveLabel *ptcut = new TPaveLabel(0.60,0.40,0.75,0.46,labelcut,"NDC");
      ptcut->SetBorderSize(0);
      ptcut->SetTextColor(kMagenta);
      ptcut->SetTextSize(0.75);
      ptcut->SetFillColor(0);
      ptcut->Draw();
      gPad->Update();
      double pedMean = fpedpar[1];
      cout<<"pedMean:"<<pedMean<<endl;*/
//      TCut cut_wadc = Form("((P.evtypebits&2)==2&&R.vdc.u1.nclust==1&&R.vdc.v1.nclust==1&&R.vdc.u2.nclust==1&&R.vdc.v2.nclust==1&&R.gold.th>-0.08&&R.gold.th<0.08&&R.gold.ph>-0.05&&R.gold.ph<0.05&&P.upQadcR>%f)",upADCcut);
      TCut cut_wadc = Form("((P.evtypebits&2)==2&&R.vdc.u1.nclust==1&&R.vdc.v1.nclust==1&&R.vdc.u2.nclust==1&&R.vdc.v2.nclust==1&&R.gold.th>-0.08&&R.gold.th<0.08&&R.gold.ph>-0.05&&R.gold.ph<0.05&&R.gold.dp>-0.02&&R.gold.dp<0&&P.upQadcR>%f)",upADCcut);
      TCut cut = Form("((P.evtypebits&2)==2&&R.vdc.u1.nclust==1&&R.vdc.v1.nclust==1&&R.vdc.u2.nclust==1&&R.vdc.v2.nclust==1&&R.gold.th>-0.08&&R.gold.th<0.08&&R.gold.ph>-0.05&&R.gold.ph<0.05&&R.gold.dp>-0.02&&R.gold.dp<0)");
      TCut cut_trig = Form("(P.evtypebits&2)==2");
      //TCut cut_trig = Form("2==2");
      TCut cut_cluster = Form("R.vdc.u1.nclust==1&&R.vdc.v1.nclust==1&&R.vdc.u2.nclust==1&&R.vdc.v2.nclust==1");
      //TCut cut_cluster = Form("1==1");
      TCut cut_wildtrack = Form("R.gold.th>-0.08 && R.gold.th<0.08 && R.gold.ph>-0.05 && R.gold.ph<0.05 && R.gold.dp>-0.04 && R.gold.dp<-0.002");
      //TCut cut_wildtrack = Form("1==1");
      TCut cut_sig = Form("P.upQadcR>%f",upADCcut);
      TCut cut_ped = Form("P.upQadcR<=%f",upADCcut);
      TCut x_cut = Form("(R.tr.x[0]+1.3*R.tr.th[0]) > %f",x_edge);
      //c1->cd(2);
      TH1F* QsqR = new TH1F("QsqR", Form("RHRS Qsq from T-tree using T1 trigger (run%d)",run_num), 100, 0.002, 0.015);

      T->Draw("EK_R.Q2>>QsqR", cut_trig+cut_cluster+cut_wildtrack+cut_sig);
      QsqR->SetXTitle("Qsq(GeV/c)^{2}");
      QsqR->SetLineColor(kBlue);
      QsqR->SetTitle("Qsq from analyzer");
      QsqR->Draw();
      c1->Print(Form("%s",outfile.Data()),"pdf");



      TH1F* hy1 = new TH1F("hy1", "Projected y on detector plane", 200, -0.04, 0.04);
      TH1F* hyup2 = new TH1F("hyup2", "Projected y on detector plane + adc>cut", 200, -0.04, 0.04);
      TH1F* hyup3 = new TH1F("hyup3", "Projected y on detector plane + adc<cut", 200, -0.04, 0.04);
      T->Project(hy1->GetName(), "R.tr.y[0]+1.3*R.tr.ph[0]", cut_trig+cut_cluster+cut_wildtrack+x_cut);
      T->Project(hyup2->GetName(), "R.tr.y[0]+1.3*R.tr.ph[0]", cut_trig+cut_cluster+cut_wildtrack+cut_sig+x_cut);
      T->Project(hyup3->GetName(), "R.tr.y[0]+1.3*R.tr.ph[0]", cut_trig+cut_cluster+cut_wildtrack+cut_ped+x_cut);
      hy1->SetXTitle("R.tr.y[0]+1.3*R.tr.ph[0] (m)");
      hy1->SetLineColor(1);
      hyup2->SetLineColor(2);
      hyup3->SetLineColor(4);
      hy1->Draw();
      hyup2->Draw("same");
      hyup3->Draw("same");

      TLatex latex;
      latex.SetNDC();
      latex.SetTextSize(0.05);
      latex.SetTextColor(1);
      latex.DrawLatex(0.20,0.85,"All events");
      latex.SetTextColor(2);
      latex.DrawLatex(0.20,0.80,"US accepted");
      latex.SetTextColor(4);
      latex.DrawLatex(0.20,0.75,"US missed");
      c1->Print(Form("%s",outfile.Data()),"pdf");


      TH1F* hx1 = new TH1F("hx1", "Projected x on detector plane", 300, -0.14, 0.03);
      TH1F* hxup2 = new TH1F("hxup2", "Projected x on detector plane + adc>cut", 300, -0.14, 0.03);
      TH1F* hxup3 = new TH1F("hxup3", "Projected x on detector plane + adc<cut", 300, -0.14, 0.03);

      T->Project(hx1->GetName(), "R.tr.x[0]+1.3*R.tr.th[0]", cut_trig+cut_cluster+cut_wildtrack);
      T->Project(hxup2->GetName(), "R.tr.x[0]+1.3*R.tr.th[0]", cut_trig+cut_cluster+cut_wildtrack+cut_sig);
      T->Project(hxup3->GetName(), "R.tr.x[0]+1.3*R.tr.th[0]", cut_trig+cut_cluster+cut_wildtrack+cut_ped);
      hx1->SetXTitle("R.tr.x[0]+1.3*R.tr.th[0] (m)");
      hx1->SetLineColor(1);
      hxup2->SetLineColor(2);
      hxup3->SetLineColor(4);
      hx1->Draw();
      hxup2->Draw("same");
      hxup3->Draw("same");
      TF1* fit_x = new TF1("fit_x","gaus",hxup2->GetBinCenter(hxup2->GetMaximumBin())-0.01,hxup2->GetBinCenter(hxup2->GetMaximumBin())+0.01);
      hxup2->Fit(fit_x,"R");

      latex.SetTextColor(1);
      latex.DrawLatex(0.20,0.85,"All events");
      latex.SetTextColor(2);
      latex.DrawLatex(0.20,0.80,"US accepted");
      latex.SetTextColor(4);
      latex.DrawLatex(0.20,0.75,"US missed");
      latex.SetTextColor(6);
      latex.DrawLatex(0.20,0.70,Form("Peak = %1.4f",fit_x->GetParameter(1)));
      latex.DrawLatex(0.20,0.65,Form("Edge = %1.4f",x_edge));
      latex.SetTextColor(49);
      /*latex.DrawLatex(0.20,0.60,Form("x_cut = %1.3f",xCut));     

      TLine* xCut_line = new TLine(xCut,0.0,xCut,hx1->GetMaximum()/2.0);
      xCut_line->SetLineColor(49);
      xCut_line->SetLineWidth(2);
      xCut_line->Draw();*/
      c1->Print(Form("%s",outfile.Data()),"pdf");
 

  //Central Angle
  double th0 = 4.73*TMath::Pi()/180;
  double cth0 = TMath::Cos(th0);
  double sth0 = TMath::Sin(th0);

  T->SetBranchAddress("P.upQadcR",&thisDet);
  T->SetBranchAddress("P.evtypebits",&thisEvt);
  T->SetBranchAddress("R.gold.y",&thistgty);
  T->SetBranchAddress("R.gold.th",&thisThtg);
  T->SetBranchAddress("R.gold.ph",&thisPhtg);
  T->SetBranchAddress("R.gold.p",&thisP);
  T->SetBranchAddress("R.gold.dp",&thisdp);
  T->SetBranchAddress("R.vdc.u1.nclust",&thisu1);
  T->SetBranchAddress("R.vdc.v1.nclust",&thisv1);
  T->SetBranchAddress("R.vdc.u2.nclust",&thisu2);
  T->SetBranchAddress("R.vdc.v2.nclust",&thisv2);
  
  // --- This segfaults when I loop it for some reason
  T->SetBranchAddress("R.tr.x",&thisXvdc[0]);
  T->SetBranchAddress("R.tr.y",&thisYvdc[0]);
  T->SetBranchAddress("R.tr.th",&thisThvdc[0]);
  T->SetBranchAddress("R.tr.ph",&thisPhvdc[0]);
 
  //Asymmetry table is in degrees so we need to convert from radians to degrees
  double radtodeg = 180/TMath::Pi();

  long n = T->GetEntries();

  for(long i = 0; i < n; i++){ 

    T->GetEntry(i);
  
    xq = thisXvdc[0]+1.3*thisThvdc[0]; yq = thisYvdc[0]+1.3*thisPhvdc[0];
    int thisevent = (int) thisEvt;
    //VDC cut and x cut
     if( thisu1==1 && thisv1==1 && thisu2==1 && thisv2==1 && thisThtg>-0.08 && thisThtg<0.08&& thisPhtg>-0.05 && thisPhtg<0.05 &&thisdp>-0.04 && thisdp<-0.002 && thisDet>upADCcut && ((thisevent&2)==2) ){
     //if( thisu1==1 && thisv1==1 && thisu2==1 && thisv2==1 && thisDet>upADCcut && ((thisevent&2)==2) ){
 
     thisCosAng = (cth0 + thisPhtg*sth0)/TMath::Sqrt(1+thisThtg*thisThtg+thisPhtg*thisPhtg);
     thisAngle = radtodeg*TMath::ACos(thisCosAng);
    
     thisQsq = 2*E*thisP*(1-thisCosAng);
     frac = 1./(1.+(E/(208*0.931494))*(1-thisCosAng));
     thisQsq1 = 2*E*E*frac*(1-thisCosAng);

      Qsq.push_back(thisQsq); 
      Qsq1.push_back(thisQsq1); 
      Angle.push_back(thisAngle);
      TgtY.push_back(thistgty);
      //Energy table in MeV so need to be careful here
      thisAsym = 1e6*Interpolate(thisP*1000,thisAngle,0,1);//A (ppm)
      thisAsymSt = 1e6*Interpolate(thisP*1000,thisAngle,1,1); // Stretched A (ppm); 

      ASYM.push_back(thisAsym);ASYM_ST.push_back(thisAsymSt);//Stretched A (ppm)
      Sens.push_back(fabs(thisAsym-thisAsymSt)/thisAsym);//Sensitivity - Why not?
  }
 }

 //Histograms 
 TH1F *Qsq_1 = new TH1F("Qsq_1","Q^{2} from dp, E and angle:  (GeV/c)^{2}",100,0.002,0.015);
 TH1F *Qsq_2 = new TH1F("Qsq_2","Q^{2} from incident energy and angle (GeV/c)^{2}",100,0.002,0.015);
 TH1F *Theta = new TH1F("Theta","#theta_{lab} (deg)",150,2,8);
 TH1F *Asym = new TH1F("Asym","Asymmetry (ppm)",150,0,1);
 TH1F *SAsym = new TH1F("SAsym","Stretched Asymmetry (ppm)",150,0,1);
 TH1F *sens = new TH1F("sens","Sensitivity",150,0,0.05);
 TH1F *htgY = new TH1F("htgY","Target Y ",200,0.0002,0.001);

/*
 Q2->SetTitle(Form("Q^{2} (GeV/c)^{2}, xCut = %1.3f m",xCut));
 Theta->SetTitle(Form("#theta_{lab} (deg), xCut = %1.3f m",xCut));
 Asym->SetTitle(Form("Asymmetry (ppm), xCut = %1.3f m",xCut));
 SAsym->SetTitle(Form("Stretched Asym. (ppm),  xCut = %1.3f m",xCut));
 sens->SetTitle(Form("Sensitivity, xCut = %1.3f m",xCut));
*/
 Qsq_1->GetXaxis()->SetTitle("Q^{2} (GeV/c)^{2} from E, dp and angle");
 Qsq_2->GetXaxis()->SetTitle("Q^{2} (GeV/c)^{2} from E and angle");
 Theta->GetXaxis()->SetTitle("#theta_{lab} (deg)");
 Asym->GetXaxis()->SetTitle("Asymmetry (ppm)");
 SAsym->GetXaxis()->SetTitle("Stretched Asym. (ppm)");
 htgY->GetXaxis()->SetTitle("Targt y (m)");

 for(int k = 0; k < Qsq.size(); k++){

 Qsq_1->Fill(Qsq[k]);
 Qsq_2->Fill(Qsq1[k]);
 Theta->Fill(Angle[k]);
 Asym->Fill(ASYM[k]);
 SAsym->Fill(ASYM_ST[k]);
 sens->Fill(Sens[k]);
 htgY->Fill(TgtY[k]);
 }
      long QsqEntry = Qsq_1->GetEntries();
      //std::cout<<"Run : "<<run_num<<" Entry "<<QsqEntry<<" Qsq "<<Qsq_1->GetMean(1)<<" Qsq error "<<Qsq_1->GetStdDev(1)<<" Qsq err/sqrt(entry) "<<Qsq_1->GetStdDev()/QsqEntry<<std::endl;
      //std::cout<<"Run : "<<run_num<<" Entry "<<QsqEntry<<" Asym "<<Asym->GetMean(1)<<" Asym error "<<Asym->GetStdDev(1)<<" Asym err/sqrt(entry) "<<Asym->GetStdDev()/sqrt(1.*QsqEntry)<<std::endl;
      /*std::cout<<"run_numi  Qsq_1->GetMean(1) Qsq_1->GetStdDev()/QsqEntry Asym->GetMean(1) Asym->GetStdDev()/sqrt(1.*QsqEntry) sens->GetMean(1) sens->GetStdDev()/sqrt(1.*QsqEntry) Theta->GetMean(1) Theta->GetStdDev()/sqrt(1.*QsqEntry) Tgt_Y err(tgtY)"<<std::endl;
      std::cout<<run_num<<" "<<Qsq_1->GetMean(1)<<" "<<Qsq_1->GetStdDev(1.)/sqrt(1.*QsqEntry)<<" "<<Asym->GetMean(1)<<" "<<Asym->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<sens->GetMean(1)<<" "<<sens->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<Theta->GetMean(1)<<" "<<Theta->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<htgY->GetMean(1)<<" "<<htgY->GetStdDev(1)/sqrt(1.*QsqEntry)<<std::endl;*/
      int binmax = htgY->GetMaximumBin();
      double Tgt_bin = htgY->GetXaxis()->GetBinCenter(binmax);
      std::cout<<"run_numi  Qsq_1->GetMean(1) Qsq_1->GetStdDev()/QsqEntry Asym->GetMean(1) Asym->GetStdDev()/sqrt(1.*QsqEntry) sens->GetMean(1) sens->GetStdDev()/sqrt(1.*QsqEntry) Theta->GetMean(1) Theta->GetStdDev()/sqrt(1.*QsqEntry) Tgt_Y err(tgtY) Tgt_bin(peak)"<<std::endl;
      std::cout<<run_num<<" "<<Qsq_1->GetMean(1)<<" "<<Qsq_1->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<Asym->GetMean(1)<<" "<<Asym->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<sens->GetMean(1)<<" "<<sens->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<Theta->GetMean(1)<<" "<<Theta->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<1000*htgY->GetMean(1)<<" "<<1000*htgY->GetStdDev(1)/sqrt(1.*QsqEntry)<<" "<<1000*Tgt_bin<<std::endl;
      //Qsq_1
      Qsq_1->Draw();
      Qsq_1->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");
      //Qsq_2
      Qsq_2->Draw();
      Qsq_2->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //Scattering angle
      Theta->Draw();
      Theta->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //Asymmetry
      Asym->Draw();
      Asym->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //sAsymmetry
      SAsym->Draw();
      SAsym->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //sens
      sens->Draw();
      sens->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //Tht Y
      htgY->Draw();
      htgY->Write("",TObject::kOverwrite);
      c1->Print(Form("%s",outfile.Data()),"pdf");


      //Momentum
      TH1F *p_unGated = new TH1F("p_unGated","momentum distributions ungated",200,0.91,0.97);
      TH1F *p_SigGated = new TH1F("p_SigGated","momentum distributions with Signal gated",200,0.91,0.97);
      TH1F *p_PedGated = new TH1F("p_PedGated","momentum distributions with pedestal gated",200,0.91,0.97);


      T->Draw("R.gold.p>>p_unGated",cut_trig+cut_cluster+cut_wildtrack,"hist");
      T->Draw("R.gold.p>>p_SigGated",cut_trig+cut_cluster+cut_wildtrack+cut_sig,"hist sames");
      T->Draw("R.gold.p>>p_PedGated",cut_trig+cut_cluster+cut_wildtrack+cut_ped,"hist sames");
      p_unGated->SetLineColor(1);
      p_SigGated->SetLineColor(2);
      p_PedGated->SetLineColor(4);

      latex.SetTextColor(1);
      latex.DrawLatex(0.20,0.85,"All events");
      latex.SetTextColor(2);
      latex.DrawLatex(0.20,0.80,"US accepted");
      latex.SetTextColor(4);
      latex.DrawLatex(0.20,0.75,"US missed");
      gPad->Update();

      TPaveStats *p_unGated_pad = (TPaveStats*)p_unGated->FindObject("stats");
      TPaveStats *p_SigGated_pad = (TPaveStats*)p_SigGated->FindObject("stats");
      TPaveStats *p_PedGated_pad = (TPaveStats*)p_PedGated->FindObject("stats");
      p_unGated_pad->SetY2NDC(0.90);
      p_unGated_pad->SetY1NDC(0.75);
      p_SigGated_pad->SetY2NDC(0.75);
      p_SigGated_pad->SetY1NDC(0.60);
      p_PedGated_pad->SetY2NDC(0.60);
      p_PedGated_pad->SetY1NDC(0.45);
      p_unGated_pad->SetTextColor(1);
      p_SigGated_pad->SetTextColor(2);
      p_PedGated_pad->SetTextColor(4);
      gPad->Modified();
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //Momentum -dp
      TH1F *dp_unGated = new TH1F("dp_unGated","dp ungated",200,-0.045,0.015);
      TH1F *dp_SigGated = new TH1F("dp_SigGated","dp with Signal gated",200,-0.045,0.015);
      TH1F *dp_PedGated = new TH1F("dp_PedGated","dp with pedestal gated",200,-0.045,0.015);
      T->Draw("R.gold.dp>>dp_unGated",cut_trig+cut_cluster+cut_wildtrack,"hist");
      T->Draw("R.gold.dp>>dp_SigGated",cut_trig+cut_cluster+cut_wildtrack+cut_sig,"hist sames");
      T->Draw("R.gold.dp>>dp_PedGated",cut_trig+cut_cluster+cut_wildtrack+cut_ped,"hist sames");
      dp_unGated->SetLineColor(1);
      dp_SigGated->SetLineColor(2);
      dp_PedGated->SetLineColor(4);

      latex.SetTextColor(1);
      latex.DrawLatex(0.20,0.85,"All events");
      latex.SetTextColor(2);
      latex.DrawLatex(0.20,0.80,"US accepted");
      latex.SetTextColor(4);
      latex.DrawLatex(0.20,0.75,"US missed");
      gPad->Update();
 
      TPaveStats *dp_unGated_pad = (TPaveStats*)dp_unGated->FindObject("stats");
      TPaveStats *dp_SigGated_pad = (TPaveStats*)dp_SigGated->FindObject("stats");
      TPaveStats *dp_PedGated_pad = (TPaveStats*)dp_PedGated->FindObject("stats");
      dp_unGated_pad->SetY2NDC(0.90);
      dp_unGated_pad->SetY1NDC(0.75);
      dp_SigGated_pad->SetY2NDC(0.75);
      dp_SigGated_pad->SetY1NDC(0.60);
      dp_PedGated_pad->SetY2NDC(0.60);
      dp_PedGated_pad->SetY1NDC(0.45);
      dp_unGated_pad->SetTextColor(1);
      dp_SigGated_pad->SetTextColor(2);
      dp_PedGated_pad->SetTextColor(4);
      gPad->Modified();
      c1->Print(Form("%s",outfile.Data()),"pdf");
 
      //theta
      TH1F *theta = new TH1F("theta","R.tr.th",100,-0.2,0.1);
      T->Draw("R.tr.th>>theta");
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //phi
      TH1F *phi = new TH1F("phi","R.tr.ph",100,-0.1,0.1);
      T->Draw("R.tr.ph>>phi");
      c1->Print(Form("%s",outfile.Data()),"pdf");

      //Target theta phi
      TH2F *tgt_ThetaPhi = new TH2F("tgt_ThetaPhi","Target theta vs phi",100,-0.05,0.1,100,-0.1,0.1);
      T->Draw("R.tr.tg_th:R.tr.tg_ph>>tgt_ThetaPhi",cut_trig+cut_cluster+cut_wildtrack,"colz");
      c1->Print(Form("%s",outfile.Data()),"pdf");

     //Qsq vs tg_y
     TH2F *Qsq_tgY = new TH2F("Qsq_tgY","Qsq vs Tgt Y",100,0.0002,0.0012,100,0.002,0.015);
      T->Draw("EK_R.Q2:R.tr.tg_y>>Qsq_tgY",cut_trig+cut_cluster+cut_wildtrack,"colz");
      c1->Print(Form("%s",outfile.Data()),"pdf");

      E1->Draw("IPM1H04B.XPOS:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      E1->Draw("IPM1H04B.YPOS:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      E1->Draw("IPM1H04D.XPOS:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      E1->Draw("IPM1H04D.YPOS:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");

      TSRight->Draw("RightT1_r:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      TSRight->Draw("RightT2_r:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      TSRight->Draw("RightT3_r:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");
      TSRight->Draw("RightT6_r:Entry$","","*");
      c1->Print(Form("%s",outfile.Data()),"pdf");

    }

  c1->Print(Form("%s]",outfile.Data()),"pdf");
}
