set ylabel "Speedup (base: T(30))"
set xlabel "# Threads"
set title "Speedup on Xeon Phi - 10Mio. pop operations"
set yrange[0:3]
set xrange[25:500]
set logscale x 2
set term post eps color
set output 'speedup_pop.eps'
func1(x) = x / 1000.0
plot "speedup_pop.dat" using 1:((39875/$2)):xtic(1) title 'Lock-free' with linespoints, \
"speedup_pop.dat" using 1:((102306/$4)) title 'Lock-based' with linespoints, \
"speedup_pop.dat" using 1:((136853/$6)) title 'std-based' with linespoints, \
"speedup_pop.dat" using 1:((65592/$8)) title 'tbb' with linespoints
