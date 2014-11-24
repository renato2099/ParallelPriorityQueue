#/bin/bash

clean_up(){
    echo "===== CLEAN UP ====="
    make -C ../../ppq_std -f Makefile clean 
    make -C ../../sequential/v2 -f Makefile clean
    make -C ../../ppq_with_locks -f Makefile clean
    make -C ../../ppq_lock_free -f Makefile clean
}

if [ "$#" -eq 1 ]; then
    if [ $1 == "clean" ]; then
        clean_up
    fi
fi

if [ "$#" -ne 4 ]; then
    echo "Usage <method> <threads> <inserts> <fixedPercentage>"
    exit
fi

clean_up
echo ""
echo "======= COMPILING STD PPQ ======="
make -C ../../ppq_std -f Makefile main

echo "======= COMPILING SEQUENTIAL PPQ ======="
make -C ../../sequential/v2 -f Makefile main

echo "======= COMPILING LOCK-BASED PPQ ======="
make -C ../../ppq_with_locks -f Makefile main

echo "======= COMPILING LOCK-FREE PPQ ======="
make -C ../../ppq_lock_free -f Makefile main

# std
./../../ppq_std/main -p -t$2 -i$3 -f$4

# sequential
#./../../ppq_lock_free/main -p -t3 -i10000 -f0.1

#lock based
./../../ppq_with_locks/main -p -t$2 -i$3 -f$4

#lock-free

./../../ppq_lock_free/main -p -t$2 -i$3 -f$4
