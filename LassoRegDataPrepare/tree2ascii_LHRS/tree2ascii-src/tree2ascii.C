// tree2ascii.C
//
// Ole Hansen, JLab, February 2004
//
// Read tree(s) from ROOT file(s) and write selected variables to
// text file subject to cuts.
//
// Modified by Jin Huang to support additional cuts with -C


#include "TChain.h"
#include "TFile.h"
#include "TObjString.h"
#include "TRegexp.h"
#include "TTreeFormula.h"
#include "TCutG.h"
#include "TBranchObject.h"
#include "TBranchElement.h"
#include "TKey.h"
#include "TROOT.h"
#include "TList.h"
#include "TObjArray.h"
#include "TMath.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <libgen.h>

using namespace std;

class DataElement;

Int_t GetEntry(Int_t entry);
char* GetTreeVariable(const char* name);
DataElement* GetTreeVariableInfo(const char* name);
Int_t LoadGCuts();
Int_t LoadTree(Int_t entry);
Int_t LoadTreeVariables();
Int_t LoadDefinedVariables();
Int_t LoadNCuts();
void Init(TTree *tree);
void Notify();
Int_t WriteVariables(FILE* fi, int icut = -255);

TTree* fChain = 0; //pointer to the analyzed TTree or TChain
Int_t fCurrent = -1; //current Tree number in a TChain
TList fElements; // List of tree variable descriptors
TList fWriteElements; // List of tree variables to write
TList fPeaks; // List of peak definitions
TList fFiles; // List of input file names

// Parameters and flags, set via command line
const char* treename = "T";
const char* gcutsfile = 0;
const char* peakfile = 0;
const char* program = 0;
const char* ncutsfile = 0;
const char* format = "12g";
bool lAppend = false;
bool lDebug = false;
bool lList = false;
bool lProgr = false;
bool lQuiet = false;
bool lDefFile = false;
bool lNCutsFile = false;
Int_t fNev = -1; //Max number of events to read (-1=all)
Int_t fNout = -1; //Max number of events to write (-1=all)
Int_t fNSieveAll = -1;
Int_t* fNSieve;
Int_t* fSieveCount=NULL;
Int_t fPkOffset = 0;
TString fOutfile("-"); //Output file name
TString fVarDef("tr.d_x tr.d_y tr.d_th tr.d_ph gold.x");

TString fAdditionalCut = "1";

static const char* types[] = { "", "Char_t", "Short_t", "Int_t", "Long_t",
		"Float_t", "Int_t", "", "Double_t", "", "", "UChar_t", "UShort_t",
		"UInt_t", "ULong_t", "UInt_t", 0 };

const Double_t kInvalid = 1e38;

//FIXME: replace with ifstreams
const size_t BUFLEN = 10240;

//-----------------------------------------------------------------------------
// Helper class for managing tree variables
class DataElement: public TNamed {
public:
	bool good; //Item found in tree
	bool write; //Write this item to output
	char* data; //Pointer to storage for data
	Int_t type; //Data type (3=int, 8=double)
	Int_t size; //Max size of data array (scalar=1)
	TLeaf* leafcount; //Leaf describing actual array size (var arrays)
	TString branchname; //Actual name of branch
	TBranch* branch; //Pointer to branch
	Int_t min, max; //Requested index range for output (max=-1: all)
	TString fmt; //Output format

	DataElement() :
		good(false), write(false), data(0), size(0), leafcount(0), branch(0),
				min(0), max(1) {
	}
	DataElement(const char* s, const char* f = ::format) :
		TNamed(s, s), good(false), write(false), data(0), size(0),
				leafcount(0), branch(0), min(0), max(1), fmt(f) {
	}
	~DataElement() {
		delete[] data;
	}

	Int_t GetSize() const {
		if (leafcount)
			return int(leafcount->GetValue() + .5);
		else
			return size;
	}

