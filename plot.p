#!/usr/bin/gnuplot

set logscale y 10

set term qt 0
plot "model.dat" using 1:2 title "Target" with lines, \
     "model.dat" using 1:3 title "Infected" with lines, \
     "model.dat" using 1:4 title "Viral" with lines

set term qt 1
plot "model.dat" using 6:7 title "Target" with lines, \
     "model.dat" using 6:8 title "Infected" with lines, \
     "model.dat" using 6:9 title "Viral" with lines

set term qt 2
plot "model.dat" using 11:12 title "Target" with lines, \
     "model.dat" using 11:13 title "Infected" with lines, \
     "model.dat" using 11:14 title "Viral" with lines

set term qt 3
plot "model.dat" using 1:5 title "Environment 0" with lines, \
     "model.dat" using 1:10 title "Environment 1" with lines, \
     "model.dat" using 1:15 title "Environment 50" with lines

unset logscale y

set term qt 4
plot "model.dat" using 1:16 title "Susceptible" with lines, \
     "model.dat" using 1:17 title "Infected" with lines

pause -1
