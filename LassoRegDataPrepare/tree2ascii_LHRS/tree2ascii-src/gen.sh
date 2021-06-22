#!/bin/bash

LIST=$1      #
nfoil=$2     # only one foil for the prex target
((nfoil=nfoil-1))
N=100
echo $LIST ${nfoil}
for run in $LIST
do
  echo $run 
  : >tritium_$run.SieveCut.cut
  : >tritium_$run.SieveCut.ncut
  for i in $(seq 0 ${nfoil}) #foil id
  do
    for j in $(seq 0 26) #col, use 27 total foils instead of 25 to put no.13 in the middle
    do
      name=$(echo -e "tritium_$run.SieveCut.$i"_"$j.cut") #col
      if [[ -e "$name" ]]; then
        while read l
        do
          echo -e "$l" >>tritium_$run.SieveCut.cut
          if [[ "$l" == "fEvtHdr.fRun==0" ]];then
            echo -e "0" >>tritium_$run.SieveCut.ncut 
          else
            echo -e "$N" >>tritium_$run.SieveCut.ncut
          fi
        done < $name
      else
        for k in $(seq 0 10) #holes
        do
          echo -e "fEvtHdr.fRun==0" >>tritium_$run.SieveCut.cut
          echo -e "0" >>tritium_$run.SieveCut.ncut
        done
      fi
      echo -e  "" >>tritium_$run.SieveCut.cut 
      echo -e  "" >>tritium_$run.SieveCut.ncut
    done
    echo -e  "" >>tritium_$run.SieveCut.cut 
    echo -e  "" >>tritium_$run.SieveCut.ncut
  done
done
