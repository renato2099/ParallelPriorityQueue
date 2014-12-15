set ylabel "Time [s]"
set xlabel "# Threads"
set title "Runtime on Xeon Phi - Fixed  pops/thread"
set yrange[0:500]
set xrange[25:500]
set term post eps color
set output 'runtime_pop.eps'
func1(x) = x / 1000.0
plot "scaleout_pop.dat" using 1:(func1($2)):(func1($3)) title 'Lock-free' with errorlines, \
"scaleout_pop.dat" using 1:(func1($4)):(func1($5)) title 'Lock-based' with errorlines, \
"scaleout_pop.dat" using 1:(func1($6)):(func1($7)) title 'std' with errorlines, \
"scaleout_pop.dat" using 1:(func1($8)):(func1($9)) title 'tbb' with errorlines
