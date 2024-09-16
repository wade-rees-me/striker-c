#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <time.h>
#include "table.h"
#include "report.h"
#include "memory.h"

typedef struct {
    char name[100];
    int year;
    int month;
    int day;
    Parameters* parameters;
    Table* table;
    Report report;
} Simulation;

typedef struct {
    char playbook[50];
    char guid[50];
    const char* simulator;
    const char* summary;
    const char* simulations;
    char rounds[20];
    char hands[20];
    char total_bet[20];
    char total_won[20];
    char advantage[20];
    char total_time[20];
    char average_time[50];
    char* parameters;
} SimulationDatabaseTable;

extern Simulation* new_simulation(Parameters *parameters);
extern void simulation_delete(Simulation* sim);
extern void simulator_run_once(Simulation *simulation);
extern void simulator_run_simulation(Simulation *sim);
extern void simulator_insert(SimulationDatabaseTable *sdt, const char *playbook);

#endif // SIMULATOR_H

