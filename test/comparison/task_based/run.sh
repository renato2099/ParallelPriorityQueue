#/bin/bash

clean_up(){
    echo "===== CLEAN UP ====="
    make -C ../../../ppq_tbb/ -f Makefile clean
}

compile(){
    clean_up
    echo ""
    echo "======= COMPILING TBB PPQ ======="
    make -C ../../../ppq_tbb -f Makefile release
}

print_help(){
    echo "\t-p, Parameters file"
}

TBB=false
LF=false
while getopts ":p: :c :r :t :l" opt; do
   case $opt in
        t)
            TBB=true
            ;;
        l)
            LF=true
            ;;
        c)
            compile
            ;;
        r) 
            clean_up
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

if [ "$#"  -eq 0  ]; then
    print_help
    exit 0
fi

if [ -z ${PARAMS} ]; then
    exit 0
fi

while read line
do
    THREADS=`echo $line | awk '{x=$1}END{print x}'`
    INSERTS=`echo $line | awk '{x=$2}END{print x}'`
    FIXED=`echo $line | awk '{x=$3}END{print x}'`
    CMD="./../../../ppq_tbb/main -p -t${THREADS} -i${INSERTS} -f${FIXED} "
    if [ ${TBB} == true  ]; then
        CMD+="-c"
    else
        CMD+="-l"
    fi
    eval ${CMD}
done < "${PARAMS}"

#tbb
#./../../ppq_tbb/main -c -p -t${THREADS} -i${INSERTS} -f${FIXED} 
