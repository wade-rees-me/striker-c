#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include "constants.h"
#include <stdbool.h>

//
typedef struct {
    long long number_of_hands;
    int number_of_threads;
    bool mimic_flag;
    bool basic_flag;
    bool neural_flag;
    bool linear_flag;
    bool polynomial_flag;
    bool high_low_flag;
    bool wong_flag;
    bool single_deck_flag;
    bool double_deck_flag;
    bool six_shoe_flag;
    char number_of_hands_min[MAX_BUFFER_SIZE];
    char number_of_hands_max[MAX_BUFFER_SIZE];
} Arguments;

// Global functions
Arguments *newArguments(int argc, char *argv[]);
void argumentsDelete(Arguments *arguments);
const char *getStrategy(const Arguments *args);
const char *getDecks(const Arguments *args);
int getNumberOfDecks(const Arguments *args);

#endif // ARGUMENTS_H
