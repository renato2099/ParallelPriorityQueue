#/bin/bash
if [ "$#" -ne 2 ]; then
    echo "Usage <check|compare> <(1)MinHeap(2)Std(3)Lock(4)Lock-free|(5)All>"
    exit
fi

if [ "$1" -eq "check" ]; then
    echo "check"
    exit
fi

echo ""
echo "=== COMPILING ==="
make Makefile clean
make Makefile main

if [ "$1" == "check" ]; then
    echo ""
    echo "=== CORRECTNESS ==="
    if [ "$2" -eq 1 ]; then
        ./main -t1 -c -v
    fi
    if [ "$2" -eq 2 ]; then
        ./main -t2 -c -v
    fi
    if [ "$2" -eq 3 ]; then
        ./main -t3 -c -v
    fi
    if [ "$2" -eq 4 ]; then
        ./main -t4 -c -v
    fi
    if [ "$2" -eq 5 ]; then
        O_MIN="$(./main -t1 -c -v | wc -l)"
        O_STD="$(./main -t2 -c -v | wc -l)"
        if [ $O_MIN -eq $O_STD ]; then
            echo "STD ..............[OK]"
        else
            echo "STD ..............[FAILED]"
        fi
        O_LOCK="$(./main -t3 -c -v | wc -l)"
        if [ $O_MIN -eq $O_LOCK ]; then
            echo "LOCK_BASED PPQ ...[OK]"
        else
            echo "LOCK_BASED PPQ ...[FAILED]"
        fi
        O_LOCKFREE="$(./main -t4 -c -v | wc -l)"
        if [ $O_MIN -eq $O_LOCKFREE ]; then
            echo "LOCK_FREE PPQ ....[OK]"
        else
            echo "LOCK_FREE PPQ ....[FAILED]"
        fi
    fi
    exit
fi

if [ "$1" == "compare" ]; then
    echo ""
    echo "=== CHECK PERFORMANCE ==="
    if [ "$2" -eq 1 ]; then
        ./main -t1
    fi
    if [ "$2" -eq 2 ]; then
        ./main -t2
    fi
    if [ "$2" -eq 3 ]; then
        ./main -t3
    fi
    if [ "$2" -eq 4 ]; then
        ./main -t4
    fi
    if [ "$2" -eq 5 ]; then
        ./main -t1
        ./main -t2 
        ./main -t3 
        ./main -t4 
    fi
    exit
fi
