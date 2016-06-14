#!/usr/bin/gnuplot

reset

#set terminal x11 size 600,600 enhanced font 'Verdana,10' persist

set terminal pngcairo size 500,500 enhanced font 'Verdana,10'
set output 'benchmark.png'

set border linewidth 1
set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5 # --- blue
set style line 2 lc rgb '#dd181f' lt 1 lw 2 pt 5 ps 1.5 # --- red

set key right bottom

set xtics 1
set ytics 10
set tics scale 0.75

set xlabel "cores"
set ylabel "win %"

set xrange [0.5:12.5]
set yrange [0:100]

plot 'plot.dat' index 0 with linespoints ls 1 title "single", \
     ''         index 1 with linespoints ls 2 title "multi"
