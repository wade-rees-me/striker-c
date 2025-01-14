#ifndef DATABASE_H
#define DATABASE_H

#include "constants.h"

//
typedef struct {
    char playbook[MAX_STRING_SIZE];
    char guid[MAX_STRING_SIZE];
    const char *simulator;
    const char *summary;
    const char *simulations;
    char rounds[MAX_STRING_SIZE];
    char hands[MAX_STRING_SIZE];
    char total_bet[MAX_STRING_SIZE];
    char total_won[MAX_STRING_SIZE];
    char advantage[MAX_STRING_SIZE];
    char total_time[MAX_STRING_SIZE];
    char average_time[MAX_STRING_SIZE];
    char parameters[MAX_BUFFER_SIZE];
    char rules[MAX_BUFFER_SIZE];
} Simulation;

#endif // DATABASE_H
