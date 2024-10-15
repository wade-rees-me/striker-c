#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "table.h"
#include "report.h"
#include "rules.h"
#include "parameters.h"
#include "simulation.h"
#include "constants.h"

//
typedef struct {
    Parameters* parameters;
    Rules* rules;
    Table* table;
    Report report;
} Simulator;

// Global functions
Simulator* newSimulator(Parameters *parameters, Rules *rules);
void simulatorDelete(Simulator* sim);
void simulatorRunOnce(Simulator *simulation);
void simulatorRunSimulation(Simulator *sim);
void simulatorInsert(Simulator *sim, Simulation *sdt, const char *playbook);

#endif // SIMULATOR_H
