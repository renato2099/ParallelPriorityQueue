#/bin/bash

DEF_METHOD="pop"
DEF_THREADS=1
DEF_INSERTS=10
DEF_FIXED=1
COMP=0
CL=0

clean_up(){
    echo "===== CLEAN UP ====="
    make -C ../../ppq_std -f Makefile clean 
    make -C ../../sequential/v2 -f Makefile clean
    make -C ../../ppq_with_locks -f Makefile clean
    make -C ../../ppq_lock_free -f Makefile clean
}

compile(){
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
}

while getopts ":r :c :m: :t: :f: :i: :h" opt; do
   case $opt in
        c)
            COMP=1
            ;;
        r)
            CL=1 
            ;;
        m)
            METHOD=${OPTARG} 
            ;;
        t)
            THREADS=${OPTARG} 
            ;;
        f)
            FIXED=${OPTARG} 
            ;;
        i)
            INSERTS=${OPTARG} 
            ;;
       \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
        h)
            echo "-c, To compile or not to compile"
            echo "-r, To run make clean over all codes"
            echo "-t, Define number of threads"
            echo "-i, Define number of insert operations"
            echo "-f, Define percentage of fixed insert operations"
            exit 0
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit 1
          ;;
   esac
done

if [ -z $THREADS ]; then
    echo "Using DEFAULT THREADS ", ${DEF_THREADS}
    THREADS=${DEF_THREADS}
fi

if [ -z $FIXED ]; then
    echo "Using DEFAULT FIXED PERCENTAGE ", ${DEF_FIXED}
    FIXED=${DEF_FIXED}
fi

if [ -z $INSERTS ]; then
    echo "Using DEFAULT INSERTS ", ${DEF_INSERTS}
    INSERTS=${DEF_INSERTS}
fi

if [ ${CL} -eq 1 ]; then
    clean_up
    exit 0
fi

if [ ${COMP} -eq 1 ]; then
    compile
fi

# std
#./../../ppq_std/main -p -t${THREADS} -i${INSERTS} -f${FIXED}

# sequential
#./../../ppq_lock_free/main -p -t${THREADS} -i${INSERTS} -f${FIXED}

#lock based
./../../ppq_with_locks/main -p -t${THREADS} -i${INSERTS} -f${FIXED}

#lock-free
./../../ppq_lock_free/main -p -t${THREADS} -i${INSERTS} -f${FIXED}
