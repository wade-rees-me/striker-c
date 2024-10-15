#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdbool.h>
#include "constants.h"

//
typedef struct {
	long long number_of_hands;
	bool mimic_flag;
	bool basic_flag;
	bool linear_flag;
	bool polynomial_flag;
	bool high_low_flag;
	bool wong_flag;
	bool striker_flag;
	bool single_deck_flag;
	bool double_deck_flag;
	bool six_shoe_flag;
} Arguments;

// Global functions
Arguments* newArguments(int argc, char *argv[]);
void argumentsDelete(Arguments* arguments);
const char* getStrategy(const Arguments *args);
const char* getDecks(const Arguments *args);
int getNumberOfDecks(const Arguments *args);

#endif // ARGUMENTS_H
