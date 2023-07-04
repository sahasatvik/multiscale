#!/usr/bin/gnuplot

plot for [i=1:100] sprintf("output/p8/countdata_%03d.dat", i) using 1:3 with lines lw 2 lc rgb '#DD000000' notitle, \
     for [i=1:100] sprintf("output/p6/countdata_%03d.dat", i) using 1:3 with lines lw 2 lc rgb '#DDFF0000' notitle, \
     for [i=1:100] sprintf("output/p4/countdata_%03d.dat", i) using 1:3 with lines lw 2 lc rgb '#DD00FF00' notitle, \
     for [i=1:100] sprintf("output/p2/countdata_%03d.dat", i) using 1:3 with lines lw 2 lc rgb '#DD0000FF' notitle

pause -1
