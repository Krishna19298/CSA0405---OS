#!/bin/bash
# Compiles and runs every program. Usage: ./run_all.sh
set -e
FL="-std=gnu11 -Wall -Wextra -pthread"
mkdir -p bin
gcc $FL -O2 -o bin/fork_demo Q1/fork_demo.c;  gcc -O2 -pthread -o bin/q1bench Q1/bench.c 2>/dev/null
gcc $FL -o bin/rr Q2/rr.c
gcc -O2 -pthread -o bin/plates Q3/plates.c; gcc -O1 -pthread -o bin/race Q3/race.c
gcc $FL -o bin/pipe_sensor Q4/pipe_sensor.c; gcc -O2 -pthread -o bin/xfer Q4/xfer.c
gcc $FL -o bin/bb Q5/bb.c; gcc -O1 -pthread -o bin/bbm Q5/bbm.c; gcc -O2 -pthread -o bin/book Q5/book.c; gcc -O2 -pthread -o bin/cnt Q5/cnt.c
gcc $FL -o bin/banker Q6/banker.c; gcc -O2 -o bin/q6bench Q6/bench.c
echo "== Q1"; bin/fork_demo 3; bin/q1bench
echo "== Q2"; for q in 3 1 20; do bin/rr $q; done; python3 Q2/sched.py | head -20
echo "== Q3"; bin/plates 0; bin/plates 20; bin/race
echo "== Q4"; bin/pipe_sensor; bin/xfer
echo "== Q5"; bin/bb; bin/bbm 1 1; bin/bbm 4 1; bin/bbm 4 0; for m in 0 1 2; do bin/book $m 4; done; for m in 0 1 2; do bin/cnt $m 4; done
echo "== Q6"; bin/banker; bin/q6bench