	Double_t GetValueAsDouble(Int_t i = 0) const {
		if (i >= GetSize())
			return kInvalid;
		switch (type) {
		case 1:
			return Double_t(*(((Char_t*) data) + i));
		case 2:
			return Double_t(*(((Short_t*) data) + i));
		case 3:
			return Double_t(*(((Int_t*) data) + i));
		case 4:
			return Double_t(*(((Long_t*) data) + i));
		case 5:
			return Double_t(*(((Float_t*) data) + i));
		case 6:
			return Double_t(*(((Int_t*) data) + i));
		case 8:
			return Double_t(*(((Double_t*) data) + i));
		case 11:
			return Double_t(*(((UChar_t*) data) + i));
		case 12:
			return Double_t(*(((UShort_t*) data) + i));
		case 13:
			return Double_t(*(((UInt_t*) data) + i));
		case 14:
			return Double_t(*(((ULong_t*) data) + i));
		case 15:
			return Double_t(*(((UInt_t*) data) + i));
		default:
			return kInvalid;
		}
	}

	bool IsArray() const {
		return (size > 1);
	}

	bool IsVarArray() const {
		return (size > 1 && leafcount != 0);
	}

	void Print(Option_t* opt = "") const {
		bool full = !(opt && *opt == 'S');
		if (full)
			cout << "DataElement: ";
		cout << GetName() << " ";
		if (full) {
			cout << "(" << branchname << ") ";
			cout << "\tgood:" << good;
		}
		if (good) {
			cout << " \ttype:" << type << " (" << types[type] << ")"
					<< " \tsize:" << size;
			cout << " \ta/va: " << IsArray() << "/" << IsVarArray();
			if (full) {
				cout << "  min/max: " << min << "/" << max;
				cout << "  fmt: " << fmt;
			}
		}
		cout << endl;
	}
};

//-----------------------------------------------------------------------------
Int_t GetEntry(Int_t entry) {
	// Read contents of entry.
	if (!fChain)
		return 0;
	return fChain->GetEntry(entry);
}

//-----------------------------------------------------------------------------
Int_t Init() {
	fChain = 0;
	if (treename && *treename)
		fChain = (TTree*) gDirectory->Get(treename);
	if (fChain == 0) {
		cerr << "Tree " << treename << " not found. Can't do a thing." << endl;
		return 1;
	}
	fCurrent = -1;
	fChain->SetMakeClass(1);
	if (lDebug)
		cout << "Found and initialized tree " << treename << endl;
	return 0;
}

//-----------------------------------------------------------------------------
void Notify() {
	// Called when loading a new file.
	// Get branch pointers.
	TIter next(&fElements);
	DataElement* item;
	while ((item = (DataElement*) next())) {
		if (!item->good)
			continue;
		item->branch = fChain->GetBranch(item->branchname);
	}
	return;
}

//-----------------------------------------------------------------------------
void Reset() {
	// Set branch addresses and load branch pointers
	// for all defined tree variables
	TIter next(&fElements);
	DataElement* item;
	while ((item = (DataElement*) next())) {
		if (!item->good)
			continue;
		fChain->SetBranchAddress(item->branchname, item->data);
	}
	Notify();
}

//-----------------------------------------------------------------------------
Int_t LoadTree(Int_t entry) {
	// Set the environment to read one entry
	if (!fChain)
		return -5;
	Int_t centry = fChain->LoadTree(entry);
	if (centry < 0)
		return centry;
	if (fChain->IsA() != TChain::Class())
		return centry;
	TChain *chain = (TChain*) fChain;
	if (chain->GetTreeNumber() != fCurrent) {
		fCurrent = chain->GetTreeNumber();
		Notify();
	}
	return centry;
}

