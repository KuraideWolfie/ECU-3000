# ECU-3000

## Assignment 04
### Assignment Description
This assignment, basically put, is a replication of the command `ls -l`, where files were scanned using Linux's i-node system, and information about those files were printed to the screen. If a specific file was specified, then information about that file was printed, and if no parameter was specified with the program, then the current directory was to be assumed as the one to be printed.
### Source Files
myls.c, myls.h
### Compilation, Testing, and Known Issues
```
Compile: gcc -std=c99 -Wall -w -Werror -o myls myls.c myls.h
Testing: ./myls [file]

[file] can be nothing, a directory, or a specific filename.
```
