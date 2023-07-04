#!/usr/bin/env sh

RUNS=100

for i in $(seq -w 1 $RUNS); do
        ./model 2> /dev/null
        mv output/countdata.dat "output/countdata_${i}.dat"
        mv output/averagedata.dat "output/averagedata_${i}.dat"
        echo "Run $i/$RUNS"
done
