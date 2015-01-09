set grid
set ylabel ""
set xlabel "Number of Insert Operations"
set title "Operational Intensity - Intel Haswell vs Intel IvyBridge"
set decimal locale
set format x "%'g"
set log x
set term post eps color
set output 'haswell_ivybridge.eps'
plot "haswell_ivybridge.dat" using 1:2 title '16Thrs-Haswell' with errorlines, \
"haswell_ivybridge.dat" using 1:3 title '32Thrs-Haswell' with errorlines, \
"haswell_ivybridge.dat" using 1:4 title '16Thrs-IvyBridge' with errorlines, \
"haswell_ivybridge.dat" using 1:5 title '32Thrs-IvyBridge' with errorlines