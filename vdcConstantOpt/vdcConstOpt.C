/*
 * CERN Root Optimizer Used for get the VDC constant
 *
 * author: Siyu Jian
 *      sj9va@virginia.edu
 */

#include "vector"
#include "string"
#include "TString.h"
#include "TChain.h"
#include "iostream"
#include "TMinuit.h"
#include "TSystem.h"
#include "TMinuit.h"
#include "TVirtualFitter.h"
#include "THaString.h"
#include "TMath.h"
#include "TSQLResult.h"
#include "TFile.h"
#include "TCutG.h"
#include "TROOT.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "exception"
using namespace std;
using THaString::Split;

struct vdcTrack;
class THaMatrixElement;

std::vector<THaMatrixElement> fTMatrixElems;
std::vector<THaMatrixElement> fDMatrixElems;
std::vector<THaMatrixElement> fPMatrixElems;
std::vector<THaMatrixElement> fPTAMatrixElems; // involves abs(theta_fp)
std::vector<THaMatrixElement> fYMatrixElems;
std::vector<THaMatrixElement> fYTAMatrixElems; // involves abs(theta_fp)
std::vector<THaMatrixElement> fFPMatrixElems; // matrix elements used in
std::vector<THaMatrixElement> fRMatrixElems; // Path-length corrections (meters)

enum EFPMatrixElemTag { T000 = 0, Y000, P000 };

class THaMatrixElement {
public:

    THaMatrixElement() : iszero(true), pw(3), order(0), v(0), poly(7), OptOrder(0)
    {
    }
    bool match(const THaMatrixElement& rhs) const;

    bool iszero; // whether the element is zero
    std::vector<int> pw; // exponents of matrix element
    //   e.g. D100 = { 1, 0, 0 }
    int order;
    double v; // its computed value
    std::vector<double> poly; // the associated polynomial

    void SkimPoly(); //reduce order to highest non-zero poly

    UInt_t OptOrder; //order optimize to

    void print() const{
        std::cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>"<<std::endl;
        std::cout<<"IsZero : "<< iszero<<std::endl;
        std::cout<<"pw::"<<std::endl;
        int counter=1;
        for (auto item : pw){
            std::cout<<"    "<<(counter++)<<"/"<<pw.size()<<" :: "<<item<<std::endl;
        }
        std::cout<<"Order : "<<order<<std::endl;
        std::cout<<"v     : "<<v<<std::endl;
        std::cout<<"poly::"<<std::endl;
        counter=1;
        for (auto item : poly){
            std::cout<<"    "<<(counter++)<<"/"<<poly.size()<<" :: "<<item<<std::endl;
        }
        std::cout<<"OptOrders : "<<(UInt_t)OptOrder<<std::endl;
        std::cout<<std::endl;
    }
};

bool THaMatrixElement::match(const THaMatrixElement& rhs) const
{
    // Compare coefficients of this matrix element to another

    if (pw.size() != rhs.pw.size())
        return false;
    for (vector<int>::size_type i = 0; i < pw.size(); i++) {
        if (pw[i] != rhs.pw[i])
            return false;
    }
    return true;
}

void THaMatrixElement::SkimPoly()
{
    // reduce order to highest non-zero poly

    if (iszero) return;

    while (!poly[order - 1] && order > 0) {
        poly.pop_back();
        order = order - 1;
    }

    if (order == 0) iszero = kTRUE;
}

std::vector<vdcTrack> vdcTrackData;


struct vdcTrack{
public:
    vdcTrack(){};
    ~vdcTrack(){};

private:
    // Detector Coordination system
    double d_x;
    double d_th;
    double d_y;
    double d_ph;

    // transport coordination system
    double th_tr=0;
    double ph_tr=0;
    double y_tr=0;
    double x_tr=0;

    // calculated value with the matrix
    double th_f=0;
    double ph_f=0;
    double y_f=0;
    double x_f=0;

    // theoretical Value
    double th_f_theor=0.0;
    double ph_f_theor=0.0;
    double y_f_theor=0.0;
    double x_f_theor=0.0;

public:

    void WriteDCSVar(double x, double theta, double y, double phi);
    void WriteTRCSVar( double x_tr, double th_tr, double y_tr,double ph_tr);
    void WriteFocalVar(double f_x, double f_th,double f_y, double f_ph);
//    double getResidualSqr(TString item);

    double_t  getDX(){return  d_x;};
    double_t  getDTheta(){return  d_th;};
    double_t  getDY(){return d_y;};
    double_t  getDPhi(){return d_ph;};

    double_t  getTrX(){return  x_tr;};
    double_t  getTrTheta(){return  th_tr;};
    double_t  getTrY(){return y_tr;};
    double_t  getTrPhi(){return ph_tr;};

    double_t getFocalX(){return x_f;};
    double_t getFocalY(){return y_f;};
    double_t getFocalTh(){return th_f;};
    double_t getFocalPh(){return ph_f;};

