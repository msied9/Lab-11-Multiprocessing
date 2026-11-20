#!/bin/bash
# Times mandelmovie with 1, 2, 5, 10 and 20 processes.
# Results are saved in results.csv (processes,seconds).
# Usage: ./run_tests.sh          (full size images, takes several minutes)
#        ./run_tests.sh -W 200 -H 200 -m 100   (extra options, for a quick test)
make > /dev/null || exit 1
TIMEFORMAT=%R
echo "processes,seconds" > results.csv
for n in 1 2 5 10 20
do
    rm -f mandel*.jpg
    secs=$( { time ./mandelmovie -n $n "$@" > /dev/null; } 2>&1 )
    echo "$n processes: $secs seconds"
    echo "$n,$secs" >> results.csv
done