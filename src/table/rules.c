#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "rules.h"

//
bool rulesFetchTable(Rules *rules, const char *url);

// Function to load table rules by calling FetchRulesTable
Rules* newRules(const char *decks) {
    Rules *rules = (Rules*)malloc(sizeof(Rules));

	char url[MAX_BUFFER_SIZE];
	snprintf(url, MAX_BUFFER_SIZE, "%s/%s", getRulesUrl(), decks);

	if (!rulesFetchTable(rules, url)) {
		printf("Error fetching rules table.\n");
		exit(1);
	}

	return rules;
}

//
void rulesDelete(Rules* rules) {
	free(rules);
}

// Function to fetch rules table using libcurl
bool rulesFetchTable(Rules *rules, const char *url) {
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;
	chunk.size = 0;

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
		rules->hit_soft_17 = parseAuxBool(json, "hitSoft17", false);
		rules->surrender = parseAuxBool(json, "surrender", false);
		rules->double_any_two_cards = parseAuxBool(json, "doubleAnyTwoCards", false);
		rules->double_after_split = parseAuxBool(json, "doubleAfterSplit", false);
		rules->resplit_aces = parseAuxBool(json, "resplitAces", false);
		rules->hit_split_aces = parseAuxBool(json, "hitSplitAces", false);
		rules->blackjack_bets = parseAuxInt(json, "blackjackBets", 0);
		rules->blackjack_pays = parseAuxInt(json, "blackjackPays", 0);
		rules->penetration = parseAuxDouble(json, "penetration", 0.50);

		// Cleanup
		cJSON_Delete(json);
		curl_easy_cleanup(curl_handle);
		curl_global_cleanup();
		return true;
	}
	return false;
}

//
void printRules(Rules* rules) {
	printf("    %-24s\n", "Table Rules");
	printf("      %-24s: %s\n", "Table", rules->playbook);
	printf("      %-24s: %s\n", "Hit soft 17", boolToString(rules->hit_soft_17));
	printf("      %-24s: %s\n", "Surrender", boolToString(rules->surrender));
	printf("      %-24s: %s\n", "Double any two cards", boolToString(rules->double_any_two_cards));
	printf("      %-24s: %s\n", "Double after split", boolToString(rules->double_after_split));
	printf("      %-24s: %s\n", "Resplit aces", boolToString(rules->resplit_aces));
	printf("      %-24s: %s\n", "Hit split aces", boolToString(rules->hit_split_aces));
	printf("      %-24s: %d\n", "Blackjack bets", rules->blackjack_bets);
	printf("      %-24s: %d\n", "Blackjack pays", rules->blackjack_pays);
	printf("      %-24s: %0.3f %%\n", "Penetration", rules->penetration);
}

//
void serializeRules(Rules *rules, char* buffer, int buffer_size) {
	cJSON* json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "hit_soft_17", rules->hit_soft_17 ? "true" : "false");
	cJSON_AddStringToObject(json, "surrender", rules->surrender ? "true" : "false");
	cJSON_AddStringToObject(json, "double_any_two_cards", rules->double_any_two_cards ? "true" : "false");
	cJSON_AddStringToObject(json, "double_after_split", rules->double_after_split ? "true" : "false");
	cJSON_AddStringToObject(json, "resplit_aces", rules->resplit_aces ? "true" : "false");
	cJSON_AddStringToObject(json, "hit_split_aces", rules->hit_split_aces ? "true" : "false");
	cJSON_AddNumberToObject(json, "blackjack_bets", rules->blackjack_bets);
	cJSON_AddNumberToObject(json, "blackjack_pays", rules->blackjack_pays);
	cJSON_AddNumberToObject(json, "penetration", rules->penetration);

	char* jsonString = cJSON_Print(json);
    snprintf(buffer, buffer_size, "%s", jsonString);
	free(jsonString);
	cJSON_Delete(json);
}

