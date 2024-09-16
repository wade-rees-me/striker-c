#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arguments_simulation.h"
#include "constants.h"

// Define the global simulation argument structure
SimulationArgs CLSimulation = {DEFAULT_NUMBER_OF_ROUNDS, "3:2", 0.75};

void simulation_parse_flags(int argc, char *argv[]) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-number-of-rounds") == 0 && i + 1 < argc) {
			CLSimulation.rounds = atoll(argv[++i]);
			if (CLSimulation.rounds < MIN_NUMBER_OF_ROUNDS || CLSimulation.rounds > MAX_NUMBER_OF_ROUNDS) {
				printf("Number of rounds must be between %lld and %lld\n", MIN_NUMBER_OF_ROUNDS, MAX_NUMBER_OF_ROUNDS);
				exit(EXIT_FAILURE);
			}
		} else if (strcmp(argv[i], "-table-blackjack-pays") == 0 && i + 1 < argc) {
			strncpy(CLSimulation.blackjack_pays, argv[++i], sizeof(CLSimulation.blackjack_pays) - 1);
			CLSimulation.blackjack_pays[sizeof(CLSimulation.blackjack_pays) - 1] = '\0'; // Ensure null-termination
		} else if (strcmp(argv[i], "-table-penetration") == 0 && i + 1 < argc) {
			CLSimulation.penetration = atof(argv[++i]);
		}
	}
}