//-----------------------------------------------------------------------------
Int_t WriteVariables(FILE* fi, int icut) {
	//FIXME: use fstream to avoid this type mess?
	const TRegexp ifmts("[di]");
	const TRegexp ufmts("[ouxX]");
	const TRegexp dfmts("[eEfFgG]");
	bool first = true;
	if (icut != -255) {
		fprintf(fi, "%3d", icut);
		first = false;
	}
	TIter next(&fWriteElements);
	DataElement* item;
	Int_t retval = 0;
	while ((item = (DataElement*) next()) && retval == 0) {
		if (!item->write || !item->good)
			continue;
		if (first)
			first = false;
		else
			fprintf(fi, " ");
		int kmax = (item->max >= 0) ? item->max : item->GetSize();
		for (int k = item->min; k < kmax; k++) {
			int status = 0;
			if (item->fmt.Contains(ifmts)) {
				int data = int(item->GetValueAsDouble(k) + .5);
				status = fprintf(fi, Form("%%%s", item->fmt.Data()), data);
			} else if (item->fmt.Contains(ifmts)) {
				unsigned data = unsigned(item->GetValueAsDouble(k) + .5);
				status = fprintf(fi, Form("%%%s", item->fmt.Data()), data);
			} else if (item->fmt.Contains(dfmts)) {
				double data = item->GetValueAsDouble(k);
				status = fprintf(fi, Form("%%%s", item->fmt.Data()), data);
			}
			if (status <= 0) {
				cerr << "Error writing item " << item->GetName() << endl;
				retval = 1;
				break;
			}
		}
	}
	fprintf(fi, "\n");
	return retval;
}

//-----------------------------------------------------------------------------
DataElement* GetTreeVariableInfo(const char* name) {
	if (!name || !*name)
		return NULL;
	DataElement* item = NULL;
	// Remove whitespace from name (in particular leading and trailing blanks)
	char* cname = Compress(name);
	if (*cname) {
		item = (DataElement*) fElements.FindObject(cname);
	}
	delete[] cname;
	return item;
}

//-----------------------------------------------------------------------------
char* GetTreeVariable(const char* name) {
	DataElement* elem = GetTreeVariableInfo(name);
	return (elem) ? elem->data : NULL;
}

//-----------------------------------------------------------------------------
Int_t LoadDefinedVariables() {
	if (lDebug)
		cout << "Parsing output variable definitions..." << endl;
	fWriteElements.Clear();
	Ssiz_t len = fVarDef.Length(), i = 0;
	char* buf = new char[len + 1];
	const char* p = fVarDef.Data();
	const char* name = p;
	int l, retval = 0, nvar = 0;
	while (i < len && sscanf(p + i, "%s%n", buf, &l) > 0) {
		TString s(buf), range, fmt;
		Ssiz_t lbrk = s.First('['), rbrk = s.First(']'), col = s.First(':');
		retval = 2;
		if (lbrk != kNPOS) {
			if (rbrk == kNPOS || rbrk <= lbrk + 1)
				break;
			range = s(lbrk + 1, rbrk - lbrk - 1);
		} else if (rbrk != kNPOS)
			break;
		if (col != kNPOS) {
			if (rbrk != kNPOS && col <= rbrk)
				break;
			fmt = s(col + 1, s.Length());
		}
		if (lbrk != kNPOS || col != kNPOS) {
			Ssiz_t pos = (lbrk != kNPOS) ? lbrk : col;
			s.Remove(pos);
		}
		//    s.Prepend(prefix);
		name = s.Data();
		DataElement* elem;
		if (!(elem = GetTreeVariableInfo(name))) {
			retval = 1;
			break;
		}
		if (!fmt.IsNull())
			elem->fmt = fmt;
		if (!range.IsNull()) {
			if (range == "*") {
				elem->min = 0;
				elem->max = -1;
			} else {
				Ssiz_t dash = range.First('-');
				if (dash != kNPOS) {
					if (dash == 0 || dash == range.Length() - 1)
						break;
					TString s = range(0, dash);
					range.Remove(0, dash + 1);
					elem->min = atoi(s.Data());
					elem->max = atoi(range.Data()) + 1;
				} else {
					elem->min = atoi(range.Data());
					elem->max = elem->min + 1;
				}
				retval = 3;
				if (elem->min < 0)
					elem->min = 0;
				if (elem->min >= elem->max)
					break;
				if (elem->IsArray() && !elem->IsVarArray() && elem->max
						> elem->size)
					break;
				if (!elem->IsArray() && (elem->min != 0 || elem->max != 1))
					break;
			}
		}
		retval = 0;
		nvar++;
		i += l;
		elem->write = true;
		fWriteElements.Add(elem);
		if (lDebug)
			elem->Print();
	}
	switch (retval) {
	case 0:
		if (lDebug)
			cout << nvar << " tree variable(s) successfully loaded" << endl;
		break;
	case 1:
		cerr << "Error loading tree variable " << name << endl;
		break;
	case 2:
		cerr << "Illegal definition " << buf << endl;
		break;
	case 3:
		cerr << "Index out of range for variable " << name << " (definition: "
				<< buf << ") " << endl;
		break;
	default:
		break;
	}
	delete[] buf;
	return retval;
}

