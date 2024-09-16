#ifndef CONSTANTS_H
#define CONSTANTS_H

// General constants
#define STRIKER_VERSION "v01.00.01"
#define TIME_LAYOUT "%Y-%m-%d %H:%M:%S %z"

// Simulation constants
#define MAX_NUMBER_OF_ROUNDS 1000000000LL
#define MIN_NUMBER_OF_ROUNDS 100LL
#define DEFAULT_NUMBER_OF_ROUNDS 1000000LL
#define MAX_SPLIT_HANDS 3
#define STRIKER_WHO_AM_I "striker-c"

#define MINIMUM_BET 2
#define MAXIMUM_BET 98

// Function declarations for getting environment variables
extern const char* get_strategy_url();
extern const char* get_strategy_mlb_url();
extern const char* get_rules_url();
extern const char* get_simulation_url();

#endif // CONSTANTS_H