    double_t getFocalResidualTheta(){
        return (th_f-th_f_theor)*(th_f - th_f_theor);
    };
    double_t getFocalResidualPhi(){
        return (ph_f - ph_f_theor)*(ph_f - ph_f_theor);
    }

    double_t getFocalResidualY(){
        return (y_f - y_f_theor)*(y_f - y_f_theor);
    }
    void ResetFocalVar(){
        th_f=0;
        ph_f=0;
        y_f=0;
        x_f=0;
    }
    void Print();
};

/// Diagnose Plot
void vdcTrack::Print() {
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<std::endl;
    std::cout <<"\t transport CS: "<<std::endl;
    std::cout <<"\t\t x :"<< x_tr<<std::endl;
    std::cout <<"\t\t th:"<< th_tr<<std::endl;
    std::cout <<"\t\t y :"<< y_tr<<std::endl;
    std::cout <<"\t\t ph:"<< ph_tr<<std::endl;
    std::cout << ".........................................."<<std::endl;
    std::cout <<"\t Focalplane CS: "<<std::endl;
    std::cout <<"\t\t f_x :"<< x_f<<std::endl;
    std::cout <<"\t\t f_th:"<< th_f<<std::endl;
    std::cout <<"\t\t f_y :"<< y_f<<std::endl;
    std::cout <<"\t\t f_ph:"<< ph_f<<std::endl;
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<std::endl;
}

/// Write the Detector Coordination Parameters to the track, the detector coordination parameter
/// will be used in theta_focal calculation
/// \param x
/// \param theta
/// \param y
/// \param phi
void vdcTrack::WriteDCSVar(double x, double theta, double y, double phi) {
    d_x= x;
    d_th = theta;
    d_y = y;
    d_ph = phi;
}

///
/// \param f_x
/// \param f_th
/// \param f_y
/// \param f_ph
void vdcTrack::WriteFocalVar(double f_x, double f_th, double f_y, double f_ph) {
    x_f =  f_x;
    th_f = f_th;
    y_f = f_y;
    ph_f = f_ph;
}


///
/// \param x_tr
/// \param th_tr
/// \param f_tr
/// \param ph_tr
void vdcTrack::WriteTRCSVar(double x, double th, double y, double ph) {
    th_tr = th;
    ph_tr = ph;
    y_tr = y;
    x_tr = x;
}

