bool CollimatorL(double x, double y){


    double rmin = 0.175;
    double rmax = 0.315;
    double rmin_c = 0.147 + 0.352*rmin;
    double rmax_c = 0.147 + 0.352*rmax;
    double chamfer1_m = 1.88;
    double chamfer1_b = 0.0722;
    double chamfer1_s = 0.04;
    double chamfer2_m = 10.;
    double chamfer2_b = 0.05;
    double chamfer2_s = 0.03456;
    double xmax = 0.089;
    double ymin = -0.03975;
 
     


    if( fabs(x) > xmax) return false; 

    if( x < (-chamfer1_m*(y+chamfer1_s) - chamfer1_b) ) return false;
    if( x > (chamfer1_m*(y+chamfer1_s) + chamfer1_b) ) return false;

    if( x < (-chamfer2_m*(y+chamfer2_s) - chamfer2_b) ) return false;
    if( x > (chamfer2_m*(y+chamfer2_s) + chamfer2_b) ) return false;

    if( y < ymin ) return false; 
    

    if( sqrt(x*x + (y+rmin_c)*(y+rmin_c)) < rmin ) return false;
    if( sqrt(x*x + (y+rmax_c)*(y+rmax_c)) > rmax ) return false;
    



return true;

}
