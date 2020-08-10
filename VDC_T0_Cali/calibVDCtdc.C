/*
 * calibVDCtdc.C
 *
 *      Created on: May 20, 2020
 *      Author: Siyu Jian
 *
 *
 *      Code adapted from the GMP experiment
 *      initial code
 *      https://github.com/JeffersonLab/HallAC-Workshop2017/blob/a141b4f5ebe8bb9bc8945d5a06a891589425275d/vdccalib/calibVDCtdc.C
 */
#include <TROOT.h>
#include <TChain.h>
#include <TH1I.h>
#include <TCanvas.h>
#include <TSpectrum.h>
#include <TLine.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string.h>
#include <string>
#include <TStyle.h>
#include <fstream>

static const Int_t MAX_HIT = 1000;   //?
static const Int_t NPLANE = 4;
static const Int_t MAX_GRP = 23;     // what is GRP
static const Int_t NWIRES = 368;     // number of wires
static const Int_t NPERROW = 8;      //?

using namespace std;

// need to perform on each arm individually
void multirun_calibVDCtdc( const char *arm, const vector<string>& rootfiles );

void calibVDCtdc(const char *arm = "R", const char* rootfilename = "Afile.root"){
	vector<string> rootfiles;
	rootfiles.push_back(string(rootfilename));
	multirun_calibVDCtdc( arm, rootfiles );
}


