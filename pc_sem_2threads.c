//  pc_sem_2threads.c  [ one producer thread and one consumer thread ]
//
//  Created by Tunjay Jafarli on 2014-04-07.
//
// NOTE: This solution assumes that uthread.c has been implemented correctly

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <assert.h>
#include "uthread.h"

#define MAX_ITEMS 10
#define NUM_ITERATIONS 10

int items = 0;
int histogram [MAX_ITEMS + 1];

uthread_semaphore_t* empty;
uthread_semaphore_t* full;


void* producer() {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        assert (items >= 0 && items <= MAX_ITEMS);
        // decrements
        uthread_P (full);
            // add an item to the shared resources pool
            items++;
            // print the produced item for debugging
            printf ("Produce: %d\n", items);
            histogram [items] ++;
        // increments
        uthread_V (empty);
        assert (items >= 0 && items <= MAX_ITEMS);
    }
    return NULL;
}

void* consumer() {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        assert (items >= 0 && items <= MAX_ITEMS);
        // decrements
        uthread_P (empty);
            // print the consumed item for debugging
            printf ("Consume: %d\n", items);
            // remove an item from the shared resources pool
            items--;
            histogram [items] ++;
        // increments
        uthread_V (full);
        assert (items >= 0 && items <= MAX_ITEMS);
    }
    return NULL;
}


int main() {
    // initialized to four processors
    uthread_init(2);
    
    // semaphores
    empty = uthread_semaphore_create (0);
    full  = uthread_semaphore_create (MAX_ITEMS);
    
    // create one thread for each producer and one thread for each consumer
    void* arg = (void*) NUM_ITERATIONS;
    uthread_t* producer_thread = uthread_create(producer, arg);
    uthread_t* consumer_thread = uthread_create(consumer, arg);
    
    // the main thread must wait for the producer and consumer threads to finish before it exits
    uthread_join(producer_thread);
    uthread_join(consumer_thread);
    
    
    // TESTING AREA
    // the shared resources pool must be 0 when the program terminates, indicating that all produced items were consumed
    printf ("The Pool: %d\n", items);
    int total = 0;
    for (int i = 0; i < MAX_ITEMS+1; i++) {
        total += histogram[i];
        printf ("Histogram: %d\n", histogram[i]);
    }
    printf ("Total changs to items: %d\n", total);
}


