#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "rules.h"
#include "memory.h"
#include "constants.h"

//
bool rulesFetchTable(Rules *rules, const char *url);
const char* boolToString(bool b);
int parseAuxBool(cJSON *json, char* tag, int value);
int parseAuxInt(cJSON *json, char* tag, int value);
double parseAuxDouble(cJSON *json, char* tag, double value);

// Function to load table rules by calling FetchRulesTable
void loadRules(Rules *rules, const char *decks) {
	char url[256];
	snprintf(url, sizeof(url), "%s/%s", getRulesUrl(), decks);

	if (!rulesFetchTable(rules, url)) {
		printf("Error fetching rules table.\n");
		exit(1);
	}
}

// Function to fetch rules table using libcurl
bool rulesFetchTable(Rules *rules, const char *url) {
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;
	chunk.memory = malloc(1);  // Will be grown as needed by realloc
	chunk.size = 0;			// No data at this point

	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();

	if(curl_handle) {
		curl_easy_setopt(curl_handle, CURLOPT_URL, url);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

		res = curl_easy_perform(curl_handle);
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			return false;
		}

		// Process the JSON response
		cJSON *json = cJSON_Parse(chunk.memory);
		if(json == NULL) {
			printf("Error parsing JSON response\n");
			return false;
		}

		cJSON *itemPayload = cJSON_GetObjectItemCaseSensitive(json, "payload");
		json = cJSON_Parse(itemPayload->valuestring);
		if(json == NULL) {
			printf("Error parsing JSON response\n");
			return false;
		}

		// Extract data from JSON into TableRules struct
		cJSON *playbook = cJSON_GetObjectItemCaseSensitive(json, "playbook");
		if (playbook) strcpy(rules->playbook, playbook->valuestring);
		rules->hit_soft_17 = parseAuxBool(json, "hitSoft17", 0);
		rules->surrender = parseAuxBool(json, "surrender", 0);
		rules->double_any_two_cards = parseAuxBool(json, "doubleAnyTwoCards", 0);
		rules->double_after_split = parseAuxBool(json, "doubleAfterSplit", 0);
		rules->resplit_aces = parseAuxBool(json, "resplitAces", 0);
		rules->hit_split_aces = parseAuxBool(json, "hitSplitAces", 0);
		rules->blackjack_bets = parseAuxInt(json, "blackjackBets", 0);
		rules->blackjack_pays = parseAuxInt(json, "blackjackPays", 0);
		rules->penetration = parseAuxDouble(json, "penetration", 0.50);

		// Cleanup
		cJSON_Delete(json);
		curl_easy_cleanup(curl_handle);
		free(chunk.memory);
		curl_global_cleanup();
		return true;
	}
	return false;
}

//
int parseAuxBool(cJSON *json, char* tag, int value) {
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

//
void printRules(Rules* rules, Logger *logger) {
	char buffer[256];

	sprintf(buffer, "    %-24s\n", "Rules");
	Logger_simulation(logger, buffer);
	sprintf(buffer, "      %-24s: %s\n", "Table", rules->playbook);
	Logger_simulation(logger, buffer);
	sprintf(buffer, "      %-24s: %s\n", "Hit soft 17", boolToString(rules->hit_soft_17));
	Logger_simulation(logger, buffer);
	sprintf(buffer, "      %-24s: %s\n", "Surrender", boolToString(rules->surrender));
	Logger_simulation(logger, buffer);
	sprintf(buffer, "      %-24s: %s\n", "Double any two cards", boolToString(rules->double_any_two_cards));
	Logger_simulation(logger, buffer);
	sprintf(buffer, "      %-24s: %s\n", "Double after split", boolToString(rules->double_after_split));
	Logger_simulation(logger, buffer);
	sprintf(buffer, "      %-24s: %s\n", "Resplit aces", boolToString(rules->resplit_aces));
	Logger_simulation(logger, buffer);
	sprintf(buffer, "      %-24s: %s\n", "Hit split aces", boolToString(rules->hit_split_aces));
	Logger_simulation(logger, buffer);
	sprintf(buffer, "      %-24s: %d\n", "Blackjack bets", rules->blackjack_bets);
	Logger_simulation(logger, buffer);
	sprintf(buffer, "      %-24s: %d\n", "Blackjack pays", rules->blackjack_pays);
	Logger_simulation(logger, buffer);
	sprintf(buffer, "      %-24s: %0.3f %%\n", "Penetration", rules->penetration);
	Logger_simulation(logger, buffer);
}

// Function to convert bool to string
const char* boolToString(bool b) {
	return b ? "true" : "false";
}