//-----------------------------------------------------------------------------
Int_t LoadTreeVariables() {
	// Find all tree variables of basic type in the current tree,
	// determine their type, maximum size, and allocate
	// data storage for their data.
	//
	// This is a simplified algorithm that will probably not work with
	// every ROOT tree.  Each variable needs to be on its own branch,
	// and object branches are not supported.
	// The algorith is sufficient, however, for the simple trees
	// from C++ analyzer v1.

	if (lDebug)
		cout << "Parsing tree..." << endl;

	TObjArray* leaves = fChain->GetListOfLeaves();
	if (!leaves) {
		cerr << "Tree has no leaves??" << endl;
		return 1;
	}

	fElements.Delete();

	//   TBranchElement* bre;
	Ssiz_t pos;
	Int_t nleaves = leaves->GetEntriesFast();
	Int_t ngood = 0;
	TIter next(&fElements);
	for (Int_t ileaf = 0; ileaf < nleaves; ileaf++) {
		TLeaf* leaf = (TLeaf*) leaves->UncheckedAt(ileaf);
		TBranch* branch = leaf->GetBranch();
		if (!branch || branch->IsA() == TBranchObject::Class())
			continue;
		TString branchname = branch->GetName();
		if (branch->GetNleaves() > 1)
			continue;
		// 	branchname += ".";
		// 	branchname += leaf->GetTitle();
		// 	if( leafcount && (pos = branchname.First('[') != kNPOS ))
		// 	  branchname.Remove(pos);
		//       }
		//       if( branch->IsA() == TBranchElement::Class()) {
		// 	bre = (TBranchElement*)branch;

		//       } else
		// 	bre = 0;

		TLeaf* leafcount = leaf->GetLeafCount();
		Int_t len = (leafcount) ? leafcount->GetMaximum() : leaf->GetLen();
		if ((pos = branchname.First('[') != kNPOS))
			branchname.Remove(pos);
		const char** typ = types;
		while (*typ && strcmp(*typ, leaf->GetTypeName()))
			typ++;
		if (*typ && **typ) {
			DataElement* item = new DataElement(branchname);
			item->type = typ - types;
			item->good = true;
			item->size = len;
			item->data = new char[len * leaf->GetLenType()];
			item->branchname = branch->GetName();
			fChain->SetBranchAddress(item->branchname, item->data);
			if (leafcount)
				item->leafcount = leafcount;
			//	    fElements.AddLast(new DataElement(leafcount->GetName()));
			fChain->SetBranchAddress(item->branchname, item->data);
			item->branch = fChain->GetBranch(item->branchname);
			fElements.Add(item);
			ngood++;
		}
	}
	if (lDebug) {
		cout << "Loaded " << ngood << " tree variables from " << nleaves
				<< " tree leaves" << endl;
	}
	return 0;
}

