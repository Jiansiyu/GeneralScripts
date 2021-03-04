bool DownPlane(double xd1,double yd1, double xd2, double yd2, double xd3, double yd3, double xd4, double yd4, double xd5, double yd5, double xd6, double yd6, double xd7, double yd7, double xd8, double yd8, double xd9, double yd9, int hrs){

//hrs = 0 is RHRS, 1 is LHRS

int sign;

if(hrs == 0 ) { sign = -1; } else {sign = 1; }

double xmin[9] = { 0.082, 0.0862, 0.0989, 0.1084, 0.1195, 0.1236, 0.1326, 0.1958, 0.2149 };
double xmax[9] = { 0.1256, 0.1318, 0.1505, 0.1643, 0.1798, 0.1853, 0.1975, 0.2777, 0.301 };
double ymin[9] = {-0.0447, -0.0469, -0.053, -0.0571, -0.0612, -0.0624, -0.065, -0.0786, -0.0818 };
double ymax[9] = { 0.0445, 0.0466, 0.0529, 0.057, 0.0611, 0.0624, 0.065, 0.0785, 0.0818 };
 

if( sign*xd1 < xmin[0] ) return false; if( sign*xd1 > xmax[0] ) return false; if( yd1 < ymin[0] ) return false; if( yd1 > ymax[0] ) return false;
if( sign*xd2 < xmin[1] ) return false; if( sign*xd2 > xmax[1] ) return false; if( yd2 < ymin[1] ) return false; if( yd2 > ymax[1] ) return false;
if( sign*xd3 < xmin[2] ) return false; if( sign*xd3 > xmax[2] ) return false; if( yd3 < ymin[2] ) return false; if( yd3 > ymax[2] ) return false;
if( sign*xd4 < xmin[3] ) return false; if( sign*xd4 > xmax[3] ) return false; if( yd4 < ymin[3] ) return false; if( yd4 > ymax[3] ) return false;
if( sign*xd5 < xmin[4] ) return false; if( sign*xd5 > xmax[4] ) return false; if( yd5 < ymin[4] ) return false; if( yd5 > ymax[4] ) return false;
if( sign*xd6 < xmin[5] ) return false; if( sign*xd6 > xmax[5] ) return false; if( yd6 < ymin[5] ) return false; if( yd6 > ymax[5] ) return false;
if( sign*xd7 < xmin[6] ) return false; if( sign*xd7 > xmax[6] ) return false; if( yd7 < ymin[6] ) return false; if( yd7 > ymax[6] ) return false;
if( sign*xd8 < xmin[7] ) return false; if( sign*xd8 > xmax[7] ) return false; if( yd8 < ymin[7] ) return false; if( yd8 > ymax[7] ) return false;
if( sign*xd9 < xmin[8] ) return false; if( sign*xd9 > xmax[8] ) return false; if( yd9 < ymin[8] ) return false; if( yd9 > ymax[8] ) return false;








return true;

}
