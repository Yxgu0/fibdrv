reset
set xlabel 'F(n)'
set ylabel 'time (ns)'
set title 'Fibonacci execution time'
set term png enhanced font 'Verdana, 10'
set output 'plot_iter_fast_bn.png'
set grid

# Setting x and y with range 0~100 and 0~11000
plot [0:100][0:16000] \
'plot_iter_fast_bn' using 1:2 with linespoints linewidth 2 title 'bignum iteration w/ cache', \
'' using 1:3 with linespoints linewidth 2 title 'bignum fast doubling w/o clz'