//-----------------------------------------------------------------------------
void PrintTreeVariableList() {
	cout << "List of tree variables:" << endl;
	TIter next(&fElements);
	DataElement* item;
	while ((item = (DataElement*) next())) {
		if (item->good)
			item->Print("S");
	}
}

//-----------------------------------------------------------------------------
Int_t LoadGCuts() {
	if (!gcutsfile || !*gcutsfile)
		return 0;
	TFile* f = new TFile(gcutsfile, "READ");
	if (!f)
		return 1;
	TIter next(f->GetListOfKeys());
	TKey* key;
	if (lDebug)
		cout << "Reading graphical cuts from " << gcutsfile << " ..." << endl;
	int ngcut = 0;
	while ((key = (TKey*) next())) {
		if (!strcmp(key->GetClassName(), "TCutG")) {
			TCutG* g = dynamic_cast<TCutG*> (key->ReadObj());
			if (g) {
				if (lDebug)
					g->ls();
				ngcut++;
			}
		}
	}
	delete f;
	f = 0;
	if (lDebug)
		cout << ngcut << " cut(s) found" << endl;
	return 0;
}

//-----------------------------------------------------------------------------
Int_t LoadPeakDefinitions() {
	// Read definitions of cuts for the peaks.
	// If none found, this is not an error. The program will then write
	// out every event.
	if (!peakfile || !*peakfile)
		return 0;

	FILE* fi = fopen(peakfile, "r");
	if (!fi) {
		cerr << "Error opening " << peakfile << endl;
		return 1;
	}
	if (lDebug)
		cout << "Reading peak definition file " << peakfile << endl;

	fPeaks.Delete();
	// FIXME: ifstream
	char* buf = new char[BUFLEN];
	int npk = 0;
	while (fgets(buf, BUFLEN, fi)) {
		TString tbuf(buf);
		if (tbuf.EndsWith("\n"))
			tbuf.Chop();
		Ssiz_t pos = tbuf.First('#');
		if (pos != kNPOS)
			tbuf.Remove(pos);
		char* cbuf = Compress(tbuf.Data());
		TString sbuf(cbuf);
		delete[] cbuf;
		if (sbuf.IsNull() || sbuf[0] == '#')
			continue;

		sbuf = "( " + fAdditionalCut + " ) && ( " + sbuf + " )";

		if (lDebug)
			cout << "Cut " << npk << ": " << sbuf << endl;
		fPeaks.Add(new TTreeFormula(Form("peak_%d", npk++), sbuf.Data(), fChain));
	}
	fclose(fi);
	delete[] buf;
	buf = 0;
	if (lDebug) {
		if (npk == 0)
			cerr << "No peaks defined in " << peakfile << endl;
		else
			cout << npk << " peak definition(s) found" << endl;
	}
	return 0;
}

//-----------------------------------------------------------------------------
Int_t LoadVarDefFile() {
	FILE* fi = fopen(fVarDef.Data(), "r");
	if (!fi) {
		cerr << "Error opening " << fVarDef << endl;
		return 1;
	}
	if (lDebug)
		cout << "Reading variable list from file " << fVarDef << endl;

	fVarDef = "";
	// FIXME: ifstream
	char* buf = new char[BUFLEN];
	while (fgets(buf, BUFLEN, fi)) {
		TString sbuf(buf);
		if (sbuf.EndsWith("\n"))
			sbuf.Chop();
		fVarDef += sbuf;
		fVarDef += " ";
	}
	fVarDef.Chop();
	fclose(fi);
	delete[] buf;
	return 0;
}

