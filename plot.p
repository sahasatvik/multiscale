#!/usr/bin/gnuplot

set logscale y 10

set term qt 0
set title "Agent 1"
plot "agentdata.dat" using 1:2 title "Target" with lines, \
     "agentdata.dat" using 1:3 title "Infected" with lines, \
     "agentdata.dat" using 1:4 title "Viral" with lines, \
     "agentdata.dat" using 1:5 title "Viral (External)" with lines, \
     "agentdata.dat" using 1:6 title "Antibodies" with lines

set term qt 1
set title "Agent 2"
plot "agentdata.dat" using 1:7 title "Target" with lines, \
     "agentdata.dat" using 1:8 title "Infected" with lines, \
     "agentdata.dat" using 1:9 title "Viral" with lines, \
     "agentdata.dat" using 1:10 title "Viral (External)" with lines, \
     "agentdata.dat" using 1:11 title "Antibodies" with lines

set term qt 2
set title "Agent 3"
plot "agentdata.dat" using 1:12 title "Target" with lines, \
     "agentdata.dat" using 1:13 title "Infected" with lines, \
     "agentdata.dat" using 1:14 title "Viral" with lines, \
     "agentdata.dat" using 1:15 title "Viral (External)" with lines, \
     "agentdata.dat" using 1:16 title "Antibodies" with lines

set term qt 3
set title "Averages across all agents"
plot "averagedata.dat" using 1:2 title "Target" with lines, \
     "averagedata.dat" using 1:3 title "Infected" with lines, \
     "averagedata.dat" using 1:4 title "Viral" with lines, \
     "averagedata.dat" using 1:5 title "Antibodies" with lines


unset logscale y

set term qt 4
set title "Number of individuals in stage"
plot "countdata.dat" using 1:2 title "Susceptible" with lines, \
     "countdata.dat" using 1:3 title "Infected" with lines, \
     "countdata.dat" using 1:4 title "Recovered" with lines


pause -1
