#include <stdio.h>
#include <stdlib.h>
#include "parameters.h"

// Local functions
void getCurrentTime(char *buffer, long bufferSize);
void generateName(char *buffer);

//
Parameters *newParameters(const char *decks, const char *strategy, int number_of_decks, int64_t number_of_hands) {
	Parameters *parameters = (Parameters*)malloc(sizeof(Parameters));

	generateName(parameters->name);
	snprintf(parameters->decks, MAX_STRING_SIZE, "%s", decks);
	snprintf(parameters->strategy, MAX_STRING_SIZE, "%s", strategy);
	parameters->number_of_decks = number_of_decks;
	parameters->number_of_hands = number_of_hands;
	snprintf(parameters->playbook, MAX_STRING_SIZE, "%s-%s", parameters->decks, parameters->strategy);
	snprintf(parameters->processor, MAX_STRING_SIZE, "%s", STRIKER_WHO_AM_I);
	getCurrentTime(parameters->timestamp, sizeof(parameters->timestamp));

	return parameters;
}

//
void parametersDelete(Parameters *parameters) {
	free(parameters);
}

// Print the Parameters struct
void printParameters(const Parameters *parameters) {
	char buffer[MAX_BUFFER_SIZE];

	printf("    %-24s: %s\n", "Name", parameters->name);
	printf("    %-24s: %s\n", "Playbook", parameters->playbook);
	printf("    %-24s: %s\n", "Processor", parameters->processor);
	printf("    %-24s: %s\n", "Version", STRIKER_VERSION);
	printf("    %-24s: %s\n", "Number of hands", convertToStringWithCommas(parameters->number_of_hands, buffer, MAX_BUFFER_SIZE));
	printf("    %-24s: %s\n", "Timestamp", parameters->timestamp);
}

//
void serializeParameters(Parameters *parameters, char *buffer, int buffer_size) {
	cJSON *json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "playbook", parameters->playbook);
	cJSON_AddStringToObject(json, "name", parameters->name);
	cJSON_AddStringToObject(json, "processor", parameters->processor);
	cJSON_AddStringToObject(json, "timestamp", parameters->timestamp);
	cJSON_AddStringToObject(json, "decks", parameters->decks);
	cJSON_AddStringToObject(json, "strategy", parameters->strategy);
	cJSON_AddNumberToObject(json, "hands", parameters->number_of_hands);
	cJSON_AddNumberToObject(json, "number_of_decks", parameters->number_of_decks);

	char *jsonString = cJSON_Print(json);
	snprintf(buffer, buffer_size, "%s", jsonString);
	free(jsonString);
	cJSON_Delete(json);
}

//
void getCurrentTime(char *buffer, long bufferSize) {
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, bufferSize, TIME_LAYOUT, timeinfo);
}

//
void generateName(char *buffer) {
	time_t t = time(NULL);
	struct tm *tm_info = localtime(&t);

	int year = tm_info->tm_year + 1900;
	int month = tm_info->tm_mon + 1;
	int day = tm_info->tm_mday;

	snprintf(buffer, MAX_STRING_SIZE, "%s_%4d_%02d_%02d_%012ld", STRIKER_WHO_AM_I, year, month, day, (long)t);
}

