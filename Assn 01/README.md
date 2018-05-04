# ECU-3000

## Assignment 01
### Assignment Description
This assignment was split into two parts - coordinator, and worker. Coordinator uses worker by forking and using execlp to re-image the children processes into workers. Each worker is responsible for adding two integers together, and coordinator then displays the final result after all the additions. (The way the sum of two integers is returned is via the exit status of each worker process.)
### Source Files
coordinator.c, worker.c
### Compilation, Testing, and Known Issues
```
Compile:
gcc -std=c99 -Wall -w -Werror -o worker worker.c
gcc -std=c99 -Wall -w -Werror -o coordinator coordinator.c

Testing: ./coordinator <integers>
```
Notes:
- `<integers>` in the testing command means that you should type an arbitrary amount of integers. For example, you could execute the command `./coordinator 7 8 9 1 5 2 7 4 8 2 1 9 5 7 1 8 4 3`, and the final result should be 91