//-----------------------------------------------------------------------------
Int_t LoadNCuts() {
	FILE* fi = fopen(ncutsfile, "r");
	if (!fi) {
		cerr << "Error opening " << ncutsfile << endl;
		return 1;
	}
	if (lDebug)
		cout << "Reading event limt of each cut from file " << ncutsfile << endl;
    
	// FIXME: ifstream
	char* buf = new char[BUFLEN];
    int ipk=0;
	while (fgets(buf, BUFLEN, fi)) {
		TString tbuf(buf);
		if (tbuf.EndsWith("\n"))
			tbuf.Chop();
        Ssiz_t pos = tbuf.First('#');
		if (pos != kNPOS)
			tbuf.Remove(pos);
 		char* cbuf = Compress(tbuf.Data());
        TString sbuf(cbuf);
		delete[] cbuf;
		if (sbuf.IsNull() || sbuf[0] == '#')
			continue;
        fNSieve[ipk++]=atoi(sbuf.Data());
        if (lDebug)
            cout << " event limit for cut " << ipk << " is " << fNSieve[ipk-1] << endl;
	}
	fclose(fi);
	delete[] buf;
	return 0;
}

//-----------------------------------------------------------------------------
void usage() {
	if (!program || !*program)
		program = "tree2ascii";
	char* prg = strdup(program);
	cerr << "Usage: " << basename(prg) << " [-avlpq] "
            << "[-T<treename>] [-n<nread>] [-N<nwrite>] [-S<ncut>]"<< endl
			<< "        [-c<gcutsfile>] [-C<cut>] [-p<peakfile>] [-D<vardef>] " << endl
			<< "        [-d<vardef_file>] [-f<format>] [-o<outfile>] "
			<< "[-O<peakoffset>] " << endl
			<< "        ROOTFILE1 [ROOTFILE2 ...]" << endl;
	cerr << " Copy tree variables from ROOT file "
			<< "to ASCII output file subject to cuts" << endl;
	cerr << " ROOTFILE1 etc.: ROOT input file(s)" << endl;
	cerr << " -a Append to output file" << endl;
	cerr << " -v Verbose messages" << endl;
	cerr << " -l List variables found in tree" << endl;
	cerr << " -p Report progress if outputting to file" << endl;
	cerr << " -q Never output cut numbers" << endl;
	cerr << " -T <treename> Read Tree <treename> in <rootfile> "
			<< "(default: T)" << endl;
	cerr << " -n <nread> Limit on number of events read from input"
			<< "(default: no limit)" << endl;
	cerr << " -N <nwrite> Limit on number of events to write to output "
			<< "after cuts (default: no limit)" << endl;
    cerr << " -S <ncut> Event limit of each cut to write to output "
            << "(default: no limit)" << endl;
    cerr << " -s <ncutsfile> Event limit of each cut to write to output, "
            << "take from file <ncutsfile> (default: none)" << endl;
	cerr << " -g <gcutsfile> Read graphical cuts from ROOT file <gcutsfile> "
			<< "(default: no graphical cuts)" << endl;
	cerr << " -c <cutfile> Read selection cuts from ASCII file <cutfile> "
			<< "(default: none)" << endl;
    cerr << " -C <cut> Read additional cuts "
			<< "(default: none)" << endl;
	cerr << " -D <vardef> Define variables to copy, space-separated list "
			<< "(default: predefined set, use -t to see)" << endl;
	cerr << " -d <vardef_file> Like -D, but take definition of variables "
			<< "from file" << endl;
	cerr << " -f <format> Format for output of each variable, fprintf style "
			<< "(default: 12g)" << endl;
	cerr << " -o <outfile> Write ASCII output to <outfile> "
			<< "(default: stdout)" << endl;
	cerr << " -O <peakoffset> Add offset to all peak numbers "
			<< "(default: 0)" << endl;
	free(prg);
	exit(-1);
}

//-----------------------------------------------------------------------------
void argstr(const char* &str, const char* &opt, int &argc, char** &argv) {
	if (!*++opt && (argc-- < 1 || *(opt = *++argv) == '-'))
		usage();
	str = opt;
	opt = "?";
}

