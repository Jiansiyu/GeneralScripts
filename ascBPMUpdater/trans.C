#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "vector"
#include "map"
#include "sys/stat.h"

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

inline bool isFileExist(const std::string& name){
    struct  stat buff;
    return (stat (name.c_str(), &buff)==0);
}

std::map<char,double> getBPM(unsigned int runID, std::string csvfname="/home/newdriver/Storage/HomeDir/Learning/GeneralScripts/ascBPMUpdater/bpm_on_targ.csv"){

    if (!isFileExist(csvfname.c_str())){
        std::cout<<"\033[1;33m [Warning]\033[0m Missing csv file::"<<csvfname.c_str()<<std::endl;
        exit(-1);
    }

    std::map<unsigned int, std::map<char,double>> bpmList;

    std::ifstream csvStream(csvfname.c_str());
    std::string line;

    while (std::getline(csvStream,line)){
        std::istringstream s(line);
        std::string field;

        getline(s,field,',');
        std::string title=field;
        if(title.find("run") != std::string::npos) continue;

        unsigned int runs=std::stoi(field.c_str());

        getline(s,field,',');
        double bpmx=std::stof(field.c_str());

        getline(s,field,',');
        double bpmy=std::stof(field.c_str());

        std::map<char,double> vec;
        vec['x'] = bpmx;
        vec['y'] = bpmy;

        bpmList[runs]=vec;
    }
    if(bpmList.find(runID) != bpmList.end()){
        return bpmList[runID];
    } else{
        std::cout<<"\033[1;33m [Warning]\033[0m Missing runID file::"<<csvfname.c_str()<<std::endl;
        exit(-1);
    }

}

unsigned int getRunID(std::string filename){
    auto s= filename;
    size_t begin = 0, end = 0;

    std::vector<unsigned int> buffer;
    while(end != std::string::npos)
    {
        begin = s.find_first_of("0123456789", end);
        if(begin != std::string::npos) // we found one
        {
            end = s.find_first_not_of("0123456789", begin);
            std::string num = s.substr(begin, end - begin);
            int number = atoi(num.c_str());
            buffer.push_back(number);
        }
    }

    for (auto value : buffer){
        if ((value > 1500) && (value < 4000)){
            return value;
        }

        if ((value > 20000) &&(value < 30000)){
            return value;
        }
    }
    std::cout<<"\033[1;33m [Warning]\033[0m Can not find a valid runID"<<std::endl;
    exit(-1);
}


int main(int argc, char* argv[])
{
	FILE *fp2;
	char newname[200];
	sprintf(newname,"%s_reform",getFileName(argv[1]).c_str());
	
	std::string inputFilename(argv[1]);

    unsigned int runID = getRunID(inputFilename);

	fp2=fopen(newname,"w");
    	std::ifstream file(argv[1]);
    	std::string line;

        auto bpmPos = getBPM(runID);
        std::cout<<"Working on file : "<< inputFilename.c_str()<<std::endl <<
             "\t runID "<< runID<<"   bpmPos : (" <<bpmPos['x'] <<", " << bpmPos['y'] <<")"<<std::endl;

    while(std::getline(file, line)){
        	unsigned int index=0;
        	double d[20];

        	std::stringstream linestream(line.c_str());

                              //kx  kth      ky     kphi    kurb_e  kbeamx  kbeamy   kbeam_z
        	linestream >> index >> d[0] >> d[1] >> d[2] >> d[3] >> d[4] >> d[5] >> d[6];
        	
			// macth the runs and fill the average bpm on target


			if(true){
				//std::cout<<"Working on file"<< inputFilename.c_str()<<std::endl;

                d[5]=bpmPos['x'];
                d[6]=bpmPos['y'];

/*				if(inputFilename.find("1694")!=std::string::npos){
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


                if(inputFilename.find("2683")!=std::string::npos){
                    d[5]= 1.2946;
                    d[6]= -0.5042;
                }
                if(inputFilename.find("2687")!=std::string::npos){
                    d[5]= 0.3023;
                    d[6]= -0.4954;
                }*/
            }
        	fprintf(fp2,"%d\t%1.8e\t%1.8e\t%1.8e\t%1.8e\t%1.8e\t%1.8e\t%1.8e\t%1.8e\n",index,d[0],d[1],d[2],d[3],d[4],d[5]/1000.0,d[6]/1000.0,0.0);
    	}

    	fclose(fp2);

    	std::cout << '\a' << std::flush;
    	std::cout<<"reformed file saved as :"<<newname<<std::endl;
    	
    	return 0;
}


//int main (int argc, char* argv[]){
//    unsigned int runID = std::stoi(argv[1]);
//
//    auto bpm = getBPM(runID);
//
//    std::cout<<"Beam Position: x. "<< bpm['x'] <<"   y. "<<bpm['y']<<std::endl;
//
//}
