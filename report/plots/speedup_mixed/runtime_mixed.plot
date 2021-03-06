set ylabel "Time [s]"
set xlabel "# Threads"
set title "Runtime on Xeon Phi - 10Mio. operations (push/pop)"
set yrange[0:500]
set xrange[25:500]
set logscale x 2
set term post eps color
set output 'runtime_mixed.eps'
func1(x) = x / 1000.0
plot "speedup_mixed.dat" using 1:(func1($2)):(func1($3)):xtic(1) title 'Lock-free' with errorlines, \
"speedup_mixed.dat" using 1:(func1($4)):(func1($5)) title 'Lock-based' with errorlines, \
"speedup_mixed.dat" using 1:(func1($6)):(func1($7)) title 'std' with errorlines, \
"speedup_mixed.dat" using 1:(func1($8)):(func1($9)) title 'tbb' with errorlines
