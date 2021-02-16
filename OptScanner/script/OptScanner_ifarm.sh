#!/bin/bash
# template bash script used for scan single folder
# input: $2 target folder than contains the databse template subfolder
# input: $1 the source folder of the opt code, the code will go into the code, and start the scripts
# author : siyu  jiansiyu@gmail.com, sj9va@virginia.edu

RED=`tput setaf 1`
GREEN=`tput setaf 2`
NC=`tput sgr0`

if [ "$#" -ne 2 ]; then
    echo "${RED}[ERROR] ${NC}Illegal number of parameters"
    echo "./optScanner.sh [OptSourcePath] [Target path(contains the db template)]"
    exit
fi

RunOptFolder=$1  # the folder of the optimization code
TargetFolder=$2  # the folder that contains the

#create the template name and check the existance of the template of the database template
TemplateFname=${TargetFolder}/templateDB.db
OptimizedThetaDBFname=${TargetFolder}/templateDB.db.Theta.optimied
OptimizedPhiDBFname=${TargetFolder}/templateDB.db.Theta.Phi.optimied
OptimizedYDBFname=${TargetFolder}/templateDB.db.Theta.Phi.Y.optimied
OptimizedDpDBFname=${TargetFolder}/templateDB.db.Theta.Phi.Y.Dp.optimied

OptimizedDBFname=${TargetFolder}/templateDB.db.optimied
OptimizedDBFname_postrun=${TargetFolder}/templateDB.db.optimied_junk

# check the existance of the folders
if [ ! -d "${TargetFolder}" ]; then
    echo "${RED}[ERROR] ${NC} Path NOT Found ${TargetFolder}"
    exit
else
    echo"${GREEN} LOG:${NC}Target Folder:${TargetFolder}"
fi

if [ ! -d "${RunOptFolder}" ]; then
    echo "${RED}[ERROR] ${NC} Path NOT Found ${RunOptFolder}"
    exit
else
    echo echo"${GREEN} LOG:${NC}Opt Source Folder:${OptSourcePath}"
fi
if [ ! -f "${TemplateFname}" ]; then
    echo "${RED}[ERROR] ${NC} Path NOT Found ${TemplateFname}"
    exit
else
    echo  "${GREEN} LOG:${NC} Using database ${TemplateFname}"
fi


echo "go into folder ${RunOptFolder}"
cd ${RunOptFolder}

optScriptBranch=$(git branch | sed -n -e 's/^\* \(.*\)/\1/p')

echo "Opt script branch name: ${optScriptBranch}"

echo "Start the Theta and Phi Optimization!!"
analyzer -b -q .L rootlogon.C 'ROpticsOptScript.C('true',"theta","'${TemplateFname}'","'${OptimizedThetaDBFname}'")'
analyzer -b -q .L rootlogon.C 'ROpticsOptScript.C('true',"phi","'${OptimizedThetaDBFname}'","'${OptimizedPhiDBFname}'")'
analyzer -b -q .L rootlogon.C 'ROpticsOptScript.C('false',"phi","'${OptimizedPhiDBFname}'","'${OptimizedDBFname}'")'
