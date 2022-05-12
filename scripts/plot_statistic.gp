reset
set xlabel 'F(n)'
set ylabel 'time (ns)'
set title 'Fibonacci execution time'
set term png enhanced font 'Verdana, 10'
set output 'plot_statistic.png'
set grid

# setting x and y with range 0~92 and 0~200
# using column 1 in dataset as x-axis, column 2 and 3 as y-axis
plot [0:92][0:200] \
'plot_statistic' using 1:2 with linespoints linewidth 2 title 'recursion w/ cache', \
'' using 1:3 with linespoints linewidth 2 title 'fast doubling w/o clz'

