// Assignment 02
// File:    prefix.c
// Author:  Matthew Morgan
// Date:    14 February 2018
// Description:
/* Prefix implements threading and barriers to accomplish the simple task
 * of adding together a string of integers read from an input file. */

// Inclusion statements
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// phaseBarrier is the barrier that holds the threads back after computation
// tAttributes is the attributes for threads - initialized in main
// tIdentifiers will be an array of thread identifiers
// dist is the current distance being used for addition
// nums will be initialized as an array of integers
// oNums will be the same as nums, except it will store the prior phase's sums
// cnt is the number of integers stored in the array nums
pthread_barrier_t phaseBarrier;
pthread_attr_t tAttributes;
pthread_t *tIdentifiers;
int dist=1, *nums, *oNums, cnt=0;

// Function prototypes
void printNums();
void* threadExecute(void* index);

int main(int argc, char* argv[]) {
    int tmp; // Temporary storage variable

    // Open the file specified in argv[1] for reading, and then
    // calculate the number of integers that need to be read
    FILE* f = fopen(argv[1], "r");
    if (f == NULL) { printf("SYS: The file couldn't be opened\n"); exit(-1); }
    while(!feof(f))
        if (fscanf(f, "%d", &tmp)==1) { cnt++; }
    rewind(f);

    // Alocate enough space in the arrays for integers to be stored
    nums = malloc(sizeof(*nums)*(cnt+1));
    oNums = malloc(sizeof(*nums)*(cnt+1));
    tIdentifiers = malloc(sizeof(*tIdentifiers)*(cnt+1));

    // Set global thread attributes and barrier attributes
    pthread_attr_init(&tAttributes);
    pthread_attr_setscope(&tAttributes, PTHREAD_SCOPE_SYSTEM);
    pthread_barrier_init(&phaseBarrier, NULL, cnt+1);

    // Scan initial integers into the array
    for(int i=0; i<cnt; i++) {
        fscanf(f, "%d", &nums[i+1]);
        oNums[i+1] = nums[i+1];
    }
    fclose(f);

    // Print index labels and initial contents of the array
    printf("Index:   ");
    for(int i=0; i<cnt; i++) { printf("%4d ", i+1); }
    printf("\nInitial: ");
    printNums();

    // Create the necessary threads for processing
    for(int i=0; i<cnt+1; i++) {
        int* index = malloc(sizeof(*index));
        *index = i;

        // If an error occurs, terminate the program
        if (pthread_create(&tIdentifiers[i], &tAttributes, threadExecute, (void*)index)) {
            printf("SYS: An error has occured during thread creation!\n");
            exit(-1);
        }
    }

    // Waits for the coordinator thread to exit before program terminates
    pthread_join(tIdentifiers[0], NULL);
    exit(0);
}

/* Loops through summation of integers until the distance being used for addition
 * surpasses the number of integers being summed.
 * @param index The index of the array that the thread executing this function
 *   should use to reference the integer it does additions for */
void* threadExecute(void* index) {
    // Cast the index of the thread's index in-array to an integer. An index of
    // 0 correlates to the thread being the coordinator, and others workers
    int ind = *((int*)index);

    // Execution of this loop continues until the current distance being summed
    // is greater than the numbers in the array currently
    while(dist < cnt) {
        if (ind == 0) {
            /* COORDINATOR THREAD
             * Blocks at the phase barrier, updating the distance and oNums
             * afterward. Then blocks on the update barrier to release the
             * worker threads to do computation */
            pthread_barrier_wait(&phaseBarrier);
            printf("Dist[%d]: ", dist);
            printNums();
            for(int i=0; i<cnt; i++) { oNums[i+1] = nums[i+1]; }
            dist *= 2;
            pthread_barrier_wait(&phaseBarrier);
        }
        else {
            /* WORKER THREAD
             * Handles additions based on the current distance
             * Workers block at the phase barrier after computation, and then
             * the update barrier after for distance/oNums to update */
            if (dist < ind)
                nums[ind] += oNums[ind-dist];
            pthread_barrier_wait(&phaseBarrier);
            pthread_barrier_wait(&phaseBarrier);
        }
    }

    // Addition execution is complete; terminate the thread(s)
    pthread_exit(0);
}

/* Prints the array of integers in its current state */
void printNums() {
    for(int i=0; i<cnt; i++)
        printf("%4d ", nums[i+1]);
    printf("\n");
}
