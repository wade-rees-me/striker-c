#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "constants.h"

//
const char *getStrategyUrl() {
	return getenv("STRIKER_URL_STRATEGY");
}

//
const char *getRulesUrl() {
	return getenv("STRIKER_URL_RULES");
}

//
const char *getSimulationUrl() {
	return getenv("STRIKER_URL_SIMULATION");
}

// Function to convert bool to string
const char* boolToString(bool b) {
    return b ? "true" : "false";
}

//
void toUpperString(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

//
bool parseAuxBool(cJSON *json, char *tag, bool value) {
	cJSON *item = cJSON_GetObjectItem(json, tag);
	if (item != NULL) {
		return item->valueint == 1;
	}
	return value;
}

//
int parseAuxInt(cJSON *json, char *tag, int value) {
	cJSON *item = cJSON_GetObjectItem(json, tag);
	if (item != NULL) {
		return item->valueint;
	}
	return value;
}

//
double parseAuxDouble(cJSON *json, char *tag, double value) {
	cJSON *item = cJSON_GetObjectItem(json, tag);
	if (item != NULL) {
		return item->valuedouble;
	}
	return value;
}