///
Bool_t LoadDatabase(TString DataBaseName,TString fPrefix){
    UInt_t kPORDER = 7;
    static const char* const here = "LoadDataBase";
    TString OldComments = "";
    FILE* file = fopen(DataBaseName, "r");
    if (!file) {
        std::cout<<"[error]"<<__FUNCTION__ <<"("<<__LINE__<<"): Error Loadding Database "<<DataBaseName<<std::endl;
        return false;
    }

    const int LEN = 200;
    char buff[LEN];

    // Look for the section [<prefix>.global] in the file, e.g. [ R.global ]
    TString tag(fPrefix);
    Ssiz_t tagpos = tag.Index(".");
    if (tagpos != kNPOS)
        tag = tag(0, tagpos + 1);
    else
        tag.Append(".");
    tag.Prepend("[");
    tag.Append("global]");
    TString tag2(tag);
    tag.ToLower();

    bool found = false;
    while (!found && fgets(buff, LEN, file) != NULL) {
        // read in comments
        TString line = buff;
        if (line.BeginsWith("#")) {
            OldComments += line;
            // OldComments += "\n";
        }

        line = ::Compress(buff); // strip blanks
        if (line.EndsWith("\n")) line.Chop();

        line.ToLower();
        if (tag == line) found = true;
    }
    if (!found) {
        std::cout<<"[error]"<<__FUNCTION__ <<"("<<__LINE__<<"): Error Loadding Database "<<DataBaseName<<std::endl;
        fclose(file);
        assert(0); //
        return false;
    }

    // We found the section, now read the data
    fgets(buff, LEN, file); // Skip constant line
    fgets(buff, LEN, file); // Skip comment line

    fTMatrixElems.clear();
    fDMatrixElems.clear();
    fPMatrixElems.clear();
    fPTAMatrixElems.clear();
    fYMatrixElems.clear();
    fYTAMatrixElems.clear();
    fRMatrixElems.clear();

    fFPMatrixElems.clear();
    fFPMatrixElems.resize(3);

    typedef std::vector<std::string>::size_type vsiz_t;
    std::map<std::string, vsiz_t> power;
    power["t"] = 3; // transport to focal-plane tensors
    power["y"] = 3;
    power["p"] = 3;
    power["D"] = 3; // focal-plane to target tensors
    power["T"] = 3;
    power["Y"] = 3;
    power["YTA"] = 4;
    power["P"] = 3;
    power["PTA"] = 4;
    power["R"] = 4; // pathlength from z=0 (target) to focal plane (meters)
    power["XF"] = 5; // forward: target to focal-plane (I think)
    power["TF"] = 5;
    power["PF"] = 5;
    power["YF"] = 5;

    std::map<std::string, std::vector<THaMatrixElement>*> matrix_map;
    matrix_map["t"] = &fFPMatrixElems;
    matrix_map["y"] = &fFPMatrixElems;
    matrix_map["p"] = &fFPMatrixElems;
    matrix_map["D"] = &fDMatrixElems;
    matrix_map["T"] = &fTMatrixElems;
    matrix_map["Y"] = &fYMatrixElems;
    matrix_map["YTA"] = &fYTAMatrixElems;
    matrix_map["P"] = &fPMatrixElems;
    matrix_map["PTA"] = &fPTAMatrixElems;
    matrix_map["R"] = &fRMatrixElems;

    std::map<std::string, int> fp_map;
    fp_map["t"] = 0;
    fp_map["y"] = 1;
    fp_map["p"] = 2;

    // Read in as many of the matrix elements as there are.
    // Read in line-by-line, so as to be able to handle tensors of
    // different orders.
    while (fgets(buff, LEN, file)) {
        std::string line(buff);
        // Erase trailing newline
        if (line.size() > 0 && line[line.size() - 1] == '\n') {
            buff[line.size() - 1] = 0;
            line.erase(line.size() - 1, 1);
        }
        // Split the line into whitespace-separated fields
        std::vector<std::string> line_spl = Split(line);

        // Stop if the line does not start with a string referring to
        // a known type of matrix element. In particular, this will
        // stop on a subsequent timestamp or configuration tag starting with "["
        if (line_spl.empty()) continue; //ignore empty lines
        const char* w = line_spl[0].c_str();
        vsiz_t npow = power[w];
        if (npow == 0) break;

#if DEBUG_LEVEL>=4
        cout << "Matrix Line = ";
        for (Ssiz_t i = 1; (UInt_t) i < (UInt_t) line_spl.size(); i++) {
            cout << i << "(" << line_spl[i].c_str() << "), ";
        }
        cout << endl;
#endif

        // Looks like a good line, go parse it.
        THaMatrixElement ME;
        ME.pw.resize(npow);
        ME.iszero = true;
        ME.order = 0;
        vsiz_t pos;
        for (pos = 1; pos <= npow && pos < line_spl.size(); pos++) {
            ME.pw[pos - 1] = atoi(line_spl[pos].c_str());
        }
        vsiz_t p_cnt;
        for (p_cnt = 0; pos < line_spl.size() && p_cnt < kPORDER && pos <= npow + kPORDER; pos++, p_cnt++) {
            ME.poly[p_cnt] = atof(line_spl[pos].c_str());
            if (ME.poly[p_cnt] != 0.0) {
                ME.iszero = false;
                ME.order = p_cnt + 1;
            }
        }
        if (p_cnt < 1) {
            std::cout<<"[error]"<<__FUNCTION__ <<"("<<__LINE__<<"): "<<Form("Could not read in Matrix Element %s%d%d%d!", w, ME.pw[0], ME.pw[1], ME.pw[2])<<std::endl;
            std::cout<<"[error]"<<__FUNCTION__ <<"("<<__LINE__<<"): "<<Form("Line looks like: %s", line.c_str())<<std::endl;

            fclose(file);
            return false;
        }

        // order optimize to
        ME.OptOrder = atoi(line_spl[line_spl.size() - 1].c_str());

        // added by siyu to fix the issues when the number of none-zero > optOrder
        ME.order=ME.OptOrder;


        // Don't bother with all-zero matrix elements
        if (ME.iszero) continue;

        // Add this matrix element to the appropriate array
        std::vector<THaMatrixElement> *mat = matrix_map[w];
        if (mat) {
            // Special checks for focal plane matrix elements
            if (mat == &fFPMatrixElems) {
                ME.order = 4;
                if (ME.pw[0] == 0 && ME.pw[1] == 0 && ME.pw[2] == 0) {
                    THaMatrixElement& m = (*mat)[fp_map[w]];
                    if (m.order > 0) {
                        std::cout<<"[error]"<<__FUNCTION__ <<"("<<__LINE__<<"): "<<Form("Duplicate definition of focal plane matrix element: %s. Using first definition.", buff)<<std::endl;
                    } else
                        m = ME;
                } else
                    std::cout<<"[error]"<<__FUNCTION__ <<"("<<__LINE__<<"): "<<Form("Bad coefficients of focal plane matrix element %s", buff)<<std::endl;
            } else {
                // All other matrix elements are just appended to the respective array
                // but ensure that they are defined only once!
                bool match = false;
                for (vector<THaMatrixElement>::iterator it = mat->begin(); it != mat->end() && !(match = it->match(ME)); it++) {
                }
                if (match) {
                    std::cout<<"[error]"<<__FUNCTION__ <<"("<<__LINE__<<"): "<<Form("Duplicate definition of matrix element: %s. Using first definition.", buff)<<std::endl;
                } else
                    mat->push_back(ME);
            }
        }
    }
    return true;
}


