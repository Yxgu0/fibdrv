reset
set xlabel 'F(n)'
set ylabel 'time (ns)'
set title 'Fibonacci execution time'
set term png enhanced font 'Verdana, 10'
set output 'plot_clz.png'
set grid

plot [0:92][22:150] \
'plot_statistic_shift63' using 1:3 with linespoints linewidth 2 title '1UL << 63', \
'plot_statistic_shift6' using 1:3 with linespoints linewidth 2 title '1UL << 6', \
'plot_statistic_clz' using 1:3 with linespoints linewidth 2 title '\_\_builtin\_clzll(n)'

