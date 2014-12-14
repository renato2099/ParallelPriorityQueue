set ylabel "Time [s]"
set xlabel "# Threads"
set title "Runtime on Xeon Phi - 10Mio. inserts"
set yrange[0:300]
set xrange[25:500]
set term post eps color
set output 'runtime_insert.eps'
func1(x) = x / 1000.0
plot "speedup_insert.dat" using 1:(func1($2)) title 'Lock-free' with linespoints, \
"speedup_insert.dat" using 1:(func1($4)) title 'Lock-based' with linespoints, \
"speedup_insert.dat" using 1:(func1($6)) title 'std' with linespoints, \
"speedup_insert.dat" using 1:(func1($8)) title 'tbb' with linespoints
