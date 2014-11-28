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
    make -C ../../ppq_tbb -f Makefile clean
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

    echo "======= COMPILING TBB PPQ ======="
    make -C ../../ppq_tbb -f Makefile release
}

print_help(){
    echo "\t-c, To compile or not to compile"
    echo "\t-r, To run make clean over all codes"
    echo "\t-t, Define number of threads"
    echo "\t-i, Define number of insert operations"
    echo "\t-f, Define percentage of fixed insert operations"
    echo "\t-p, Parameters file. Will not use any other of the parameters."
}

run_params(){
    echo ${PARAMS}
    while read line
    do
        THREADS=`echo $line | awk '{x=$1}END{print x}'`
        INSERTS=`echo $line | awk '{x=$2}END{print x}'`
        FIXED=`echo $line | awk '{x=$3}END{print x}'`
        #std
        CMD="./../../ppq_std/main -p -t${THREADS} -i${INSERTS} -f${FIXED}"
        eval ${CMD}
        #echo ${CMD}
        # sequential
        #./../../ppq_lock_free/main -p -t${THREADS} -i${INSERTS} -f${FIXED}
        #lock based
        CMD="./../../ppq_with_locks/main -p -t${THREADS} -i${INSERTS} -f${FIXED}"
        eval ${CMD}
        #echo ${CMD}
        #lock-free
        CMD="./../../ppq_lock_free/main -p -t${THREADS} -i${INSERTS} -f${FIXED}"
        eval ${CMD}
        #echo ${CMD}
        #tbb
        CMD="./../../ppq_tbb/main -p -t${THREADS} -i${INSERTS} -f${FIXED} -c "
        eval ${CMD}
        #echo ${CMD}
        echo 
    done < "${PARAMS}"
}

while getopts ":r :c :m: :t: :f: :i: :h :p:" opt; do
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
        p)
            PARAMS=${OPTARG}
            ;;
       \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
        h)
            print_help
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

if [ "$#"  -eq 0  ]; then
    print_help
    exit 0
fi

if [ ${CL} -eq 1 ]; then
    clean_up
    exit 0
fi

if [ ${COMP} -eq 1 ]; then
    compile
fi

if [ -n ${PARAMS} ]; then
    run_params
    exit 0
fi
# std
./../../ppq_std/main -p -t${THREADS} -i${INSERTS} -f${FIXED}

# sequential
#./../../ppq_lock_free/main -p -t${THREADS} -i${INSERTS} -f${FIXED}

#lock based
./../../ppq_with_locks/main -p -t${THREADS} -i${INSERTS} -f${FIXED}

#lock-free
./../../ppq_lock_free/main -p -t${THREADS} -i${INSERTS} -f${FIXED}

#tbb
./../../ppq_tbb/main -c -p -t${THREADS} -i${INSERTS} -f${FIXED} 

