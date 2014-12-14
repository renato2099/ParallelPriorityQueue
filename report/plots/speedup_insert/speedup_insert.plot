set ylabel "Time [s]"
set xlabel "# Threads"
set title "Speedup on Xeon Phi - 10Mio. inserts"
set yrange[0:3]
set xrange[25:500]
set term post eps color
set output 'speedup_insert.eps'
func1(x) = x / 1000.0
plot "speedup_insert.dat" using 1:(($2/62847)) title 'Lock-free' with linespoints, \
"speedup_insert.dat" using 1:(($4/269591)) title 'Lock-based' with linespoints, \
"speedup_insert.dat" using 1:(($6/43463)) title 'std' with linespoints, \
"speedup_insert.dat" using 1:(($8/9479)) title 'tbb' with linespoints
