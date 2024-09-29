#ifndef CONSTANTS_H
#define CONSTANTS_H

// General constants
#define STRIKER_WHO_AM_I "striker-c"
#define STRIKER_VERSION "v01.00.01"
#define TIME_LAYOUT "%Y-%m-%d %H:%M:%S %z"

// Simulation constants
#define MAXIMUM_NUMBER_OF_ROUNDS 1000000000LL
#define MINIMUM_NUMBER_OF_ROUNDS 100LL
#define DEFAULT_NUMBER_OF_ROUNDS 1000000LL
#define MAX_SPLIT_HANDS 18

// Bettting constants
#define MINIMUM_BET 2
#define MAXIMUM_BET 98

// Function declarations for getting environment variables
const char* getStrategyUrl();
const char* getStrategyMlbUrl();
const char* getRulesUrl();
const char* getSimulationUrl();
const char* getSimulationDirectory();
int64_t getDatabaseMinimumRounds();

#endif // CONSTANTS_H
