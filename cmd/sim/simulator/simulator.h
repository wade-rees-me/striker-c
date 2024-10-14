#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <time.h>
#include "table.h"
#include "report.h"
#include "memory.h"
#include "rules.h"
#include "parameters.h"
#include "constants.h"

//
typedef struct {
    Parameters* parameters;
    Rules* rules;
    Table* table;
    Report report;
} Simulator;

//
typedef struct {
    char playbook[MAX_STRING_SIZE];
    char guid[MAX_STRING_SIZE];
    const char* simulator;
    const char* summary;
    const char* simulations;
    char rounds[MAX_STRING_SIZE];
    char hands[MAX_STRING_SIZE];
    char total_bet[MAX_STRING_SIZE];
    char total_won[MAX_STRING_SIZE];
    char advantage[MAX_STRING_SIZE];
    char total_time[MAX_STRING_SIZE];
    char average_time[MAX_STRING_SIZE];
    char parameters[MAX_BUFFER_SIZE];
    char rules[MAX_BUFFER_SIZE];
} SimulationDatabaseTable;

// Global functions
Simulator* newSimulator(Parameters *parameters, Rules *rules);
void simulatorDelete(Simulator* sim);
void simulatorRunOnce(Simulator *simulation);
void simulatorRunSimulation(Simulator *sim);
void simulatorInsert(Simulator *sim, SimulationDatabaseTable *sdt, const char *playbook);

#endif // SIMULATOR_H
