#!/usr/bin/env sh

RUNS_START=$1
RUNS_END=$2

for i in $(seq -w $RUNS_START $RUNS_END); do
        ./model
        mv output/countdata.dat "output/countdata_${i}.dat"
        mv output/averagedata.dat "output/averagedata_${i}.dat"
        mv output/agentdata.dat "output/agentdata_${i}.dat"
        mv output/environmentdata.dat "output/environmentdata_${i}.dat"
        echo "Run $i/$RUNS_END"
done
