#!/bin/bash 
startRunID=$1
endRunID=$2

for id in $(seq $startRunID 10 $endRunID)
do
    start=$id
    end=$((id + 10))

    ./runVerifier ${start} ${end}
done