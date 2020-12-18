{
#include <TError.h>
 gStyle->SetPalette(1);

 gSystem->AddIncludePath("-I$ANALYZER/src/Podd");
 gInterpreter->AddIncludePath("$ANALYZER/src/Podd");

 gSystem->AddIncludePath("-I$ANALYZER/src/HallA");
 gInterpreter->AddIncludePath("$ANALYZER/src/HallA");

 gSystem->AddIncludePath("-I$ANALYZER/src/hana_decode");
 gInterpreter->AddIncludePath("$ANALYZER/src/hana_decode");

 gROOT->LoadMacro("vdcConstOpt.C");
// gROOT->LoadMacro("ROpticsOptScript.C");
//gErrorIgnoreLevel = kFatal;
}

