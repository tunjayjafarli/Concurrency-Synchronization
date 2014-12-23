//  pc.c
//
//  Created by Tunjay Jafarli on 2014-04-05.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <assert.h>
#include "uthread.h"

#define MAX_ITEMS 10
#define NUM_ITERATIONS 100

int items = 0;
int producer_wait_count;
int consumer_wait_count;
int histogram [MAX_ITEMS + 1];

uthread_monitor_t* mon;
uthread_cond_t* empty;
uthread_cond_t* full;

void* producer() {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        uthread_monitor_enter (mon);
        assert (items >= 0 && items <= MAX_ITEMS);
        while (items == MAX_ITEMS) {
            // block producer using CV's
            uthread_cond_wait(full);
            producer_wait_count++;
        }
        // add an item to the shared resources pool
        items++;
        // print the produced item for debugging
        printf ("Produce: %d\n", items);
        histogram [items] ++;
        // signal to wakeup the thread
        uthread_cond_signal(empty);
        assert (items >= 0 && items <= MAX_ITEMS);
        uthread_monitor_exit (mon);
    }
    return NULL;
}

void* consumer() {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        uthread_monitor_enter (mon);
        assert (items >= 0 && items <= MAX_ITEMS);
        while (items == 0) {
            // block consumer using CV's
            uthread_cond_wait(empty);
            consumer_wait_count++;
        }
        // print the consumed item for debugging
        printf ("Consume: %d\n", items);
        // remove an item from the shared resources pool
        items--;
        histogram [items] ++;
        // signal to wakeup the thread
        uthread_cond_signal(full);
        assert (items >= 0 && items <= MAX_ITEMS);
        uthread_monitor_exit (mon);
    }
    return NULL;
}


int main() {
    // initialized to four processors
    uthread_init(4);
    // initialize the monitor
    mon = uthread_monitor_create();
    // initialize the condition variables
    empty = uthread_cond_create (mon);
    full = uthread_cond_create (mon);
    
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
    printf ("Producer Wait Counter: %d\n", producer_wait_count);
    printf ("Consumer Wait Counter: %d\n", consumer_wait_count);
    
    printf ("Histogram of the values that items takes on: \n");
    int total = 0;
    for (int i = 0; i < MAX_ITEMS+1; i++) {
        total += histogram[i];
        printf ("Items = %d Times = %d\n", i, histogram[i]);
    }
    printf ("Total changes to items: %d\n", total);
    printf ("The Pool: %d\n", items);

}