/// Load the f51 formate raw data file
/// data structure
/// KineID, runID, x_d, th_d, y_d, ph_d, x_tr, th_tr, y_tr, ph_d
/// \param rawfname
/// \return
Bool_t LoadRawf51(TString rawfname){
    return false;

}



///
/// \param rawFname
/// \return
Bool_t LoadRaw(TString rawFname){
    if (rawFname.EndsWith(".root")){
        LoadRaw(rawFname.Data());
    } else{
        LoadRawf51(rawFname);
    }
    return false;
}


/// Calculate the Matrix element
/// \param x
/// \param matrix
void CalcMatrix(const double_t x, std::vector<THaMatrixElement> & matrix){
    // calculates the values of the matrix elements for a given location
    // by evaluating a polynomial in x of order it->order with
    // coefficients given by it->poly
    for (vector<THaMatrixElement>::iterator it = matrix.begin();
         it != matrix.end(); it++) {
         it->v = 0.0;

        if (it->order > 0) {
            for (int i = it->order - 1; i >= 1; i--)
                it->v = x * (it->v + it->poly[i]);
            it->v += it->poly[0];
        }
    }
}

///
/// \param track
void CalcFocalPlaneCoords(vdcTrack* track){
    // Y: Y000 Parameter
    // theta: T
    // phi : P, T

    auto theta = track->getTrTheta();
    auto phi = track->getTrPhi();
    auto x = track->getTrX();
    auto y = track->getTrY();

    //calculate the rotating tracksport frame coordinations
    double_t r_x = x;

    // calculate the focal-plane matrix elements
    CalcMatrix( r_x, fFPMatrixElems );

    Double_t r_y = y - fFPMatrixElems[Y000].v;  // Y000

    // Calculate now the tan(rho) and cos(rho) of the local rotation angle.
    Double_t tan_rho_loc = fFPMatrixElems[T000].v;   // T000
    Double_t cos_rho_loc = 1.0/sqrt(1.0+tan_rho_loc*tan_rho_loc);
    Double_t r_phi = (track->getDPhi() - fFPMatrixElems[P000].v /* P000 */ ) /
                     (1.0-track->getDTheta()*tan_rho_loc) / cos_rho_loc;

    Double_t r_theta = (track->getDTheta()+tan_rho_loc) /
                       (1.0-track->getDTheta()*tan_rho_loc);

    //write the data to the track
    track->WriteFocalVar(r_x, r_theta, r_y, r_phi );
}



inline Bool_t IsFileExist (const std::string& name) {
    return !gSystem->AccessPathName(name.c_str());
}

std::map<UInt_t, TCutG *> cutList;

TCutG *getCentralSieveCut(UInt_t runID){

    TString HRS = "L";
    if (runID > 20000){
        HRS = "R";
    }

    TString cutFileName(Form("cutFiles/WithOutMomCut/prex%sHRS_%d_-1.root.FullCut.root",HRS.Data(),runID));
    if (cutList.find(runID) == cutList.end())
    {
        if (IsFileExist(cutFileName.Data())){
            TFile *cutFileIO=new TFile(cutFileName.Data(),"READ");
            if(cutFileIO->IsZombie()){
                std::cout<<"[ERROR]:: CAN NOT FIND CUT FILE \" "<<cutFileName.Data()<<"\""<<std::endl;
                exit(-1);
            }
            auto cutg=(TCutG*)gROOT->FindObject(Form("hcut_R_%d_%d_%d", 0, 6, 3));
            if (cutg){
                cutg->SetName(Form("run%d_hcut_R_%d_%d_%d",runID, 1, 6, 3));
                cutList[runID] = cutg;
                return cutg;
            }else{
                std::cout<<"[ERROR]:: CAN NOT Find Cut"<<std::endl;
                exit(-1);
            }
        } else{
            std::cout<<"[ERROR]:: can not find cut file "<< cutFileName.Data()<<std::endl;
            exit(-1);
        }
    }

    return  cutList[runID];
}

