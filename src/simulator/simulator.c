#define _GNU_SOURCE // Enable GNU extensions (needed on some systems)
#include "simulator.h"
#include "player.h"
#include "table.h"
#include <curl/curl.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function to create a new simulation
Simulator *newSimulator(Parameters *parameters, Rules *rules, Strategy *strategy, int core) {
    Simulator *simulator = (Simulator *)malloc(sizeof(Simulator));

    simulator->table = newTable(parameters, rules, strategy);
    simulator->parameters = parameters;
    simulator->rules = rules;
    simulator->strategy = strategy;
    simulator->core = core;
    initReport(&simulator->report);

    return simulator;
}

//
void simulatorDelete(Simulator *simulator) { free(simulator); }

//
Report *getReport(Simulator *simulator) { return &(simulator->report); }

// The Simulator Process function
void *simulatorRunOnce(void *arg) {
    Simulator *simulator = (Simulator *)arg;

    // Set CPU affinity for this thread
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(simulator->core, &cpuset);

    pthread_t thread = pthread_self();
    if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0) {
        perror("thread set affinity");
    }

    tableSession(simulator->table, strcmp(MIMIC, simulator->parameters->strategy) == 0);
    mergeReport(&simulator->report, &simulator->table->player->report);
    simulator->report.total_rounds += simulator->table->report.total_rounds;
    simulator->report.total_hands += simulator->table->report.total_hands;

    return simulator;
}

