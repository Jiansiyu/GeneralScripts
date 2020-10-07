#!/bin/bash

END=30000

mv runList.txt runList_save.txt
touch runList.txt

for i in $(seq 1 $END);
do
  if [ -f "beamE/RHRS_${i}_BeamE.txt" ]; then
    ls "beamE/RHRS_${i}_BeamE.txt"
    echo "${i}" >> runList.txt
  fi

done