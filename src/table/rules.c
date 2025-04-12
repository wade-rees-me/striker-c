#include "rules.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

//
void rulesInitTable(cJSON *json, Rules *rules);

// Function to load table rules
Rules *newRules(const char *decks) {
    Rules *rules = (Rules *)malloc(sizeof(Rules));

    char url[MAX_BUFFER_SIZE];
    snprintf(url, MAX_BUFFER_SIZE, "%s/%s", getRulesUrl(), decks);

    requestFetchJson(&rules->request, url);
    rulesInitTable(rules->request.jsonResponse, rules);

    return rules;
}

//
void rulesDelete(Rules *rules) { free(rules); }

//
void rulesInitTable(cJSON *json, Rules *rules) {
    cJSON *playbook = cJSON_GetObjectItemCaseSensitive(json, "playbook");
    strcpy(rules->playbook, playbook->valuestring);
    rules->hit_soft_17 = parseAuxBool(json, "hitSoft17", false);
    rules->surrender = parseAuxBool(json, "surrender", false);
    rules->double_any_two_cards = parseAuxBool(json, "doubleAnyTwoCards", false);
    rules->double_after_split = parseAuxBool(json, "doubleAfterSplit", false);
    rules->resplit_aces = parseAuxBool(json, "resplitAces", false);
    rules->hit_split_aces = parseAuxBool(json, "hitSplitAces", false);
    rules->blackjack_bets = parseAuxInt(json, "blackjackBets", 0);
    rules->blackjack_pays = parseAuxInt(json, "blackjackPays", 0);
    rules->penetration = parseAuxDouble(json, "penetration", 0.50);
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

