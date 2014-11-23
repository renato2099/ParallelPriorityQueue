#/bin/bash
if [ "$#" -ne 4 ]; then
    echo "Usage <method> <threads> <inserts> <fixedPercentage>"
    exit
fi

echo ""
echo "======= COMPILING SEQUENTIAL PPQ ======="
make -C sequential/v2 -f Makefile clean
make -C sequential/v2 -f Makefile main

echo "======= COMPILING LOCK-BASED PPQ ======="
make -C ppq_with_locks -f Makefile clean
make -C ppq_with_locks -f Makefile main

echo "======= COMPILING LOCK-FREE PPQ ======="
make -C ppq_lock_free -f Makefile clean
make -C ppq_lock_free -f Makefile main


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
