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

// The SimulatorProcess function
void *simulatorRunOnce(void *arg) {
    Simulator *simulator = (Simulator *)arg;

    // Set CPU affinity for this thread
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(simulator->core, &cpuset);

    pthread_t thread = pthread_self();
    if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0) {
        perror("pthread_setaffinity_np");
    }

    simulatorRunSimulation(simulator);

    return simulator;
}

// Function to run the simulation
void simulatorRunSimulation(Simulator *simulator) {
    if (simulator->parameters->verbose) {
        printf("    Start: %s table session\n", simulator->parameters->strategy);
    }
    tableSession(simulator->table, strcmp("mimic", simulator->parameters->strategy) == 0);
    if (simulator->parameters->verbose) {
        printf("    End: table session\n");
    }

    simulator->report.total_bet += simulator->table->player->report.total_bet;
    simulator->report.total_won += simulator->table->player->report.total_won;
    simulator->report.total_rounds += simulator->table->report.total_rounds;
    simulator->report.total_hands += simulator->table->report.total_hands;
    simulator->report.total_blackjacks += simulator->table->player->report.total_blackjacks;
    simulator->report.total_doubles += simulator->table->player->report.total_doubles;
    simulator->report.total_splits += simulator->table->player->report.total_splits;
    simulator->report.total_wins += simulator->table->player->report.total_wins;
    simulator->report.total_pushes += simulator->table->player->report.total_pushes;
    simulator->report.total_loses += simulator->table->player->report.total_loses;
    simulator->report.duration += simulator->table->report.duration;
}