// load the root file and  return the TChain
TChain *LoadrootFile(UInt_t runID,TString folder="/home/newdriver/Storage/Research/PRex_Experiment/PRex_Replay/replay/Result/20201217_vdcphCorrected"){
    TChain *chain=new TChain("T");
    TString HRS="R";
    if(runID<20000){HRS="L";};

    if(folder.EndsWith(".root")){
        chain->Add(folder.Data());
    }else{
        TString rootDir(folder.Data());
        if(runID>20000){ //RHRS
            if(IsFileExist(Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID))){
                std::cout<<"Add File::"<<Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
                chain->Add(Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID));

                TString filename;
                int16_t split=1;
                filename=Form("%s/prexRHRS_%d_-1_%d.root",rootDir.Data(),runID,split);
                while (IsFileExist(filename.Data())){
                    std::cout<<"Add File::"<<filename.Data()<<std::endl;
                    chain->Add(filename.Data());
                    split++;
                    filename=Form("%s/prexRHRS_%d_-1_%d.root",rootDir.Data(),runID,split);
                }
            }else{
                std::cout<<"\033[1;33m [Warning]\033[0m Missing file :"<<Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
            }
        }else{
            HRS="L";
            if(IsFileExist(Form("%s/prexLHRS_%d_-1.root",rootDir.Data(),runID))){
                std::cout<<"Add File::"<<Form("%s/prexLHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
                chain->Add(Form("%s/prexLHRS_%d_-1.root",rootDir.Data(),runID));

                TString filename;
                int16_t split=1;
                filename=Form("%s/prexLHRS_%d_-1_%d.root",rootDir.Data(),runID,split);
                while (IsFileExist(filename.Data())){
                    std::cout<<"Add File::"<<filename.Data()<<std::endl;
                    chain->Add(filename.Data());
                    split++;
                    filename=Form("%s/prexLHRS_%d_-1_%d.root",rootDir.Data(),runID,split);
                }
            }else{
                std::cout<<"\033[1;33m [Warning]\033[0m Missing file :"<<Form("%s/prexRHRS_%d_-1.root",rootDir.Data(),runID)<<std::endl;
            }
        }
    }
    return chain;
}

///
/// \param rawRootFname
/// \return
Bool_t LoadRawRoot(UInt_t runID, UInt_t maxEvent = 5000){

    TString HRS = "L";
    auto fChain = LoadrootFile(runID);

    auto cutg = getCentralSieveCut(runID);

    assert(fChain);

    TString filenameStr(  fChain->GetName());
    if (filenameStr.Contains("R")){
        HRS = "R";
    }

    LoadDatabase(Form("PRex_%sHRS.db",HRS.Data()),"L");

    TBranch        *b_Ndata_L_tr_x;   //!
    TBranch        *b_L_tr_x;   //!
    TBranch        *b_Ndata_L_tr_y;   //!
    TBranch        *b_L_tr_y;   //!
    TBranch        *b_Ndata_L_tr_th;   //!
    TBranch        *b_L_tr_th;   //!
    TBranch        *b_Ndata_L_tr_ph;   //!
    TBranch        *b_L_tr_ph;   //!

    TBranch        *b_Ndata_L_tr_d_x;   //!
    TBranch        *b_L_tr_d_x;   //!
    TBranch        *b_Ndata_L_tr_d_y;   //!
    TBranch        *b_L_tr_d_y;   //!
    TBranch        *b_Ndata_L_tr_d_ph;   //!
    TBranch        *b_L_tr_d_ph;   //!
    TBranch        *b_Ndata_L_tr_d_th;   //!
    TBranch        *b_L_tr_d_th;   //!


    Int_t           Ndata_L_tr_x=0;
    Double_t        L_tr_x[6];   //[Ndata.L.tr.x]
    Int_t           Ndata_L_tr_y=0;
    Double_t        L_tr_y[6];   //[Ndata.L.tr.y]
    Int_t           Ndata_L_tr_ph=0;
    Double_t        L_tr_ph[6];   //[Ndata.L.tr.ph]
    Int_t           Ndata_L_tr_th=0;
    Double_t        L_tr_th[6];   //[Ndata.L.tr.th]

    Int_t           Ndata_L_tr_d_x;
    Double_t        L_tr_d_x[6];   //[Ndata.L.tr.d_x]
    Int_t           Ndata_L_tr_d_y;
    Double_t        L_tr_d_y[6];   //[Ndata.L.tr.d_y]
    Int_t           Ndata_L_tr_d_th;
    Double_t        L_tr_d_th[6];   //[Ndata.L.tr.d_th]
    Int_t           Ndata_L_tr_d_ph;
    Double_t        L_tr_d_ph[6];   //[Ndata.L.tr.d_ph]



    fChain->SetBranchAddress("Ndata.L.tr.x", &Ndata_L_tr_x, &b_Ndata_L_tr_x);
    fChain->SetBranchAddress("L.tr.x", L_tr_x, &b_L_tr_x);
    fChain->SetBranchAddress("Ndata.L.tr.y", &Ndata_L_tr_y, &b_Ndata_L_tr_y);
    fChain->SetBranchAddress("L.tr.y", L_tr_y, &b_L_tr_y);
    fChain->SetBranchAddress("Ndata.L.tr.th", &Ndata_L_tr_th, &b_Ndata_L_tr_th);
    fChain->SetBranchAddress("L.tr.th", L_tr_th, &b_L_tr_th);
    fChain->SetBranchAddress("Ndata.L.tr.ph", &Ndata_L_tr_ph, &b_Ndata_L_tr_ph);
    fChain->SetBranchAddress("L.tr.ph", L_tr_ph, &b_L_tr_ph);

    fChain->SetBranchAddress("Ndata.L.tr.d_ph", &Ndata_L_tr_d_ph, &b_Ndata_L_tr_d_ph);
    fChain->SetBranchAddress("L.tr.d_ph", L_tr_d_ph, &b_L_tr_d_ph);
    fChain->SetBranchAddress("Ndata.L.tr.d_th", &Ndata_L_tr_d_th, &b_Ndata_L_tr_d_th);
    fChain->SetBranchAddress("L.tr.d_th", L_tr_d_th, &b_L_tr_d_th);
    fChain->SetBranchAddress("Ndata.L.tr.d_x", &Ndata_L_tr_d_x, &b_Ndata_L_tr_d_x);
    fChain->SetBranchAddress("L.tr.d_x", L_tr_d_x, &b_L_tr_d_x);
    fChain->SetBranchAddress("Ndata.L.tr.d_y", &Ndata_L_tr_d_y, &b_Ndata_L_tr_d_y);
    fChain->SetBranchAddress("L.tr.d_y", L_tr_d_y, &b_L_tr_d_y);

    maxEvent = maxEvent > fChain->GetEntries() ? fChain->GetEntries(): maxEvent;
    assert(maxEvent);
    std::cout <<"Loading  the Root file and calculate the focalVar::"<<std::endl;

    UInt_t EventCounter = 0;

    for (UInt_t entry = 0 ; entry < maxEvent; entry ++){
        try {
            fChain->GetEntry(entry);
            if(fChain->Query("",cutg->GetName(),"",1,entry)->GetRowCount()==0) continue;
            if (entry%100==0 ) {
                MemInfo_t memInfo;
                CpuInfo_t cpuInfo;
                gSystem->GetMemInfo(&memInfo);
                std::cout<<"\x1B[s"<<Form("\tProgress %2.1f",(double)entry*100/maxEvent)<<"%" <<" /"<<Form("RAM:%6d KB",memInfo.fMemUsed)<<"\x1B[u" << std::flush;
            }


            if (Ndata_L_tr_x != 1 || Ndata_L_tr_d_x != 1 ||Ndata_L_tr_d_y != 1 || Ndata_L_tr_d_th != 1 || Ndata_L_tr_d_ph !=1) continue;
            vdcTrack track;

            track.WriteTRCSVar(L_tr_x[0],L_tr_th[0],L_tr_y[0],L_tr_ph[0]);
            track.WriteDCSVar(L_tr_d_x[0],L_tr_d_th[0],L_tr_d_y[0],L_tr_d_ph[0]);
            vdcTrackData.push_back(track);
            EventCounter +=1;
            if (EventCounter >= 200) break; // set an limite on the number of event that pass the filter
        } catch (exception const &e){
            std::cout<<"[Exception]:: Ignored ->"<<e.what()<<std::endl;
            continue;
        }
    }
    return true;
}


