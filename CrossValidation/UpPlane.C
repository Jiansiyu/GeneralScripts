bool UpPlane(double xup1, double yup1, double xup2, double yup2, int hrs ){

//hrs = 0 RHRS

int sign;

if(hrs == 0) { sign = -1; } else { sign = 1; }

//Plane 1

double x1 = 0.0448;
double x2 = 0.0707;
double y1 = -0.0258;
double y2 = 0.0255;

//Plane 2
double x3 = 0.0482;
double x4 = 0.0757;
double y3  = -0.0275;
double y4 = 0.0272;


if( xup1 < x1 ) return false; if( xup1 > x2 ) return false;
if( yup1 < y1 ) return false; if( yup1 > y2 ) return false;

if( xup2 < x3 ) return false; if( xup2 > x4 ) return false;
if( yup2 < y3 ) return false; if( yup2 > y4 ) return false;


return true; 



}