//-----------------------------------------------------------------------------
void getargs(int argc, char** argv) {
	// Parse arguments
	program = *argv;
	bool got_rootfile = false;
	const char* tmp;
	int ival;
	fFiles.Delete();

	while (argc-- > 1) {
		const char *opt = *++argv;
		if (*opt == '-') {
			while (*++opt) {
				switch (*opt) {
				case 'a':
					lAppend = true;
					break;
				case 'v':
					lDebug = true;
					break;
				case 'l':
					lList = true;
					break;
				case 'p':
					lProgr = true;
					break;
				case 'q':
					lQuiet = true;
					break;
				case 'T':
					argstr(treename, opt, argc, argv);
					break;
                case 's':
                    lNCutsFile = true;
                    argstr(ncutsfile, opt, argc, argv );
                    break;
                case 'S':
				case 'n':
				case 'N':
					argstr(tmp, opt, argc, argv);
					ival = atoi(tmp);
					if (ival < 0)
						usage();
					if (*opt == 'n')
						fNev = ival;
					else if (*opt == 'N')
						fNout = ival;
                    else
                        fNSieveAll = ival;
					break;
				case 'g':
					argstr(gcutsfile, opt, argc, argv);
					break;
				case 'c':
					argstr(peakfile, opt, argc, argv);
					break;
				case 'C':
					argstr(tmp, opt, argc, argv);
					fAdditionalCut = tmp;
					break;
				case 'd':
					lDefFile = true;
					//no break
				case 'D':
					argstr(tmp, opt, argc, argv);
					fVarDef = tmp;
					break;
				case 'f':
					argstr(format, opt, argc, argv);
					break;
				case 'o':
					argstr(tmp, opt, argc, argv);
					fOutfile = tmp;
					break;
				case 'O':
					argstr(tmp, opt, argc, argv);
					fPkOffset = atoi(tmp);
					break;
				default:
					usage();
				}
			}
		} else {
			// Parse argument not starting with '-'
			fFiles.Add(new TObjString(*argv));
			got_rootfile = true;
		}
	}
	// ROOT input file required
	if (!got_rootfile)
		usage();
	return;
}

