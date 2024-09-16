#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "arguments_strategy.h"

// Global strategy structure
StrategyArgs CLStrategy = {false, false, false, false, false, false, false};

// Function to parse the command-line flags
void strategy_parse_flags(int argc, char *argv[]) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-strategy-mimic") == 0) {
			CLStrategy.mimic_flag = true;
		} else if (strcmp(argv[i], "-strategy-basic") == 0) {
			CLStrategy.basic_flag = true;
		} else if (strcmp(argv[i], "-strategy-linear") == 0) {
			CLStrategy.linear_flag = true;
		} else if (strcmp(argv[i], "-strategy-polynomial") == 0) {
			CLStrategy.polynomial_flag = true;
		} else if (strcmp(argv[i], "-strategy-high-low") == 0) {
			CLStrategy.high_low_flag = true;
		} else if (strcmp(argv[i], "-strategy-wong") == 0) {
			CLStrategy.wong_flag = true;
		} else if (strcmp(argv[i], "-strategy-striker") == 0) {
			CLStrategy.striker_flag = true;
		}
	}
}

// Function to get the current strategy as a string
const char* strategy_get() {
	if (CLStrategy.mimic_flag) {
		return "mimic";
	}
	if (CLStrategy.polynomial_flag) {
		return "polynomial";
	}
	if (CLStrategy.linear_flag) {
		return "linear";
	}
	if (CLStrategy.high_low_flag) {
		return "high-low";
	}
	if (CLStrategy.wong_flag) {
		return "wong";
	}
	if (CLStrategy.striker_flag) {
		return "striker";
	}
	return "basic";
}

