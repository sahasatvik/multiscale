#!/usr/bin/gnuplot

set title "Probability of reinfection"
set xlabel "Delay"
plot [] [0:1] "output/reinfection.dat" using 1:2 title "" with lines

pause -1
