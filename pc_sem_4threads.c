//  pc_sem_4threads.c     [ WITH 4 THREADS ]
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

uthread_semaphore_t* mutex;
uthread_semaphore_t* empty;
uthread_semaphore_t* full;



void* producer() {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        assert (items >= 0 && items <= MAX_ITEMS);
        uthread_P (full);
            uthread_P (mutex);
                // add an item to the shared resources pool
                items++;
                // print the produced item for testing
                printf ("Produce: %d\n", items);
                histogram [items] ++;
            uthread_V (mutex);
        uthread_V (empty);
        assert (items >= 0 && items <= MAX_ITEMS);
    }
    return NULL;
}

void* consumer() {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        assert (items >= 0 && items <= MAX_ITEMS);
        uthread_P (empty);
            uthread_P (mutex);
                // print the consumed item for testing
                printf ("Consume: %d\n", items);
                // remove an item from the shared resources pool
                items--;
                histogram [items] ++;
            uthread_V (mutex);
        uthread_V (full);
        assert (items >= 0 && items <= MAX_ITEMS);
    }
    return NULL;
}


int main() {
    // initialized to four processors
    uthread_init(4);
    
    // semaphores
    mutex = uthread_semaphore_create (1);
    empty = uthread_semaphore_create (0);
    full  = uthread_semaphore_create (MAX_ITEMS);
    
    // create one thread for each producer and one thread for each consumer
    void* arg = (void*) NUM_ITERATIONS;
    uthread_t* producer_thread1 = uthread_create(producer, arg);
    uthread_t* producer_thread2 = uthread_create(producer, arg);
    uthread_t* consumer_thread1 = uthread_create(consumer, arg);
    uthread_t* consumer_thread2 = uthread_create(consumer, arg);
    
    // the main thread must wait for the producer and consumer threads to finish before it exits
    uthread_join(producer_thread1);
    uthread_join(producer_thread2);
    uthread_join(consumer_thread1);
    uthread_join(consumer_thread2);
    
    
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


