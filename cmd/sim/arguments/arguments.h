#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdbool.h>

// Define the structure to hold arguments
typedef struct {
	long long hands;
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

// Function prototypes
void parseArguments(Arguments *args, int argc, char *argv[]);
void printHelpMessage();
void printVersion();
const char* getStrategy(const Arguments *args);
const char* getDecks(const Arguments *args);
int getNumberOfDecks(const Arguments *args);

#endif // ARGUMENTS_H
