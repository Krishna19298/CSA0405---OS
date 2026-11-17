# CSA0405 – Operating Systems – Assignment 2 (CO2)

| Item | Detail |
|---|---|
| Name | Leela Krishna R |
| Register number | <REGISTER NUMBER> |
| Course / Section | CSA04 – Operating Systems, SIMATS Engineering |
| Compiler | gcc 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04.1) |
| Linux used | Ubuntu 24.04 LTS (1 CPU core, 4 GB RAM) |
| Compile command | `gcc -std=gnu11 -Wall -Wextra -pthread file.c -o file` (`-O2` for timing programs) |

## Folders
One folder per task. Each has the C source, and `output/` with command outputs and figures.

| Folder | Task | Main files | Result in one line |
|---|---|---|---|
| Q1 | Processes and states | `fork_demo.c`, `bench.c`, `mem.c` | fork() children seen with ps and /proc; process create 59.1 µs vs thread 9.3 µs; design: pool of worker processes with threads |
| Q2 | CPU scheduling | `rr.c`, `sched.py` | Round Robin output matches expected (avg WT 13.50); recommend MLFQ with ageing |
| Q3 | Threads | `plates.c`, `race.c` | CPU-only speed-up about 1.0 on 1 core; with I/O wait up to 5.39; unsafe counter gave 100000 instead of 400000 |
| Q4 | IPC | `pipe_sensor.c`, `xfer.c` | Pipe output matches expected; shared memory about 2x faster at 100 MB |
| Q5 | Synchronisation | `bb.c`, `bbm.c`, `book.c`, `book_io.c`, `cnt.c` | Sum 210 in 100/100 runs; no mutex lost 1,509 items; lock per coach 12,973 vs global 900 bookings/s |
| Q6 | Deadlock, Banker's | `banker.c`, `bench.c` | Output matches expected; time grows as n squared (3.98x when n doubles) |

## How to run
```
git clone <this repository>
cd CSA0405-<RegisterNumber>-Assignment2
./run_all.sh              # compiles into bin/ and runs everything
```
Single program example: `gcc -std=gnu11 -Wall -Wextra -pthread Q5/bb.c -o bb && ./bb`

Q1: run `./bin/fork_demo 3 &` and in another terminal `ps -o pid,ppid,stat,wchan:12,rss,comm --ppid <parent pid>` and `cat /proc/<child pid>/status`.
Q4: the hang test is `gcc -std=c11 -DNO_CLOSE Q4/pipe_sensor.c -o hang && timeout 3 ./hang` (exit code 124).

## Notes
- The test machine had one core, so thread speed-ups for pure CPU work are about 1.0. On a multi-core machine the numbers will be different. Re-run and compare.
- Timing values change a little from run to run.
- Commits: one per task (see `git log`).
