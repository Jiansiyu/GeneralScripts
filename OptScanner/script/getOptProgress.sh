#!/bin/bash

function show_time () {
    num=$1
    min=0
    hour=0
    day=0
    if((num>59));then
        ((sec=num%60))
        ((num=num/60))
        if((num>59));then
            ((min=num%60))
            ((num=num/60))
            if((num>23));then
                ((hour=num%24))
                ((day=num/24))
            else
                ((hour=num))
            fi
        else
            ((min=num))
        fi
    else
        ((sec=num))
    fi
    echo "$day":"$hour":"$min":"$sec"s
}

FolderPath=$1

# get the time stamp of the first file in the folder
datetime0=$(find ${FolderPath} -type d -printf '%TY-%Tm-%Td %TH:%TM:%TS \n' | sort | head -n 1)
timestamp0=$(date -d "${datetime0}" +%s)
#timestamp1=$(date +"%s")
#echo $timestamp0
#echo $timestamp1
#echo $(show_time $(($timestamp1-$timestamp0))) 
#exit
pstr="[=======================================================================]"

TotalFileNumber=$(ls ${FolderPath} | wc -l)
echo "Total Files need to Process : ${TotalFileNumber}"
echo "Current progress :"

CurrentReplayedNumber=$(ls ${FolderPath}/*/templateDB.db.Theta.Phi.optimied | wc -l) 

while [ ${TotalFileNumber} -gt ${CurrentReplayedNumber} ];
do 
	timestamp1=$(date +"%s")
	str='--:--:--'

	pd=$(( ${CurrentReplayedNumber} * 73 / ${TotalFileNumber} ))
	
	if [ ! "${CurrentReplayedNumber}" -eq "0" ]; then
   		str=$(($timestamp1-$timestamp0))
		timeRemain=$(( ${str} / ${pd} ))
		timeUsedTimeStamp=$(show_time ${str})
		timeRemainTimeStamp=$(show_time $((${timeRemain}*73)))
	fi

	printf "\r%3d.%1d%% %s/%s %.${pd}s "  $(( ${CurrentReplayedNumber} * 100 / ${TotalFileNumber} )) $(( (${CurrentReplayedNumber} * 10000 / ${TotalFileNumber}) % 100 )) ${timeUsedTimeStamp} ${timeRemainTimeStamp} $pstr 
	sleep 1
	CurrentReplayedNumber=$(ls ${FolderPath}/*/templateDB.db.Theta.Phi.optimied | wc -l)
done 

# post run process

CurrentReplayedNumber=$(ls ${FolderPath}/*/templateDB.db.Theta.Phi.optimied | wc -l)
pd=$(( ${CurrentReplayedNumber} * 73 / ${TotalFileNumber} ))
printf "\r%3d.%1d%% %.${pd}s"  $(( ${CurrentReplayedNumber} * 100 / ${TotalFileNumber} )) $(( (${CurrentReplayedNumber} * 10000 / ${TotalFileNumber}) % 100 ))  $pstr

echo 
echo "Process Done !!!"
