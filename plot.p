#!/usr/bin/gnuplot

set logscale y 10

set term qt 0
plot "model.dat" using 1:2 title "Target" with lines, \
     "model.dat" using 1:3 title "Infected" with lines, \
     "model.dat" using 1:4 title "Viral" with lines, \
     "model.dat" using 1:5 title "Viral (External)" with lines, \
     "model.dat" using 1:6 title "Antibodies" with lines

set term qt 1
plot "model.dat" using 8:9 title "Target" with lines, \
     "model.dat" using 8:10 title "Infected" with lines, \
     "model.dat" using 8:11 title "Viral" with lines, \
     "model.dat" using 8:12 title "Viral (External)" with lines, \
     "model.dat" using 8:13 title "Antibodies" with lines

set term qt 2
plot "model.dat" using 15:16 title "Target" with lines, \
     "model.dat" using 15:17 title "Infected" with lines, \
     "model.dat" using 15:18 title "Viral" with lines, \
     "model.dat" using 15:19 title "Viral (External)" with lines, \
     "model.dat" using 15:20 title "Antibodies" with lines

set term qt 3
plot "model.dat" using 1:7 title "Environment 0" with lines, \
     "model.dat" using 1:14 title "Environment 1" with lines, \
     "model.dat" using 1:21 title "Environment 50" with lines

unset logscale y

set term qt 4
plot "model.dat" using 1:22 title "Susceptible" with lines, \
     "model.dat" using 1:23 title "Infected" with lines, \
     "model.dat" using 1:24 title "Recovered" with lines

set term qt 5
plot "model.dat" using 1:($3/$2) title "Infected / Target 0" with lines, \
     "model.dat" using 1:($10/$9) title "Infected / Target 1" with lines, \
     "model.dat" using 1:($17/$16) title "Infected / Target 50" with lines

pause -1
