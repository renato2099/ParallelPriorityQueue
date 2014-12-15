set ylabel "Speedup (base 30 threads)"
set xlabel "# Threads"
set title "Speedup on Xeon Phi - 10Mio. pushs"
set yrange[0:3]
set xrange[25:500]
set term post eps color
set output 'speedup_insert.eps'
func1(x) = x / 1000.0
plot "speedup_insert.dat" using 1:((63536/$2)) title 'Lock-free' with linespoints, \
"speedup_insert.dat" using 1:((246226/$4)) title 'Lock-based' with linespoints, \
"speedup_insert.dat" using 1:((45956/$6)) title 'std' with linespoints, \
"speedup_insert.dat" using 1:((9533/$8)) title 'tbb' with linespoints