void Array2Matrix(Double_t *par, UInt_t OptOrder = 0,EFPMatrixElemTag tag = T000){

    // reset the parameters
    if (!OptOrder) {
        std::cout<<__FUNCTION__ <<":: Optimization order should not be zero"<<std::endl;
    }

    fFPMatrixElems[tag].order = OptOrder;
    fFPMatrixElems[tag].poly.clear();
    for(UInt_t counter = 0 ; counter < OptOrder ; counter ++){
        fFPMatrixElems[tag].poly.push_back(par[counter]);
    }
}


Double_t getResidual(EFPMatrixElemTag tag = T000){

    double residual = 0.0;

    for (int i = 0; i < vdcTrackData.size(); ++i) {
        CalcFocalPlaneCoords(&vdcTrackData[i]);   // Update the focal Variable track
    }
    if (tag == T000){
        for (auto track : vdcTrackData){
            residual += track.getFocalResidualTheta();
        }
    } else if (tag == P000){
        for (auto track : vdcTrackData){
            residual += track.getFocalResidualPhi();
        }
    } else if (tag == Y000){
        for (auto  track: vdcTrackData){
            residual += track.getFocalResidualY();
        }
    }
    return  residual;
}

void OptFunctionTheta(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t)
{
    // pass the parameter to the Optimizer Matrix
    Array2Matrix(par,3,T000);
    f = getResidual(T000);
//    std::cout << "Residual::"<<f<<std::endl;
    return;
}

void OptFunctionPhi(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t)
{
    // pass the parameter to the Optimizer Matrix
    Array2Matrix(par,3,P000);
    f = getResidual(P000);
//    std::cout << "Residual::"<<f<<std::endl;
    return;
}



void OptFunctionY(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t)
{
    //TODO
    //How to  get the Y000 optimization function
    Array2Matrix(par,3, Y000);
    f = getResidual(Y000);

    return;
}