// Expert interface
void multirun_calibVDCtdc( const char *arm, const vector<string>& rootfiles ) {
	  gROOT->SetStyle("Plain");
	  gStyle->SetOptStat(0);
	  gStyle->SetPalette(1);
	  TChain* T = new TChain("T");
	  for( vector<string>::const_iterator it = rootfiles.begin(); it != rootfiles.end(); ++it ) {
	    T->Add( (*it).c_str() );
	  }
	  T->SetBranchStatus("*", kFALSE);
	  const char plane[NPLANE][8] = {"u1", "u2", "v1", "v2"};

	  Double_t nhit[NPLANE];
	  Double_t wire[NPLANE][MAX_HIT], rawtdc[NPLANE][MAX_HIT];
	  TH1I* h_tdc[NPLANE][MAX_GRP];

	  // Set up branches & histograms
	  for( Int_t i = 0; i < NPLANE; i++ ){
	    T->SetBranchStatus(Form("%s.vdc.%s.nhit", arm, plane[i]), kTRUE);
	    T->SetBranchAddress(Form("%s.vdc.%s.nhit", arm, plane[i]), &nhit[i]);

	    T->SetBranchStatus(Form("%s.vdc.%s.wire", arm, plane[i]), kTRUE);
	    T->SetBranchAddress(Form("%s.vdc.%s.wire", arm, plane[i]), &wire[i]);

	    T->SetBranchStatus(Form("%s.vdc.%s.rawtime", arm, plane[i]), kTRUE);
	    T->SetBranchAddress(Form("%s.vdc.%s.rawtime", arm, plane[i]), &rawtdc[i]);

	    for( int grp = 0; grp < MAX_GRP; grp++ ) {
	      h_tdc[i][grp] = new TH1I(Form("h_tdc_%s_%d",plane[i],grp+1), Form("%s TDC grp %d", plane[i], grp+1), 4096, 0, 4097);
	    }
	  }

	  // Read TDC values of wires
	  for( Int_t i = 0; i < T->GetEntries(); i++ ){
	    T->GetEntry(i);
	    if( (i%5000)==0 ){ printf("Entry %d\n", i ); }

	    for( Int_t j = 0; j < NPLANE; j++ ){
	      for( Int_t hit = 0; hit < nhit[j] && hit < MAX_HIT; hit++ ){
		Int_t iwire = Int_t( wire[j][hit] + 0.5 );
		Int_t grp   = iwire/(NWIRES/MAX_GRP);
		Int_t itdc  = Int_t( rawtdc[j][hit] + 0.5 );
		h_tdc[j][grp]->Fill( itdc );
	      }
	    }
	  }

	  cout << "Start calibration" << endl;

	  TSpectrum s;
	  TCanvas* c[NPLANE];
	  Double_t sigma_est = 30, thresh_init = 0.9;
	  bool any_failed = false;
	  for( Int_t i = 0; i < NPLANE; i++ ) {
	    Int_t nfail = 0;
	    c[i] = new TCanvas(Form("c_%s",plane[i]), Form("%s raw TDCs",plane[i]), 1280, 960);
	    c[i]->Divide(6,4);

	    Double_t tdcoff[MAX_GRP];
	    memset( tdcoff, 0, sizeof(tdcoff) );

	    for( int grp=0; grp<MAX_GRP; grp++ ) {
	      c[i]->cd(grp+1);
	      cout << "plane = " << plane[i] << " grp = " << grp+1;
	      TH1I* h = h_tdc[i][grp];

	      h->Draw();

	      // Search for highest peak
	      Double_t thr = thresh_init;
	      while( s.Search( h, sigma_est, "", thr ) == 0 && thr > 0.1 ) { thr -= 0.1; }
	      if( s.GetNPeaks() == 0 ) {
		cout << " ==== Warning: no peaks " << endl;
		continue;
	      }
	      //      cout << " thr = " << thr;
	      Double_t* xpk = s.GetPositionX();
	      Double_t* ypk = s.GetPositionY();
	      Double_t xmax = h->GetBinCenter( h->GetMaximumBin() );
	      Double_t xpeak = 0;
	      Int_t ipk = -1;
	      for( int k = 0; k < s.GetNPeaks(); k++ ) {
		// Require peaks within 1 sigma of the histogram maximum
		if( TMath::Abs( xpk[k]-xmax ) < sigma_est ) {
		  if( ipk < 0 ) {
		    ipk = k;
		    xpeak = xpk[k];
		  } else {
		    cout << " ==== Warning: multiple peak candidates. Poor statistics?" << endl;
		    any_failed = true;
		    // choose the one with the largest x
		    if( xpk[k] > xpeak ) {
		      ipk = k;
		      xpeak = xpk[k];
		    }
		  }
		}
	      }
	      if( ipk == -1 ) {
		cout << " ==== Error: no good peak" << endl;
		++nfail;
		any_failed = true;
		continue;
	      }

	      // Fit Gaussian to peak. The offset is then roughly 2 fitted sigma to the right of the fitted peak position
	      /*
	      h->Fit("gaus","S","", ypk[ipk]-1.5*sigma_est, ypk[ipk]+2*sigma_est);
	      const TF1* gaus = h->GetFunction("gaus");
	      if( !gaus ) {
		cout << "Error: no fit result in plane " << plane[i] << " group " << grp << endl;
		continue;
	      }
	      Double_t mean  = gaus->GetParameter(1);
	      Double_t sigma = gaus->GetParameter(2);
	      if( TMath::
	      */
	      // Quick and dirty: t0 is 1.4 sigma to the right of the peak center, period
	      tdcoff[grp] = xpk[ipk] + 1.4*sigma_est;
	      cout << " tdcoff = " << setprecision(6) << tdcoff[grp] << endl;

	      // Show the offsets in the histograms
	      c[i]->cd(grp+1);
	      TLine* line = new TLine( tdcoff[grp], 0, tdcoff[grp], 0.2*ypk[ipk] );
	      line->SetLineColor(2); // red
	      line->SetLineWidth(3);
	      line->Draw();
	    }

	    // Deal with failures -- set them to same offset as an available adjacent bin
	    if( nfail >= MAX_GRP ) { //OOPS
	      cout << " ===== TOTAL FAILURE ====   No data????" << endl;
	      continue;
	    }
	    while( nfail > 0 ) {
	      for( int grp = 0; grp < MAX_GRP; grp++ ) {
		if( tdcoff[grp] == 0.0 ) {
		  if( grp > 0 && tdcoff[grp-1] != 0.0 ) {
		    tdcoff[grp] = tdcoff[grp-1];
		    --nfail;
		  }
		  else if( grp < MAX_GRP-1 && tdcoff[grp+1] != 0.0 ) {
		    tdcoff[grp] = tdcoff[grp+1];
		    --nfail;
		  } else
		    continue;

		  // Draw the estimated offsets in a different color
		  c[i]->cd(grp+1);
		  TLine* line = new TLine( tdcoff[grp], 0, tdcoff[grp], 0.2*h_tdc[i][grp]->GetMaximum() );
		  line->SetLineColor(4);
		  line->SetLineWidth(3);
		  line->Draw();
		}
	      }
	    }

	   // Write data out in VDC database format

	    ofstream outp;
	    outp.open( Form("tdcoff_%s_%s.txt", arm, plane[i]), ios_base::out | ios_base::trunc );
	    if( !outp ) {
	      cout << "ERROR: canot open output file tdcoff_" << arm << "_" << plane[i] << ".txt" << endl;
	      any_failed = true;
	      continue;
	    }
	    for( Int_t wire = 0; wire < NWIRES; wire += NPERROW ) {
	      for( int k=0; k<NPERROW; k++ ) {
		Int_t iwire = wire+k;
		Int_t grp   = iwire/(NWIRES/MAX_GRP);
		// analyzer 1.5 and earlier format
		//if( k > 0 ) outp << " ";
		// outp << setw(4) << iwire+1 << " " << setw(6) << setprecision(5) << tdcoff[grp];
		if( k == 0 ) outp << " ";
		outp << setw(8) << setprecision(5) << tdcoff[grp];
	      }
	      outp << endl;
	      if( !outp ) {
		cout << "ERROR writing output file tdcoff_" << arm << "_" << plane[i] << ".txt" << endl;
		any_failed = true;
		break;
	      }
	    }
	    outp.close();

	  }// end for i over planes

	  if( any_failed ) {
	    cout << "WARNING: some groups failed. Bad channels or low statistics? Offsets in blue are approximate." << endl;
	  }
}
