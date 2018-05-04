/*
 * CSCI 3000, Assignment 03
 * Author:  Matthew Morgan
 * Date:    22 March 2018
 * File:    conpro.c
 * Description:
 * Initiates two threads - a producer and a consumer. The producer
 * randomly generates integers between 1 and 100, storing them into
 * a buffer that the consumer picks them from to compute their squares.
 *
 * Version History:
 * Version 1.1, 4 May 2018
 * - The condition of the loop at line 92 was modified, as prior to its
 *   change, any value of cnt that could be found via 10 + 8x, for any
 *   value of x, would print an extra line of consumption.
 */

// Inclusion statements
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>  // Semaphore types and functions
#include <pthread.h>    // Threading types and functions

// Function prototypes
void* consume(void *arg);
void* produce(void *arg);

// tAttributes contains the attributes for the threads in the program
// consumer and producer are pointers to the thread IDs
// mutex is the semaphore that controls production and consumption
// arr is the array of produced integers; cnt is the number of entries
// stored in arr
pthread_attr_t tAttributes;
pthread_t *consumer, *producer;
sem_t mutex;
int *arr, cnt;

int main(int argc, char* argv[]) {
  if (argc == 2) {
    sscanf(argv[1], "%d", &cnt);

    if (cnt > 0) {
      // Allocate space for globals
      arr = malloc(sizeof(*arr)*cnt);
      consumer = malloc(sizeof(pthread_t));
      producer = malloc(sizeof(pthread_t));

      // Initialize the array entries to default - 0
      // and thread attributes/semaphore. Also sets seed
      // for RNG
      for(int i=0; i<cnt; i++) { arr[i] = 0; }
      pthread_attr_init(&tAttributes);
      pthread_attr_setscope(&tAttributes, PTHREAD_SCOPE_SYSTEM);
      sem_init(&mutex, 1, 1);
      srand(time(NULL));

      // Create the producer and consumer threads, and wait for
      // those threads to exit
      pthread_create(producer, &tAttributes, produce, NULL);
      pthread_create(consumer, &tAttributes, consume, NULL);
      pthread_join(*consumer, NULL);
      pthread_join(*producer, NULL);
    }
    else {
      // Error - the number of ints to produce is 0
      printf("ERR: Specify a number greater than 0!\n");
      exit(-1);
    }
  }
  else {
    // Error - the number of arguments required is insufficient
    printf("ERR: The number of arguments provided is insufficient!\n");
    exit(-1);
  }

  exit(0);
}

/**
 * Main function for the consumer thread. This function
 * has the consumer wait until it can access the semaphore,
 * reading all produced integers since its last access
 */
void* consume(void *arg) {
  FILE* f = fopen("consumption", "w");
  int curInt = 0, square;

  // While the number of integers consumed is not
  // the same as the amount requested, loop
  while(curInt < cnt) {
    sem_wait(&mutex);
    // While we haven't yet found a 0 (invalid entry)
    // square the current integer consumed and write
    // output
    while(arr[curInt] != 0 && curInt < cnt)
    {
      square = arr[curInt] * arr[curInt];
      printf("Consumer: %3d ^ 2 = %5d\n", arr[curInt], square);
      fprintf(f, "%3d:%-5d%c", arr[curInt], square, (curInt%10==9 ? '\n' : ' '));
      curInt++;
    }
    sem_post(&mutex);
  }
  
  fclose(f);
  pthread_exit(0);
}

/**
 * Main function for the producer thread. This function has the
 * producer wait on the semaphore, produce a single integer, store
 * it, and then release the semaphore up to the number of integers
 * requested.
 */
void* produce(void *arg) {
  FILE* f = fopen("production", "w");

  // For as many numbers as requested, wait on the
  // mutex, generate a random number, write output
  // and release the mutex
  for(int i=0; i<cnt; i++) {
    sem_wait(&mutex);
    arr[i] = (rand()%100)+1;
    printf("Producer: Item %d is %d\n", i+1, arr[i]);
    fprintf(f, "%3d%c", arr[i], (i%10==9 ? '\n': ' '));
    sem_post(&mutex);
  }
  
  fclose(f);
  pthread_exit(0);
}
