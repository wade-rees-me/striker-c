#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "arguments.h"
#include "constants.h"
#include <stdint.h>
#include <time.h>

//
typedef struct {
    char name[MAX_STRING_SIZE];
    char playbook[MAX_STRING_SIZE * 2];
    char decks[MAX_STRING_SIZE];
    char strategy[MAX_STRING_SIZE];
    char processor[MAX_STRING_SIZE];
    char epoch[MAX_STRING_SIZE];
    int64_t number_of_hands;
    int64_t share_of_hands;
    int number_of_decks;
    int number_of_threads;
    bool verbose;
} Parameters;

// Global functions
Parameters *newParameters(Arguments *arguments);
void parametersDelete(Parameters *parameters);
void printParameters(const Parameters *parameters);

#endif // PARAMETERS_H
