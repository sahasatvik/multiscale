#!/usr/bin/gnuplot

set logscale y 10

plot "output/oneagent.dat" using 1:2 title "Target" with lines, \
     "output/oneagent.dat" using 1:3 title "Infected" with lines, \
     "output/oneagent.dat" using 1:4 title "Viral" with lines, \
     "output/oneagent.dat" using 1:5 title "Antibodies" with lines

pause -1
