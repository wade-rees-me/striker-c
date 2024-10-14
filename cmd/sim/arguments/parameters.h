#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>
#include <time.h>
#include "constants.h"

//
typedef struct {
	char name[MAX_STRING_SIZE];
	char playbook[MAX_STRING_SIZE];
	char processor[MAX_STRING_SIZE];
	char timestamp[MAX_STRING_SIZE];
	char decks[MAX_STRING_SIZE];
	char strategy[MAX_STRING_SIZE];
	int number_of_decks;
	int64_t number_of_hands;
} Parameters;

// Global functions
Parameters* newParameters(const char *decks, const char *strategy, int number_of_decks, int64_t number_of_hands);
void parametersDelete(Parameters* parameters);
void printParameters(const Parameters *params);
void serializeParameters(Parameters *parameters, char* buffer, int buffer_size);

#endif // PARAMETERS_H
