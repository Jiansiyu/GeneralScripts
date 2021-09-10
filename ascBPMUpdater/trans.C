#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

inline std::string getFileName(const std::string & s){
	char sep = '/';
#ifdef _WIN32
        sep = '\\';
#endif
	size_t i = s.rfind(sep, s.length());
	if(i != std::string::npos){
	    return(s.substr(i+1, s.length()-i));
	}  
        return("");
}

int main(int argc, char* argv[])
{
	FILE *fp2;
	char newname[200];
	sprintf(newname,"%s_reform",getFileName(argv[1]).c_str());
	
	std::string inputFilename(argv[1]);

	fp2=fopen(newname,"w");
    	std::ifstream file(argv[1]);
    	std::string line;
    	
    	while(std::getline(file, line)){
        	unsigned int index=0;
        	double d[20];

        	std::stringstream linestream(line.c_str());

                              //kx  kth      ky     kphi    kurb_e  kbeamx  kbeamy   kbeam_z
        	linestream >> index >> d[0] >> d[1] >> d[2] >> d[3] >> d[4] >> d[5] >> d[6];
        	
			// macth the runs and fill the average bpm on target 
			if(true){
				//std::cout<<"Working on file"<< inputFilename.c_str()<<std::endl;

                d[5]=0.0;
                d[6]=0.0;
				if(inputFilename.find("1694")!=std::string::npos){
					//std::cout<<" run 21626 detected"<<std::endl;
					d[5]=-0.7409;
					d[6]=0.4932;
				}

                if(inputFilename.find("1695")!=std::string::npos){
                    //std::cout<<" run 21626 detected"<<std::endl;
                    d[5]=-0.7393;
                    d[6]=0.5048;
                }

                if(inputFilename.find("1696")!=std::string::npos){
                    //std::cout<<" run 21626 detected"<<std::endl;
                    d[5]=-0.7453;
                    d[6]=0.4971;
                }

                if(inputFilename.find("20825")!=std::string::npos){
                    //std::cout<<" run 21626 detected"<<std::endl;
                    d[5]=-0.7409;
                    d[6]=0.4931;
                }

                if(inputFilename.find("20826")!=std::string::npos){
                    //std::cout<<" run 21626 detected"<<std::endl;
                    d[5]=-0.7522;
                    d[6]=0.5059;
                }
                if(inputFilename.find("20827")!=std::string::npos){
                    //std::cout<<" run 21626 detected"<<std::endl;
                    d[5]=-0.7571;
                    d[6]=0.5107;
                }

                if(inputFilename.find("2550")!=std::string::npos){
                    d[5]=-0.1086;
                    d[6]=-0.2024;
                }

                if(inputFilename.find("2556")!=std::string::npos){
                    d[5]=-0.1130;
                    d[6]=-0.2025;
                }

                if(inputFilename.find("2565")!=std::string::npos){
                    d[5]=-0.0997;
                    d[6]=-0.1999;
                }

                if(inputFilename.find("2566")!=std::string::npos){
                    d[5]=-0.0979;
                    d[6]=-0.2023;
                }

                if(inputFilename.find("2239")!=std::string::npos){
                    d[5]=3.7944;
                    d[6]=-0.5007;
                }

                if(inputFilename.find("2240")!=std::string::npos){
                    d[5]=-0.2004;
                    d[6]=-0.4996;
                }

                if(inputFilename.find("2241")!=std::string::npos){
                    d[5]=-3.2000;
                    d[6]=-0.4986;
                }

                if(inputFilename.find("2244")!=std::string::npos){
                    d[5]=1.7921;
                    d[6]=-3.4975;
                }

                if(inputFilename.find("2245")!=std::string::npos){
                    d[5]=-2.2083;
                    d[6]=-3.4970;
                }
                if(inputFilename.find("2246")!=std::string::npos){
                    d[5]=-0.1952;
                    d[6]=-3.5004;
                }

                if(inputFilename.find("2247")!=std::string::npos){
                    d[5]=-0.1942;
                    d[6]=-0.4990;
                }

                if(inputFilename.find("2256")!=std::string::npos){
                    d[5]=-0.1973;
                    d[6]=-0.4995;
                }

                if(inputFilename.find("2257")!=std::string::npos){
                    d[5]=-0.2005;
                    d[6]=-0.4984;
                }

                if(inputFilename.find("21380")!=std::string::npos){
                    d[5]=-0.1973;
                    d[6]=-0.4995;
                }

                if(inputFilename.find("21381")!=std::string::npos){
                    d[5]=-0.1941;
                    d[6]=-0.4988;
                }

                if(inputFilename.find("21364")!=std::string::npos){
                    d[5]=-0.2005;
                    d[6]=-0.4994;
                }

                if(inputFilename.find("21363")!=std::string::npos){
                    d[5]= 3.7988;
                    d[6]=-0.4995;
                }

                if(inputFilename.find("21364")!=std::string::npos){
                    d[5]=-0.2005;
                    d[6]=-0.4994;
                }

                if(inputFilename.find("21365")!=std::string::npos){
                    d[5]=-3.2000;
                    d[6]=-0.4985;
                }

                if(inputFilename.find("21366")!=std::string::npos){
                    d[5]= 2.7970;
                    d[6]=-0.4979;
                }

                if(inputFilename.find("21367")!=std::string::npos){
                    d[5]= 1.7864;
                    d[6]=-3.5015;
                }

                if(inputFilename.find("21368")!=std::string::npos){
                    d[5]= 1.7921;
                    d[6]=-3.4975;
                }
                if(inputFilename.find("21370")!=std::string::npos){
                    d[5]=-0.1967;
                    d[6]=-3.5015;
                }
                // add from the the sister run 2241
                if(inputFilename.find("21369")!=std::string::npos){
                    d[5]=-2.2083;
                    d[6]=-3.4970;
                }

                // add the result for CRex
                if(inputFilename.find("2689")!=std::string::npos){
                    d[5]= -0.6820;
                    d[6]= -0.5063;
                }
                if(inputFilename.find("2684")!=std::string::npos){
                    d[5]= -0.6984;
                    d[6]= -0.5096;
                }
                if(inputFilename.find("2718")!=std::string::npos){
                    d[5]= -0.6612;
                    d[6]= -0.4941;
                }
            }

        	fprintf(fp2,"%d\t%1.8e\t%1.8e\t%1.8e\t%1.8e\t%1.8e\t%1.8e\t%1.8e\t%1.8e\n",index,d[0],d[1],d[2],d[3],d[4],d[5]/1000.0,d[6]/1000.0,0.0);
    	}

    	fclose(fp2);

    	std::cout << '\a' << std::flush;
    	std::cout<<"reformed file saved as :"<<newname<<std::endl;
    	
    	return 0;
}
