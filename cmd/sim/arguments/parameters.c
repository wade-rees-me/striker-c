#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <uuid/uuid.h>
#include <cjson/cJSON.h>
#include "parameters.h"
#include "constants.h"
#include "utilities.h"

// Initialize the Parameters struct
void initParameters(const char* name, Parameters *params, Rules *rules, const char *d, const char *s, int n, int64_t number_of_hands) {
	params->name = name;
	params->rules = rules;
	snprintf(params->decks, MAX_STRING_SIZE, "%s", d);
	snprintf(params->strategy, MAX_STRING_SIZE, "%s", s);
	params->number_of_decks = n;
	params->number_of_hands = number_of_hands;
	snprintf(params->playbook, MAX_STRING_SIZE, "%s-%s", params->decks, params->strategy);
	snprintf(params->processor, MAX_STRING_SIZE, "%s", STRIKER_WHO_AM_I);
	getCurrentTime(params->timestamp);
}

// Print the Parameters struct
void printParameters(const Parameters *params) {
	printf("    %-24s: %s\n", "Name", params->name);
	printf("    %-24s: %s\n", "Playbook", params->playbook);
	printf("    %-24s: %s\n", "Processor", params->processor);
	printf("    %-24s: %s\n", "Version", STRIKER_VERSION);
	printf("    %-24s: %s\n", "Number of hands", addCommas(params->number_of_hands));
	printf("    %-24s: %s\n", "Timestamp", params->timestamp);
}

// Function to get the current time and format it
void getCurrentTime(char *buffer) {
	time_t t = time(NULL);
	struct tm *tm_info = localtime(&t);
	strftime(buffer, MAX_STRING_SIZE, TIME_LAYOUT, tm_info);  // Use TIME_LAYOUT from constants.h
}

char* serializeParameters(Parameters *parameters) {
	cJSON* json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "playbook", parameters->playbook);
	cJSON_AddStringToObject(json, "name", parameters->name);
	cJSON_AddStringToObject(json, "processor", parameters->processor);
	cJSON_AddStringToObject(json, "timestamp", parameters->timestamp);
	cJSON_AddStringToObject(json, "decks", parameters->decks);
	cJSON_AddStringToObject(json, "strategy", parameters->strategy);
	cJSON_AddNumberToObject(json, "hands", parameters->number_of_hands);
	cJSON_AddNumberToObject(json, "number_of_decks", parameters->number_of_decks);

	cJSON_AddStringToObject(json, "hit_soft_17", parameters->rules->hit_soft_17 ? "true" : "false");
	cJSON_AddStringToObject(json, "surrender", parameters->rules->surrender ? "true" : "false");
	cJSON_AddStringToObject(json, "double_any_two_cards", parameters->rules->double_any_two_cards ? "true" : "false");
	cJSON_AddStringToObject(json, "double_after_split", parameters->rules->double_after_split ? "true" : "false");
	cJSON_AddStringToObject(json, "resplit_aces", parameters->rules->resplit_aces ? "true" : "false");
	cJSON_AddStringToObject(json, "hit_split_aces", parameters->rules->hit_split_aces ? "true" : "false");
	cJSON_AddNumberToObject(json, "blackjack_bets", parameters->rules->blackjack_bets);
	cJSON_AddNumberToObject(json, "blackjack_pays", parameters->rules->blackjack_pays);
	cJSON_AddNumberToObject(json, "penetration", parameters->rules->penetration);

	char* result = malloc(2048);
	char* jsonString = cJSON_Print(json);
	strcpy(result, jsonString);

	cJSON_Delete(json);
	free(jsonString);

	return result;
}
