#!/usr/bin/gnuplot

set datafile separator ","
set key autotitle columnheader
set logscale y 10

set term qt 0
set title "Agent 1"
plot for [i=2:6] "output/agentdata.dat" using 1:i with lines

set term qt 1
set title "Agent 2"
plot for [i=7:11] "output/agentdata.dat" using 1:i with lines

set term qt 2
set title "Agent 3"
plot for [i=12:16] "output/agentdata.dat" using 1:i with lines

set term qt 3
set title "Averages across all agents"
plot for [i=2:5] "output/averagedata.dat" using 1:i with lines


unset logscale y

set term qt 4
set title "Number of individuals in stage"
plot for [i=2:4] "output/countdata.dat" using 1:i with lines


pause -1