void Optimizer(EFPMatrixElemTag tag=T000, UInt_t OptOrder = 3){

    // start the Optimizer
    TVirtualFitter::SetDefaultFitter("Minuit");
    TVirtualFitter *fitter = TVirtualFitter::Fitter(NULL,OptOrder);

    if (tag == P000){
        fitter->SetFCN(OptFunctionPhi);
    } else if (tag == T000){
        fitter->SetFCN(OptFunctionTheta);
    } else if (tag == Y000){
        fitter->SetFCN(OptFunctionY);
    }

    // load the Optimize Parameters
    for(UInt_t ParaIter = 0 ; ParaIter < OptOrder ; ParaIter ++ ){

        Double_t  absold =TMath::Abs(fFPMatrixElems[tag].poly[0]);
        Double_t abslimit = absold > 0 ? absold * 10 : 10;
        fitter->SetParameter(ParaIter, Form("TMatrix T000 %03d", ParaIter), fFPMatrixElems[tag].poly[ParaIter], absold > 0 ? absold / 10 : 0.1, -abslimit, abslimit);

    }
    fitter->Print();
    cout << fitter->GetNumberFreeParameters() << " Free  / " << fitter->GetNumberTotalParameters() << " Parameters\n";

    Double_t arglist[1] = {0};
    fitter->ExecuteCommand("MIGRAD", arglist, 0);

}

///
/// \param dbname file name that optimized database write to
void WriteDataBase(TString dbname = "result/Optimized.db"){
    FILE *file = fopen(dbname.Data(),"w");
    if (!file) {
        Info("SaveDataBase", "Error Open in %s", dbname.Data());
    }

    fprintf(file,"t 0 0 0 ");
    for (UInt_t i = 0; i < fFPMatrixElems[T000].order ; i++){
        fprintf(file, " %13.6e", fFPMatrixElems[T000].poly[i]);
    }
    for (UInt_t i = fFPMatrixElems[T000].order; i < 4 ; i++){
        fprintf(file, " %13.6e", 0.0);
    }
    fprintf(file, "\n");

    fprintf(file,"y 0 0 0 ");
    for (UInt_t i = 0; i < fFPMatrixElems[Y000].order ; i++){
        fprintf(file, " %13.6e", fFPMatrixElems[Y000].poly[i]);
    }
    for (UInt_t i = fFPMatrixElems[Y000].order; i < 4 ; i++){
        fprintf(file, " %13.6e", 0.0);
    }
    fprintf(file, "\n");

    fprintf(file,"p 0 0 0 ");
    for (UInt_t i = 0; i < fFPMatrixElems[P000].order ; i++){
        fprintf(file, " %13.6e", fFPMatrixElems[P000].poly[i]);
    }
    for (UInt_t i = fFPMatrixElems[P000].order; i < 4 ; i++){
        fprintf(file, " %13.6e", 0.0);
    }
    fprintf(file, "\n");

    fclose(file);
}



/// \param Optimizer
void vdcConstOptimizer(){

    LoadRawRoot(2240,5000);
    std::cout <<"\n Total Event After Loading File "<< vdcTrackData.size()<<std::endl;
    LoadRawRoot(2256,5000);
    std::cout <<"\n Total Event After Loading File "<< vdcTrackData.size()<<std::endl;
    LoadRawRoot(2257,5000);
    std::cout <<"\n Total Event After Loading File "<< vdcTrackData.size()<<std::endl;

    Optimizer(Y000);
    Optimizer(T000);
    Optimizer(P000);

    for (auto element: fFPMatrixElems){
        element.print();
    }
    WriteDataBase();
}

