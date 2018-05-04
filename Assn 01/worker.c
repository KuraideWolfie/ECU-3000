/*
 * Assignment 01
 * File:    worker.c
 * Author:  Matthew Morgan
 * Date:    20 January 2018
 * Description:
 * Worker accepts two integers as command line arguments, returning their sum
 * upon exit.
 */

typedef enum { false, true } boolean;

// Inclusion statements
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <errno.h>

// Prototypes
void argumentCheck(int cnt, char* args[]);

int main(int argc, char* argv[]) {
    argumentCheck(argc, argv);

    // Get the process's ID, scan in its operands, and sum them
    int id, n1, n2, sum;
    id = getpid();
    sscanf(argv[1], "%d", &n1);
    sscanf(argv[2], "%d", &n2);
    sum = n1+n2;

    // Print information about this worker and then exit
    printf("PID[%d]: Ops [%3d, %3d] Sum [%3d]\n", id, n1, n2, sum);
    exit(sum);
}

void argumentCheck(int cnt, char* args[]) {
    // Toggle for if an error is found
    boolean errorFound = false;

    // Insufficient arguments given
    if (cnt < 3) {
        errorFound = true;
        printf("Insufficient arguments given to worker [%d]!\n", cnt);
    }

    // Print error message and return a failure
    if (errorFound) {
        printf("Usage: ./worker <n1> <n2>");
        exit(EXIT_FAILURE);
    }
}