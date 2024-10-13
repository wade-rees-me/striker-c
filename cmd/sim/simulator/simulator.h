#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <time.h>
#include "table.h"
#include "report.h"
#include "memory.h"
#include "rules.h"
#include "parameters.h"

typedef struct {
    Parameters* parameters;
    Rules* rules;
    Table* table;
    Report report;
} Simulation;

typedef struct {
    char playbook[128];
    char guid[128];
    const char* simulator;
    const char* summary;
    const char* simulations;
    char rounds[128];
    char hands[128];
    char total_bet[128];
    char total_won[128];
    char advantage[128];
    char total_time[128];
    char average_time[128];
    char parameters[2048];
} SimulationDatabaseTable;

Simulation* newSimulation(Parameters *parameters, Rules *rules);
void simulationDelete(Simulation* sim);
void simulatorRunOnce(Simulation *simulation);
void simulatorRunSimulation(Simulation *sim);
void simulatorInsert(Simulation *sim, SimulationDatabaseTable *sdt, const char *playbook);

#endif // SIMULATOR_H