//-----------------------------------------------------------------------------
int main(int argc, char** argv) {
	cout<<"-------------------------------good-----------\n";
	
	static const char rcsid[] =
			"$Id: tree2ascii.C,v 1.6 2006/12/06 15:18:57 ole Exp $";
	TROOT theROOT("tree2ascii", rcsid);//not needed anymore, but heck

	// Parse command line
	getargs(argc, argv);

	if (lDebug)
		cout << "Process with addtional cut (" << fAdditionalCut << "), cut id offset = "<<fPkOffset << endl;
	if (lDefFile && LoadVarDefFile())
		return 1;
	if (fVarDef.IsNull()) {
		if (lDebug)
			cout << "No variables!?! Nothing to do." << endl;
		return 0;
	}
	if (lDebug)
		cout << "Requested variables: " << fVarDef << endl;

	// Load the graphical cuts previously defined and saved in a ROOT file
	if (LoadGCuts())
		return 4;

	// Open event data output file
	FILE* fi = 0;
	if (!lList) {
		if (fOutfile == "-")
			fi = stdout;
		else if (lAppend)
			fi = fopen(fOutfile, "a");
		else
			fi = fopen(fOutfile, "w");
		if (!fi) {
			cerr << "Error opening " << fOutfile
					<< "  (no permission or disk full?)" << endl;
			return 7;
		}
		if (lDebug) {
			cout << "Output to ";
			if (fi == stdout)
				cout << "stdout" << endl;
			else
				cout << "file: " << fOutfile << endl;
		}
	}
	//TFile *ff = TFile::Open("tritium_869.root","READ");
 
	// Loop over all input files
	Int_t ntot = 0;
    Int_t tempn = 0;
   
	TIter next(&fFiles);
	TObjString* rootfile;

	//return 0;
	while ((rootfile = (TObjString*) next())) {
		// Open ROOT file
		cout<<rootfile->GetName();
		
		TFile infile(rootfile->GetName(), "READ");

		infile.Print();
		if (infile.IsZombie())
			continue;
		if (lDebug)
			cout << "Opened ROOT input file " << rootfile->GetName() << endl;

		// Get the Tree
		if (Init() || fChain == 0)
			return 3;

		// Initialize the tree variable descriptors. This involves
		// finding the requested variables in the tree by name.
		if (LoadTreeVariables())
			return 2;

		// If list requested, print it and exit
		if (lList) {
			PrintTreeVariableList();
			break;
		}

		// Parse the definitions of the output variables and
		// mark the requested variables for writing
		if (LoadDefinedVariables())
			return 6;
		//  Reset();

		// Parse peak definition file and create a TTreeFormula for each peak
		if (LoadPeakDefinitions())
			return 5;

        Int_t npeak = fPeaks.GetEntries();
        if(fSieveCount == NULL){
            fSieveCount = new Int_t[npeak];
            for(Int_t i=0;i<npeak;i++)fSieveCount[i]=0;
        }
        if(fNSieve == NULL){
            fNSieve = new Int_t[npeak];
            for(Int_t i=0;i<npeak;i++)fNSieve[i]=fNSieveAll;
            if (lNCutsFile && LoadNCuts())
                return 10;
            for(Int_t i=0;i<npeak;i++)tempn+=fNSieve[i];
            fNout=(tempn>=0&&(fNout>tempn||fNout<0))?tempn:fNout;
        }
        
		Int_t nentries = Int_t(fChain->GetEntriesFast());
        for(Int_t i=0;i<npeak;i++)fNSieve[i]=(fNSieve[i]>=0&&fNSieve[i]<nentries)?fNSieve[i]:nentries;
        Int_t nmax = (fNev >= 0 && fNev < nentries) ? fNev : nentries;
		Int_t nout = (fNout >= 0 && fNout < nentries) ? fNout : nentries;

		if (lDebug)
			cout << "Tree reports " << nentries << " events. "
					<< "Event limits: read: " << nmax << " write: " << nout
					<< endl;

		Int_t ngood = 0;
		// Loop over tree and evaluate peak cuts for each event
		// Write out ASCII file with event data and peak number
		for (Int_t jentry = 0; jentry < nmax && ntot < nout; jentry++) {
			Int_t ientry = LoadTree(jentry);
			if (ientry < 0)
				break;
			fChain->GetEntry(jentry);
			if (fPeaks.IsEmpty()) {
				WriteVariables(fi);
				ngood++;
                ntot++;
			} else {
				TIter next(&fPeaks);
				TTreeFormula* cut;
				int ipk = -1;
				while ((cut = (TTreeFormula*) next())) {
					ipk++;
					// FIXME: ESPACE seems to use the _last_ matching cut. Do we care?
					if (cut->EvalInstance() == 0.0)
						continue;
                    if (fSieveCount[ipk]<fNSieve[ipk]){
                        if (!lQuiet)
                            WriteVariables(fi, ipk + fPkOffset);
                        else
                            WriteVariables(fi);                       
                        fSieveCount[ipk]++;
                        ntot++;
                    }
                    ngood++;
				}
			}
			if (lProgr && fi != stdout && (jentry + 1) % 1000 == 0)
				cout << jentry + 1 << endl;
		}
		//ntot += ngood;
		if (lDebug)
			cout << ngood << " event(s) passed the cuts" << endl;
	}
	if (!lList) {
		if (fi != stdout)
			fclose(fi);
		if (lDebug)
			cout << ntot << " event(s) written in total" << endl;
	}

    delete[] fSieveCount;

	fPeaks.Delete();
	fElements.Delete();
	fFiles.Delete();
	return 0;
}
