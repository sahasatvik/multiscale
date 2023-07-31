#!/usr/bin/env sh

RUNS_START=$1
RUNS_END=$2
P_INFECT=(2 3 4 5 6 8)

for p in "${P_INFECT[@]}"; do
        mkdir -p "output/p$p"
        for i in $(seq -w $RUNS_START $RUNS_END); do
                ./model -p ${p}e-3
                mv output/countdata.dat "output/p$p/countdata_${i}.dat"
                mv output/averagedata.dat "output/p$p/averagedata_${i}.dat"
                mv output/agentdata.dat "output/p$p/agentdata_${i}.dat"
                mv output/environmentdata.dat "output/p$p/environmentdata_${i}.dat"
                "Run $i/$RUNS_END"
        done
done
