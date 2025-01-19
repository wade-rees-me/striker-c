#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "table.h"
#include "report.h"
#include "rules.h"
#include "strategy.h"
#include "parameters.h"
#include "simulation.h"
#include "constants.h"

//
typedef struct {
	Parameters *parameters;
	Rules *rules;
	Strategy *strategy;
	Table *table;
	Report report;
} Simulator;

// Global functions
Simulator *newSimulator(Parameters *parameters, Rules *rules, Strategy *strategy);
void simulatorDelete(Simulator *simulator);
void simulatorRunOnce(Simulator *simulator);
void simulatorRunSimulation(Simulator *simulator);
void simulatorInsert(Simulation *simulation, const char *playbook);

#endif // SIMULATOR_H
