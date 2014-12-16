set ylabel "Speedup (base: T(30))"
set xlabel "# Threads"
set title "Speedup on Xeon Phi - 10Mio. operations (push/pop)"
set yrange[0:3]
set xrange[25:500]
set logscale x 2
set term post eps color
set output 'speedup_mixed.eps'
func1(x) = x / 1000.0
plot "speedup_mixed.dat" using 1:((33597/$2)):xtic(1) title 'Lock-free' with linespoints, \
"speedup_mixed.dat" using 1:((133162/$4)) title 'Lock-based' with linespoints, \
"speedup_mixed.dat" using 1:((82942/$6)) title 'std' with linespoints, \
"speedup_mixed.dat" using 1:((19954/$8)) title 'tbb' with linespoints
