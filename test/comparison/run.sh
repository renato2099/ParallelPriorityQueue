#/bin/bash
if [ "$#" -ne 4 ]; then
    echo "Usage <method> <threads> <inserts> <fixedPercentage>"
    exit
fi

echo ""
echo "======= COMPILING SEQUENTIAL PPQ ======="
make -C ../../sequential/v2 -f Makefile clean
make -C ../../sequential/v2 -f Makefile main

echo "======= COMPILING LOCK-BASED PPQ ======="
make -C ../../ppq_with_locks -f Makefile clean
make -C ../../ppq_with_locks -f Makefile main

echo "======= COMPILING LOCK-FREE PPQ ======="
make -C ../../ppq_lock_free -f Makefile clean
make -C ../../ppq_lock_free -f Makefile main

# sequential
#./../../ppq_lock_free/main -p -t3 -i10000 -f0.1

#lock based
./../../ppq_with_locks/main -p -t$2 -i$3 -f$4

#lock-free

./../../ppq_lock_free/main -p -t$2 -i$3 -f$4