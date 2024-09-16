#ifndef ARGUMENTS_SIMULATION_H
#define ARGUMENTS_SIMULATION_H

#include <stdint.h>

// Define the simulation structure in C
typedef struct {
    int64_t rounds;
    char blackjack_pays[10];  // To store "3:2" as a string
    double penetration;
} SimulationArgs;

// Declare the global simulation argument structure
extern SimulationArgs CLSimulation;

// Function declarations
extern void simulation_parse_flags(int argc, char *argv[]);

#endif // ARGUMENTS_SIMULATION_H

