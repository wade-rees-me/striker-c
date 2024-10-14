#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdbool.h>
#include <cjson/cJSON.h>

// General constants
#define STRIKER_WHO_AM_I "striker-c"
#define STRIKER_VERSION "v01.02.02"
#define TIME_LAYOUT "%Y-%m-%d %H:%M:%S %z"

// Define the maximum sizesstring fields
#define MAX_STRING_SIZE 128
#define MAX_BUFFER_SIZE 4096
#define MAX_MEMORY_SIZE 1048576

// Simulation constants
#define MAXIMUM_NUMBER_OF_HANDS 1000000000LL
#define MINIMUM_NUMBER_OF_HANDS 100LL
#define DEFAULT_NUMBER_OF_HANDS 1000000LL
#define DATABASE_NUMBER_OF_HANDS 1000000LL
#define MAX_SPLIT_HANDS 18

// Bettting constants
#define MINIMUM_BET 2
#define MAXIMUM_BET 98

//
#define STATUS_DOT 25000
#define STATUS_LINE 1000000

// Structure to store HTTP response
struct MemoryStruct {
    char memory[MAX_MEMORY_SIZE];
    size_t size;
};

// Global functions
const char* getStrategyUrl();
const char* getRulesUrl();
const char* getSimulationUrl();
size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
void removeSpaces(char *str);
const char* boolToString(bool b);
bool parseAuxBool(cJSON *json, char* tag, bool value);
int parseAuxInt(cJSON *json, char* tag, int value);
double parseAuxDouble(cJSON *json, char* tag, double value);

#endif // CONSTANTS_H
