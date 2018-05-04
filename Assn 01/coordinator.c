/*
 * Assignment 01
 * File:    coordinator.c
 * Author:  Matthew Morgan
 * Date:    20 January 2018
 * Description:
 * Coordinator accepts a series of integers as command line arguments,
 * using worker to compute the sum of all of them before the program terminates.
 */

typedef enum { false, true } boolean;

// Inclusion statements
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <errno.h>

/* Worker represents a single worker process that gets generated. It stores
 * the PID of the worker and the exit status received on termination. */
struct Worker {
    int id;       // The PID of the child
    int exitStat;   // Exit status of the child on termination
};
typedef struct Worker worker;

// Prototypes
worker* childFork(char* n1, char* n2);

int main(int argc, char* argv[]) {
    boolean argsOdd;
    int cnt = argc-1;

    // Loop until cnt is 1, which represents that there is only a single int
    // remaining (the final sum)
    while(cnt != 1) {
        argsOdd = (cnt % 2 == 1);
        int workersNeeded = (cnt / 2) + argsOdd;

        // Loop until all necessary workers have been created
        for(int i=0; i<workersNeeded; i++) {
            worker* w = childFork(argv[(i*2)+1],
                (argsOdd && i==workersNeeded-1 ? "0" : argv[(i*2)+2]));

            argv[i+1] = (char*)malloc(33*sizeof(char));
            sprintf(argv[i+1], "%d", w->exitStat);
            free(w);
        }

        cnt = (cnt + argsOdd) / 2;
    }

    printf("The final result of the numbers provided is: %s\n", argv[1]);

    return 0;
}

worker* childFork(char* n1, char* n2) {
    int pid = fork(), stat;
    
    if (pid == 0) { execlp("./worker", "./worker", n1, n2, NULL); }
    else if (pid < 0) { perror("Error during fork!\n"); exit(-1); }
    
    // This code will only be used by the parent - it should return
    // the PID of the created worker and the exit status on its termination
    worker* w = malloc(sizeof(*w));
    w->id = wait(&stat);
    w->exitStat = WEXITSTATUS(stat);
    return w;
}