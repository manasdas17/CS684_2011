set term postscript eps enhanced 
set output "simulation.eps"
set xrange [-7:7]
set xtics 1
set grid
set yrange [-7:6]
set key 450, 700
set ytics 1
set xlabel "x-axis"
set ylabel "y-axis"
plot 'ls2438.txt' u 1:2 t 'red' with points lt 0 lc 1 pt 7
