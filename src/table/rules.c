#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "rules.h"

//
void rulesFetchTable(cJSON *json, Rules *rules);

// Function to load table rules by calling FetchRulesTable
Rules *newRules(const char *decks) {
    Rules *rules = (Rules*)malloc(sizeof(Rules));

	char url[MAX_BUFFER_SIZE];
	snprintf(url, MAX_BUFFER_SIZE, "%s/%s", getRulesUrl(), decks);

	requestFetchJson(&rules->request, url);
	rulesFetchTable(rules->request.jsonResponse, rules);

	return rules;
}

//
void rulesDelete(Rules *rules) {
	free(rules);
}

// Function to fetch rules table using libcurl
void rulesFetchTable(cJSON *json, Rules *rules) {
	cJSON *itemPayload = cJSON_GetObjectItemCaseSensitive(json, "payload");
	cJSON *jsonPayload = cJSON_Parse(itemPayload->valuestring);
	if(jsonPayload == NULL) {
		printf("Error parsing JSON response\n");
		exit(0);
	}

	// Extract data from JSON into TableRules struct
	cJSON *playbook = cJSON_GetObjectItemCaseSensitive(jsonPayload, "playbook");
	if (playbook) strcpy(rules->playbook, playbook->valuestring);
	rules->hit_soft_17 = parseAuxBool(jsonPayload, "hitSoft17", false);
	rules->surrender = parseAuxBool(jsonPayload, "surrender", false);
	rules->double_any_two_cards = parseAuxBool(jsonPayload, "doubleAnyTwoCards", false);
	rules->double_after_split = parseAuxBool(jsonPayload, "doubleAfterSplit", false);
	rules->resplit_aces = parseAuxBool(jsonPayload, "resplitAces", false);
	rules->hit_split_aces = parseAuxBool(jsonPayload, "hitSplitAces", false);
	rules->blackjack_bets = parseAuxInt(jsonPayload, "blackjackBets", 0);
	rules->blackjack_pays = parseAuxInt(jsonPayload, "blackjackPays", 0);
	rules->penetration = parseAuxDouble(jsonPayload, "penetration", 0.50);
}

//
void printRules(Rules *rules) {
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
void serializeRules(Rules *rules, char *buffer, int buffer_size) {
	cJSON *json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "hit_soft_17", rules->hit_soft_17 ? "true" : "false");
	cJSON_AddStringToObject(json, "surrender", rules->surrender ? "true" : "false");
	cJSON_AddStringToObject(json, "double_any_two_cards", rules->double_any_two_cards ? "true" : "false");
	cJSON_AddStringToObject(json, "double_after_split", rules->double_after_split ? "true" : "false");
	cJSON_AddStringToObject(json, "resplit_aces", rules->resplit_aces ? "true" : "false");
	cJSON_AddStringToObject(json, "hit_split_aces", rules->hit_split_aces ? "true" : "false");
	cJSON_AddNumberToObject(json, "blackjack_bets", rules->blackjack_bets);
	cJSON_AddNumberToObject(json, "blackjack_pays", rules->blackjack_pays);
	cJSON_AddNumberToObject(json, "penetration", rules->penetration);

	char *jsonString = cJSON_Print(json);
    snprintf(buffer, buffer_size, "%s", jsonString);
	free(jsonString);
	cJSON_Delete(json);
}

