#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>
#include <time.h>
#include "rules.h"

// Define the maximum size for string fields
#define MAX_STRING_SIZE 128

// Parameters struct to store simulation parameters
typedef struct {
	Rules *rules;

	const char* name;
	char playbook[MAX_STRING_SIZE];
	char processor[MAX_STRING_SIZE];
	char timestamp[MAX_STRING_SIZE];
	char decks[MAX_STRING_SIZE];
	char strategy[MAX_STRING_SIZE];
	int number_of_decks;
	int64_t number_of_hands;
} Parameters;

// Function prototypes
void initParameters(const char* name, Parameters *params, Rules *rules, const char *d, const char *s, int n, int64_t r);
void printParameters(const Parameters *params);
void generateUUID(char *buffer);
void getCurrentTime(char *buffer);
char* serializeParameters(Parameters *parameters);

#endif // PARAMETERS_H
