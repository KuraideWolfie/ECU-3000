# ECU-3000

## Assignment 02
### Assignment Description
This assignment, like assignment 1, takes a string of integers and sums them; however, instead of forking and creating separate processes, different threads are created, and barriers used to synchronize the addition of the integers.
### Source Files
prefix.c, prefix_new.c
### Compilation, Testing, and Known Issues
```
Compile:
gcc -std=gnu99 -Wall -w -Werror -o prefix prefix.c -pthread
gcc -std=gnu99 -Wall -w -Werror -o prefixnew prefix_new.c -pthread -lm

Testing: ./prefix <datafile> | ./prefixnew <datafile>
```
Notes:
- prefix.c is the first version of the assignment, where prefix_new.c is a second, updated version that changes how the threads work after their creation. 
- The format of a data file is similar to the first assignment, where the file should contain, simply, a string of integers, all delimited by spaces.