void test_focalCalculator(){
    /// Load Database tester
    LoadDatabase("PRex_LHRS.db","L");
    TString fname = "/home/newdriver/Storage/Research/PRex_Experiment/PRex_Replay/replay/Result/prexLHRS_2240_-1.root";
    auto fChain = new TChain("T");
    fChain->AddFile(fname.Data());

    TBranch        *b_Ndata_L_tr_x;   //!
    TBranch        *b_L_tr_x;   //!
    TBranch        *b_Ndata_L_tr_y;   //!
    TBranch        *b_L_tr_y;   //!
    TBranch        *b_Ndata_L_tr_th;   //!
    TBranch        *b_L_tr_th;   //!
    TBranch        *b_Ndata_L_tr_ph;   //!
    TBranch        *b_L_tr_ph;   //!

    TBranch        *b_Ndata_L_tr_d_x;   //!
    TBranch        *b_L_tr_d_x;   //!
    TBranch        *b_Ndata_L_tr_d_y;   //!
    TBranch        *b_L_tr_d_y;   //!
    TBranch        *b_Ndata_L_tr_d_ph;   //!
    TBranch        *b_L_tr_d_ph;   //!
    TBranch        *b_Ndata_L_tr_d_th;   //!
    TBranch        *b_L_tr_d_th;   //!


    Int_t           Ndata_L_tr_x=0;
    Double_t        L_tr_x[6];   //[Ndata.L.tr.x]
    Int_t           Ndata_L_tr_y=0;
    Double_t        L_tr_y[6];   //[Ndata.L.tr.y]
    Int_t           Ndata_L_tr_ph=0;
    Double_t        L_tr_ph[6];   //[Ndata.L.tr.ph]
    Int_t           Ndata_L_tr_th=0;
    Double_t        L_tr_th[6];   //[Ndata.L.tr.th]

    Int_t           Ndata_L_tr_d_x;
    Double_t        L_tr_d_x[6];   //[Ndata.L.tr.d_x]
    Int_t           Ndata_L_tr_d_y;
    Double_t        L_tr_d_y[6];   //[Ndata.L.tr.d_y]
    Int_t           Ndata_L_tr_d_th;
    Double_t        L_tr_d_th[6];   //[Ndata.L.tr.d_th]
    Int_t           Ndata_L_tr_d_ph;
    Double_t        L_tr_d_ph[6];   //[Ndata.L.tr.d_ph]



    fChain->SetBranchAddress("Ndata.L.tr.x", &Ndata_L_tr_x, &b_Ndata_L_tr_x);
    fChain->SetBranchAddress("L.tr.x", L_tr_x, &b_L_tr_x);
    fChain->SetBranchAddress("Ndata.L.tr.y", &Ndata_L_tr_y, &b_Ndata_L_tr_y);
    fChain->SetBranchAddress("L.tr.y", L_tr_y, &b_L_tr_y);
    fChain->SetBranchAddress("Ndata.L.tr.th", &Ndata_L_tr_th, &b_Ndata_L_tr_th);
    fChain->SetBranchAddress("L.tr.th", L_tr_th, &b_L_tr_th);
    fChain->SetBranchAddress("Ndata.L.tr.ph", &Ndata_L_tr_ph, &b_Ndata_L_tr_ph);
    fChain->SetBranchAddress("L.tr.ph", L_tr_ph, &b_L_tr_ph);

    fChain->SetBranchAddress("Ndata.L.tr.d_ph", &Ndata_L_tr_d_ph, &b_Ndata_L_tr_d_ph);
    fChain->SetBranchAddress("L.tr.d_ph", L_tr_d_ph, &b_L_tr_d_ph);
    fChain->SetBranchAddress("Ndata.L.tr.d_th", &Ndata_L_tr_d_th, &b_Ndata_L_tr_d_th);
    fChain->SetBranchAddress("L.tr.d_th", L_tr_d_th, &b_L_tr_d_th);
    fChain->SetBranchAddress("Ndata.L.tr.d_x", &Ndata_L_tr_d_x, &b_Ndata_L_tr_d_x);
    fChain->SetBranchAddress("L.tr.d_x", L_tr_d_x, &b_L_tr_d_x);
    fChain->SetBranchAddress("Ndata.L.tr.d_y", &Ndata_L_tr_d_y, &b_Ndata_L_tr_d_y);
    fChain->SetBranchAddress("L.tr.d_y", L_tr_d_y, &b_L_tr_d_y);


    for (UInt_t entry = 0; (entry <= fChain->GetEntries() ) && (entry < 1000); entry++){
        fChain->GetEntry(entry);
        vdcTrack track;
        track.WriteTRCSVar(L_tr_x[0],L_tr_th[0],L_tr_y[0],L_tr_ph[0]);
        track.WriteDCSVar(L_tr_d_x[0],L_tr_d_th[0],L_tr_d_y[0],L_tr_d_ph[0]);
        CalcFocalPlaneCoords(& track);
        vdcTrackData.push_back(track);
        track.Print();
        std::cout <<"x: "<<L_tr_x[0]<<"/"<<Ndata_L_tr_x<<"    th:"<<L_tr_th[0]<<"/"<<Ndata_L_tr_th <<"  y:"<<L_tr_y[0]<<"/"<<Ndata_L_tr_y<<"    ph:"<<L_tr_ph[0]<<"/"<<Ndata_L_tr_ph<<std::endl;
        getchar();
    }

}
///
void tester(){
    /// Load Database tester
    LoadDatabase("PRex_RHRS.db","R");
    for(auto element : fFPMatrixElems){
        element.print();
    }

    //test the calculation of the Focal Plane variables
}


/// void testMatrix
void test_ArrayMatrix(){

    for (UInt_t i = 0; i < 10; i ++){
        LoadDatabase("PRex_LHRS.db","L");
        std::cout<<__FUNCTION__ <<"  Test with Parameter "<< i <<std::endl;

        double a[i];
        for (UInt_t counter = 0 ; counter < i ; counter ++){
            a[counter] = i*1.5;
        }
        Array2Matrix(a,i, P000);

        for(auto element:fFPMatrixElems){
            element.print();
        }

        getchar();
    }

}

void test_getResidual(){

}

void test_getCut(UInt_t runID = 2240){
    auto chain = LoadrootFile(runID);
    TH2F *thetaphihh = new TH2F("thetaphi","thetaphi",1000,-0.045,0.045,1000,-0.045,0.045);
    chain->Project(thetaphihh->GetName(),"L.gold.th:L.gold.ph");
    TCanvas *canv = new TCanvas("a","a",1960,1080);
    canv->Draw();
    canv->cd();
    thetaphihh->Draw("zcol");
    if(getCentralSieveCut(runID)){
        getCentralSieveCut(runID)->Draw("same");
    }
}