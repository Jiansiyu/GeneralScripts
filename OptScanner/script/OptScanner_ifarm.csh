#! /bin/csh
source /u/home/siyuj/env_set/setPRex.csh
# template bash script used for scan single folder
# input: $2 target folder than contains the databse template subfolder
# input: $1 the source folder of the opt code, the code will go into the code, and start the scripts
# author : siyu  jiansiyu@gmail.com, sj9va@virginia.edu

set RED=`tput setaf 1`
set GREEN=`tput setaf 2`
set NC=`tput sgr0`

if ($#argv != 2)  then
    echo "${RED}[ERROR] ${NC}Illegal number of parameters"
    echo "./optScanner.sh [OptSourcePath] [Target path(contains the db template)]"
    exit
endif

set RunOptFolder=$argv[1]  # the folder of the optimization code
set TargetFolder=$argv[2]  # the folder that contains the

#create the template name and check the existance of the template of the database template
set TemplateFname=${TargetFolder}/templateDB.db
set OptimizedThetaDBFname=${TargetFolder}/templateDB.db.Theta.optimied
set OptimizedPhiDBFname=${TargetFolder}/templateDB.db.Theta.Phi.optimied
set OptimizedYDBFname=${TargetFolder}/templateDB.db.Theta.Phi.Y.optimied
set OptimizedDpDBFname=${TargetFolder}/templateDB.db.Theta.Phi.Y.Dp.optimied

set OptimizedDBFname=${TargetFolder}/templateDB.db.optimied
set OptimizedDBFname_postrun=${TargetFolder}/templateDB.db.optimied_junk

if (! -d ${TargetFolder} ) then
  echo "${RED}[ERROR] ${NC} Path NOT Found ${TargetFolder}"
    exit
endif

if (! -d ${RunOptFolder} ) then
  echo "${RED}[ERROR] ${NC} Path NOT Found ${RunOptFolder}"
    exit
endif

if ( ! -f "${TemplateFname}" ) then
    echo "${RED}[ERROR] ${NC} Path NOT Found ${TemplateFname}"
    exit
endif

echo "go into folder ${RunOptFolder}"
cd ${RunOptFolder}

#set optScriptBranch=$(git branch | sed -n -e 's/^\* \(.*\)/\1/p')

#echo "Opt script branch name: ${optScriptBranch}"

echo "Start the Theta and Phi Optimization!!"
analyzer -b -q .L rootlogon.C 'ROpticsOptScript.C('true',"theta","'${TemplateFname}'","'${OptimizedThetaDBFname}'")'
analyzer -b -q .L rootlogon.C 'ROpticsOptScript.C('true',"phi","'${OptimizedThetaDBFname}'","'${OptimizedPhiDBFname}'")'
analyzer -b -q .L rootlogon.C 'ROpticsOptScript.C('false',"phi","'${OptimizedPhiDBFname}'","'${OptimizedDBFname}'")'