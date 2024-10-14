#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "constants.h"

//
const char* getStrategyUrl() {
	return getenv("STRIKER_URL_ACE");
}

//
const char* getRulesUrl() {
	return getenv("STRIKER_URL_RULES");
}

//
const char* getSimulationUrl() {
	return getenv("STRIKER_URL_SIMULATION");
}

// Callback function for libcurl to write data
size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct*)userp;

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

//
void removeSpaces(char *str) {
    int i = 0, j = 0;

    while (str[i]) {
        if (str[i] != ' ') {
            str[j++] = str[i];
        }
        i++;
    }
    str[j] = '\0';
}

// Function to convert bool to string
const char* boolToString(bool b) {
    return b ? "true" : "false";
}

//
bool parseAuxBool(cJSON *json, char* tag, bool value) {
	cJSON *item = cJSON_GetObjectItem(json, tag);
	if (item != NULL) {
		return item->valueint == 1;
	}
	return value;
}

//
int parseAuxInt(cJSON *json, char* tag, int value) {
	cJSON *item = cJSON_GetObjectItem(json, tag);
	if (item != NULL) {
		return item->valueint;
	}
	return value;
}

//
double parseAuxDouble(cJSON *json, char* tag, double value) {
	cJSON *item = cJSON_GetObjectItem(json, tag);
	if (item != NULL) {
		return item->valuedouble;
	}
	return value;
}

