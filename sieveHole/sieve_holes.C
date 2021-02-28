{

Int_t debug=1;

Float_t Zdist=995.6; // mm to target
Float_t spaceX,spaceY;

Int_t irow, icol, idx;


TH2F *hsv = new TH2F("hsv","tg_th vs tg_ph at Sieve",100,-0.032,0.032,100,-0.067,0.067);

//hamc->Draw("ysep:xsep>>hsv","inaccept==1");

hsv->SetMarkerColor(4);

//xysepiaz->Draw();

//hsv->Draw("");

TLine *tl1 = new TLine();

tl1->DrawLine(-0.06,0,0.06,0);
tl1->DrawLine(0.0,-0.1,0,0.1);

Float_t xlo,xhi,ylo,yhi;

// densely-spaced holes

for (icol = 1; icol <= 8; icol++) {

  if(icol==1) { 
     xlo = 0; 
     xhi = 0; 
  }
  if(icol>1 && icol<=5) { // 2, 3, 4, 5
     xlo = (6.12 + (1.0*icol - 2)*4.80)/Zdist;  
     xhi = xlo; 
  }
  if(icol>5) {   // 6, 7, 8
     xlo = (1.0*icol - 5)* -6.12/Zdist;  
     xhi = xlo; 
  }

  ylo = -6.0*6.65/Zdist;
  yhi =  6.0*6.65/Zdist;


  tl1->SetLineColor(2);
  tl1->SetLineWidth(3);
  tl1->DrawLine(xlo,ylo,xhi,yhi);   

}

for (irow = 0; irow <= 6; irow++) {

  if (irow == 0) {
    ylo = 0;
    yhi = 0;
  }
  if (irow>0 && irow<=3) {
    ylo = (-13.30*irow)/Zdist;
    yhi = ylo;
  }
  if (irow>3) {
    ylo = (13.30*(irow-3))/Zdist;
    yhi = ylo;
  }

  xlo = (-3*6.12)/Zdist;
  xhi = (6.12+3*4.80)/Zdist;

  tl1->SetLineColor(2);
  tl1->SetLineWidth(3);
  tl1->DrawLine(xlo,ylo,xhi,yhi);   


}


// wider-spaced holes

for (icol = 1; icol <= 5; icol++) {

  if(icol==1) { 
     xlo = -6.12/(2*Zdist); 
     xhi = xlo; 
  }
  if(icol==2) {
     xlo = (11.58 - (6.12/2))/Zdist;
     xhi = xlo; 
  }
  if(icol==3) {
     xlo = (9.6 + 11.58 - (6.12/2))/Zdist;
     xhi = xlo; 
  }
  if(icol==4) {
     xlo = (-12.24 - 6.12/2)/Zdist;
     xhi = xlo; 
  }
  if(icol==5) {
     xlo = (-6.12 - 12.24 - 6.12/2)/Zdist;
     xhi = xlo; 
  }

  ylo = (-4.0*6.65 - 7.35)/Zdist;
  yhi =  5.0*6.65/Zdist;

  tl1->SetLineColor(4);
  tl1->SetLineWidth(3);
  tl1->DrawLine(xlo,ylo,xhi,yhi);   

}

for (irow = 1; irow <= 6; irow++) {

  if (irow == 1) {
    ylo = (-4.0*6.65 - 7.35)/Zdist;
    yhi = ylo;
  }
  if (irow == 2) {
    ylo = (-3.0*6.65)/Zdist;
    yhi = ylo;
  }
  if (irow == 3) {
    ylo = (-6.65)/Zdist;
    yhi = ylo;
  }
  if (irow == 4) {
    ylo = (6.65)/Zdist;
    yhi = ylo;
  }
  if (irow == 5) {
    ylo = (3.0*6.65)/Zdist;
    yhi = ylo;
  }
  if (irow == 6) {
    ylo = (5.0*6.65)/Zdist;
    yhi = ylo;
  }

  xlo = ((-6.12/2)-12.24-6.12)/Zdist;
  xhi = ((-6.12/2)+11.58+9.6)/Zdist;

  tl1->SetLineColor(4);
  tl1->SetLineWidth(3);
  tl1->DrawLine(xlo,ylo,xhi,yhi);   


}



}

