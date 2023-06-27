#!/usr/bin/gnuplot

set logscale y 10

plot "output/reinfection_runs.dat" using 1:2 title "Target" pt 0, \
     "output/reinfection_runs.dat" using 1:3 title "Infected" pt 0, \
     "output/reinfection_runs.dat" using 1:4 title "Viral" pt 0, \
     "output/reinfection_runs.dat" using 1:5 title "Antibodies" pt 0

pause -1
