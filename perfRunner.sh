#!/bin/bash
OPS=1
IMPL=1
while [  $IMPL -lt 5 ]; do
	COUNTER=0
	while [  $COUNTER -lt 7 ]; do
		let OPS=$(awk "BEGIN{print 10 ** $COUNTER}")
		CNT=0
		while [ $CNT -lt 5 ]; do
			perf stat -B -e cache-misses -e instructions -e branch-misses ./main -t1 -c1 -p0 -o${OPS} -l${IMPL} -i1 &>> ${IMPL}.out
			let CNT=CNT+1
		done
		let COUNTER=COUNTER+1
	done
	let IMPL=IMPL+1
done
