#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arguments.h"
#include "constants.h"

// Parse command-line flags for simulation
void parseArguments(Arguments *args, int argc, char *argv[]) {
	args->hands = MINIMUM_NUMBER_OF_HANDS;
	args->mimic_flag = false;
	args->basic_flag = false;
	args->linear_flag = false;
	args->polynomial_flag = false;
	args->high_low_flag = false;
	args->wong_flag = false;
	args->striker_flag = false;
	args->single_deck_flag = false;
	args->double_deck_flag = false;
	args->six_shoe_flag = false;

	for (int i = 1; i < argc; ++i) {
		if ((strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--number-of-hands") == 0) && i + 1 < argc) {
			args->hands = atoll(argv[++i]);
			if (args->hands < MINIMUM_NUMBER_OF_HANDS || args->hands > MAXIMUM_NUMBER_OF_HANDS) {
				fprintf(stderr, "Number of hands must be between %lld and %lld\n", MINIMUM_NUMBER_OF_HANDS, MAXIMUM_NUMBER_OF_HANDS);
				exit(EXIT_FAILURE);
			}
		} else if (strcmp(argv[i], "-M") == 0 || strcmp(argv[i], "--mimic") == 0) {
			args->mimic_flag = 1;
		} else if (strcmp(argv[i], "-B") == 0 || strcmp(argv[i], "--basic") == 0) {
			args->basic_flag = 1;
		} else if (strcmp(argv[i], "-L") == 0 || strcmp(argv[i], "--linear") == 0) {
			args->linear_flag = 1;
		} else if (strcmp(argv[i], "-P") == 0 || strcmp(argv[i], "--polynomial") == 0) {
			args->polynomial_flag = 1;
		} else if (strcmp(argv[i], "-H") == 0 || strcmp(argv[i], "--high-low") == 0) {
			args->high_low_flag = 1;
		} else if (strcmp(argv[i], "-W") == 0 || strcmp(argv[i], "--wong") == 0) {
			args->wong_flag = 1;
		} else if (strcmp(argv[i], "-S") == 0 || strcmp(argv[i], "--striker") == 0) {
			args->striker_flag = 1;
		} else if (strcmp(argv[i], "-1") == 0 || strcmp(argv[i], "--single-deck") == 0) {
			args->single_deck_flag = 1;
		} else if (strcmp(argv[i], "-2") == 0 || strcmp(argv[i], "--double-deck") == 0) {
			args->double_deck_flag = 1;
		} else if (strcmp(argv[i], "-6") == 0 || strcmp(argv[i], "--six-shoe") == 0) {
			args->six_shoe_flag = 1;
		} else if (strcmp(argv[i], "--help") == 0) {
			printHelpMessage();
			exit(EXIT_SUCCESS);
		} else if (strcmp(argv[i], "--version") == 0) {
			printVersion();
			exit(EXIT_SUCCESS);
		} else {
			fprintf(stderr, "Error: Invalid argument: %s\n", argv[i]);
			exit(EXIT_FAILURE);
		}
	}
}

// Print version information
void printVersion() {
	printf("%s: version: %s\n", STRIKER_WHO_AM_I, STRIKER_VERSION);
}

// Print help message
void printHelpMessage() {
	printf("Usage: strikerC [options]\n"
		   "Options:\n"
		   "  -h, --help                                Show this help message\n"
		   "  -v, --version                             Display the program version\n"
		   "  -r, --number-of-hands <number of hands>   The number of hands to play in this simulation\n"
		   "  -M, --mimic                               Use the mimic dealer player strategy\n"
		   "  -B, --basic                               Use the basic player strategy\n"
		   "  -L, --linear                              Use the liner regression player strategy\n"
		   "  -P, --polynomial                          Use the polynomial regression player strategy\n"
		   "  -H, --high-low                            Use the high low count player strategy\n"
		   "  -W, --wong                                Use the Wong count player strategy\n"
		   "  -S, --striker                             Use the Striker machine learning player strategy\n"
		   "  -1, --single-deck                         Use a single deck of cards and rules\n"
		   "  -2, --double-deck                         Use a double deck of cards and rules\n"
		   "  -6, --six-shoe                            Use a six deck shoe of cards and rules\n");
}

// Get the current strategy as a string
const char* getStrategy(const Arguments *args) {
	if (args->mimic_flag) {
		return "mimic";
	}
	if (args->polynomial_flag) {
		return "polynomial";
	}
	if (args->linear_flag) {
		return "linear";
	}
	if (args->high_low_flag) {
		return "high-low";
	}
	if (args->wong_flag) {
		return "wong";
	}
	if (args->striker_flag) {
		return "striker";
	}
	return "basic";
}

// Get the type of deck being used
const char* getDecks(const Arguments *args) {
	if (args->double_deck_flag) {
		return "double-deck";
	}
	if (args->six_shoe_flag) {
		return "six-shoe";
	}
	return "single-deck";
}

// Get the number of decks being used
int getNumberOfDecks(const Arguments *args) {
	if (args->double_deck_flag) {
		return 2;
	}
	if (args->six_shoe_flag) {
		return 6;
	}
	return 1;
}

