#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cjson/cJSON.h>
#include <stdbool.h>

// General constants
#define STRIKER_WHO_AM_I "striker-c"
#define STRIKER_VERSION "v3.00.00" // Major.Minor.Patch
#define TIME_LAYOUT "%Y-%m-%d %H:%M:%S %z"
#define STATUS_ROUNDS 1000000
#define MY_HOSTNAME "Striker"

//
#define NUMBER_OF_CARDS_IN_DECK 52
#define NUMBER_OF_CORES_PHYSICAL 24
#define NUMBER_OF_CORES_LOGICAL 32
#define NUMBER_OF_CORES_DEFAULT 24

// Define the maximum size string fields
#define SAFE_BUFFER_SIZE 1024
#define MAX_STRING_SIZE 512
#define MAX_BUFFER_SIZE 8192
#define MAX_MEMORY_SIZE 536870912

// Simulation constants
#define MILLION 1000000LL
#define BILLION MILLION * 1000LL
#define NUMBER_OF_HANDS_MAXIMUM BILLION * 10LL
#define NUMBER_OF_HANDS_MINIMUM 1000LL
#define NUMBER_OF_HANDS_DEFAULT MILLION * 100LL
#define NUMBER_OF_HANDS_DATABASE MILLION * 100LL

// Betting constants
#define MINIMUM_BET 2
#define MAXIMUM_BET 20
#define TRUE_COUNT_BET 2
#define TRUE_COUNT_MULTIPLIER 26
#define MAX_SPLIT_HANDS 18

// String constants
#define SINGLE_DECK "single-deck"
#define DOUBLE_DECK "double-deck"
#define SIX_SHOE "six-shoe"

#define MIMIC "mimic"
#define POLYNOMIAL "polynomial"
#define LINEAR "linear"
#define NEURAL "neural"
#define HIGH_LOW "high-low"
#define WONG "wong"
#define BASIC "basic"

// Global functions
int is_my_computer();
const char *getChartsUrl();
const char *getRulesUrl();
const char *getSimulationUrl();
const char *boolToString(bool b);
void unescape_json(char *str);
void strip_quotes(char *str);
void toUpperString(char *str);
void removeAllSubstrings(char *str, const char *sub);
bool parseAuxBool(cJSON *json, char *tag, bool value);
int parseAuxInt(cJSON *json, char *tag, int value);
double parseAuxDouble(cJSON *json, char *tag, double value);
char *convertToStringWithCommas(long long number, char *buffer, size_t bufferSize);

#endif // CONSTANTS_H
