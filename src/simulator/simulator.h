#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "constants.h"
#include "parameters.h"
#include "report.h"
#include "rules.h"
#include "strategy.h"
#include "table.h"

//
typedef struct {
    Parameters *parameters;
    Rules *rules;
    Strategy *strategy;
    Table *table;
    int core;
    Report report;
} Simulator;

// Global functions
Simulator *newSimulator(Parameters *parameters, Rules *rules, Strategy *strategy, int core);
void simulatorDelete(Simulator *simulator);
Report *getReport(Simulator *simulator);
void *simulatorRunOnce(void *arg);

#endif // SIMULATOR_H
