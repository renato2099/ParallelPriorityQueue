set style line 100 lt 2 lc rgb "gray" lw 1
set style line 101 lt 1 lc rgb "gray" lw 1

# first draw the minor tics
#set xrange [0:1]
#set mxtics 10
#set yrange [0:1]
#set mytics 5
#set grid mxtics mytics ls 101

# then the main tics
#dx=0.25  #grid spacing in x
#set for [i=1:3] arrow from graph i*dx,graph 0 to graph i*dx,graph 1 nohead front ls 100
#dy=0.1425  #grid spacing in y
#set for [i=1:6] arrow from graph 0,graph i*dy to graph 1,graph i*dy nohead front ls 100


set ylabel "Runtime (msecs)"
set xlabel "Memory allocation types"
set title "Memory allocation runtime x10mio elements"
set term postscript color
set output '| ps2pdf - mem_alloc.pdf'

set style line 1 lc rgb "red"
set style line 2 lc rgb "blue"
set style line 3 lc rgb "green"
set size 0.8, 1
set xrange [-1:3]
set style fill solid
set boxwidth 0.45

plot "mem_alloc.dat" every ::0::0 using 1:3:xtic(2) title "Regular alloc." with boxes ls 1, \
     "mem_alloc.dat" every ::1::1 using 1:3:xtic(2) title "TBB alloc." with boxes ls 2, \
     "mem_alloc.dat" every ::2::2 using 1:3:xtic(2) title "TBB scalable alloc." with boxes ls 3


