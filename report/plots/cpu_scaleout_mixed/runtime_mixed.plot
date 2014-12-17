set ylabel "Time [s]"
set xlabel "# Threads"
set title "Runtime on CPU - Fixed  operations/thread (pop/push)"
set yrange[0:500]
set xrange[1:128]
set logscale x 2
set term post eps color
set output 'runtime_mixed.eps'
func1(x) = x / 1000.0
plot "scaleout_mixed.dat" using 1:(func1($2)):xtic(1) title 'Lock-free' with errorlines, \
"scaleout_mixed.dat" using 1:(func1($3)) title 'Lock-based' with errorlines, \
"scaleout_mixed.dat" using 1:(func1($4)) title 'std' with errorlines, \
"scaleout_mixed.dat" using 1:(func1($5)) title 'tbb' with errorlines
