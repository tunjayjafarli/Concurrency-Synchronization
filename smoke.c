//  smoke.c
//
//  Created by Tunjay Jafarli on 2014-04-05.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include "uthread.h"

#define NUM_ITERATIONS 100
#define N 3

// monitor
uthread_monitor_t* mon;

// resources: tobacco, paper, and matches represented by CV's

// condition variables; each smoker's thread is only waiting on one condition variable.
uthread_cond_t* smoker;      // agent's thread must wait on this CV
uthread_cond_t* tobacco;     // smoker_t_thread must wait on this CV
uthread_cond_t* paper;       // smoker_p_thread must wait on this CV
uthread_cond_t* matches;     // smoker_m_thread must wait on this CV

void* agent() {
    uthread_monitor_enter (mon);
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        
        // make any random two resources available to smokers and signal their condition variables
        int n = random() % N;

        
        if (n == 0) {
            printf ("n = %d\nPaper and matches available. \n", n);
            uthread_cond_signal(tobacco);
            printf ("Tobacco-possessing smoker signalled.\n");
        }
        
        if (n == 1) {
            printf ("n = %d\nTobacco and matches available. \n", n);
            uthread_cond_signal(paper);
            printf ("Paper-possessing smoker signalled.\n");
        }
        
        if (n == 2) {
            printf ("n=%d\nTobacco and paper available. \n", n);
            uthread_cond_signal(matches);
            printf ("Matches-possessing smoker signalled.\n");
        }
        
        // wait on a condition variable that smokers signal when they are able to smoke
        printf ("The agent is waiting for the smoker to smoke.\n");
        uthread_cond_wait(smoker);
    }
    uthread_monitor_exit (mon);
    return NULL;
}

// tobacco-possessing smoker
void* smoker_t() {
    uthread_monitor_enter (mon);
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // signals the agent when he finds the both needs of resources
        uthread_cond_signal(smoker);
        printf ("The agent was signalled by the tobacco smoker.\n");
        
        // must wait on a condition variable (i.e tobacco) when unable to run
        printf ("Tobacco-possessing smoker is waiting.\n");
        uthread_cond_wait(tobacco);
        printf ("Tobacco-possessing smoker is smoking.\n");
    }
    uthread_monitor_exit (mon);
    return NULL;
}

// paper-possessing smoker
void* smoker_p() {
    uthread_monitor_enter (mon);
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // signals the agent when he finds the both needs of resources
        uthread_cond_signal(smoker);
        printf ("The agent was signalled by the paper smoker.\n");

        // must wait on a condition variable (i.e paper) when unable to run
        printf ("Paper-possessing smoker is waiting.\n");
        uthread_cond_wait(paper);
        printf ("Paper-possessing smoker is smoking.\n");
        
    }
    uthread_monitor_exit (mon);
    return NULL;
}

// matches-possessing smoker
void* smoker_m() {
    uthread_monitor_enter (mon);
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // signals the agent when he finds the both needs of resources
        uthread_cond_signal(smoker);
        printf ("The agent was signalled by the matches smoker.\n");
        
        // must wait on a condition variable (i.e matches) when unable to run
        printf ("Matches-possessing smoker is waiting.\n");
        uthread_cond_wait(matches);
        printf ("Matches-possessing smoker is smoking.\n");
        
    }
    uthread_monitor_exit (mon);
    return NULL;
}

void mysrandomdev() {
    unsigned long seed;
    int f = open ("/dev/random", O_RDONLY);
    read    (f, &seed, sizeof (seed));
    close   (f);
    srandom (seed);
}

int main() {
    // initialize to one processor
    uthread_init(1);
    
    // initialize monitor
    mon = uthread_monitor_create();
    
    // initialize CV's
    smoker = uthread_cond_create (mon);
    tobacco = uthread_cond_create (mon);
    paper = uthread_cond_create (mon);
    matches = uthread_cond_create (mon);
    
    mysrandomdev();
    
    // create 4 threads; one for the agent and one for each type of smoker
    uthread_t* agent_thread = uthread_create(agent, NULL);              // agent's thread
    uthread_t* smoker_t_thread = uthread_create(smoker_t, NULL);        // tobacco-possessing smoker's thread
    uthread_t* smoker_p_thread = uthread_create(smoker_p, NULL);        // paper-possessing smoker's thread
    uthread_t* smoker_m_thread = uthread_create(smoker_m, NULL);        // matches-possessing smoker's thread
    
    // wait only for the agent's thread to exit
    uthread_join(agent_thread);
}
