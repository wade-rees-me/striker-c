#define _GNU_SOURCE

#include "strategy.h"
#include "../xlog/xlog.h"
#include "cjson/cJSON.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
void strategyFetchTable(const char *decks, const char *strategy, cJSON *json, Strategy *table);
void strategyLoadTable(cJSON *strategy, Chart *chart);
int getRunningCount(Strategy *strat, const int *seenCards);
int getTrueCount(Strategy *strat, const int *seenCards, int runningCount);
bool processValue(const char *value, int trueCount, bool missing_value);

// Function to create a new Strategy
Strategy *newStrategy(const char *decks, const char *playbook, int number_of_decks) {
    Strategy *strategy = (Strategy *)malloc(sizeof(Strategy));

    strategy->number_of_cards = number_of_decks * NUMBER_OF_CARDS_IN_DECK;
    initChart(&strategy->SoftDouble, "Soft Double");
    initChart(&strategy->HardDouble, "Hard Double");
    initChart(&strategy->PairSplit, "Pair Split");
    initChart(&strategy->SoftStand, "Soft Stand");
    initChart(&strategy->HardStand, "Hard Stand");

    if (strcasecmp("mimic", playbook) != 0) {
        char *url = NULL;
        if (asprintf(&url, "%s/%s/%s", getChartsUrl(), decks, playbook) < 0) {
            xlog_panic(url);
        }

        requestFetchJson(&strategy->request, url);
        strategyFetchTable(decks, playbook, strategy->request.jsonResponse, strategy);

        if (false) {
            chartPrint(&strategy->SoftDouble);
            chartPrint(&strategy->HardDouble);
            chartPrint(&strategy->PairSplit);
            chartPrint(&strategy->SoftStand);
            chartPrint(&strategy->HardStand);
            countPrint(strategy->Counts);
        }
    }

    return strategy;
}

// Get a bet based on seen cards
int strategyGetBet(Strategy *strategy, const int *seenCards) {
    return getTrueCount(strategy, seenCards, getRunningCount(strategy, seenCards)) * TRUE_COUNT_BET;
}

// Get insurance decision
bool strategyGetInsurance(Strategy *strategy, const int *seenCards) {
    int trueCount = getTrueCount(strategy, seenCards, getRunningCount(strategy, seenCards));
    return processValue(strategy->Insurance, trueCount, false);
}

// Determine whether to double
bool strategyGetDouble(Strategy *strategy, const int *seenCards, int total, bool soft, Card *up) {
    int trueCount = getTrueCount(strategy, seenCards, getRunningCount(strategy, seenCards));
    const char *value = soft ? chartGetValueByTotal(&strategy->SoftDouble, total, cardGetValue(up))
                             : chartGetValueByTotal(&strategy->HardDouble, total, cardGetValue(up));
    return processValue(value, trueCount, false);
}

// Determine whether to split
bool strategyGetSplit(Strategy *strategy, const int *seenCards, Card *pair, Card *up) {
    int trueCount = getTrueCount(strategy, seenCards, getRunningCount(strategy, seenCards));
    const char *value = chartGetValue(&strategy->PairSplit, cardGetKey(pair), cardGetValue(up));
    return processValue(value, trueCount, false);
}

// Determine whether to stand
bool strategyGetStand(Strategy *strategy, const int *seenCards, int total, bool soft, Card *up) {
    int trueCount = getTrueCount(strategy, seenCards, getRunningCount(strategy, seenCards));
    const char *value = soft ? chartGetValueByTotal(&strategy->SoftStand, total, cardGetValue(up))
                             : chartGetValueByTotal(&strategy->HardStand, total, cardGetValue(up));
    return processValue(value, trueCount, false);
}

//
void strategyFetchTable(const char *decks, const char *strategy, cJSON *json, Strategy *table) {
    // Set Playbook
    cJSON *playbook = cJSON_GetObjectItem(json, "playbook");
    if (playbook != NULL) {
        // snprintf(table->Playbook, MAX_STRING_SIZE, "%s", playbook->valuestring);
        if (asprintf(&table->Playbook, "%s", playbook->valuestring) < 0) {
            xlog_panic(table->Playbook);
        }
    }

    // Set Counts
    cJSON *counts = cJSON_GetObjectItem(json, "counts");
    if (counts != NULL) {
        cJSON *countItem;
        int index = MINIMUM_CARD_VALUE;
        cJSON_ArrayForEach(countItem, counts) { table->Counts[index++] = countItem->valueint; }
    }

    // Set Insurance
    cJSON *insurance = cJSON_GetObjectItem(json, "insurance");
    if (insurance != NULL) {
        char *buffer = NULL;
        if (asprintf(&buffer, "%s", insurance->valuestring) < 0) {
            xlog_panic(buffer);
        }
    }

    strategyLoadTable(cJSON_GetObjectItem(json, "soft-double"), &table->SoftDouble);
    strategyLoadTable(cJSON_GetObjectItem(json, "hard-double"), &table->HardDouble);
    strategyLoadTable(cJSON_GetObjectItem(json, "pair-split"), &table->PairSplit);
    strategyLoadTable(cJSON_GetObjectItem(json, "soft-stand"), &table->SoftStand);
    strategyLoadTable(cJSON_GetObjectItem(json, "hard-stand"), &table->HardStand);

    cJSON_Delete(json);
}

//
void strategyLoadTable(cJSON *strategy, Chart *chart) {
    if (strategy != NULL) {
        cJSON *key;
        cJSON *valueArray;
        cJSON_ArrayForEach(key, strategy) {
            valueArray = cJSON_GetObjectItem(strategy, key->string);
            if (valueArray != NULL) {
                cJSON *valueItem;
                int index = MINIMUM_CARD_VALUE;
                cJSON_ArrayForEach(valueItem, valueArray) {
                    chartInsert(chart, key->string, index++, valueItem->valuestring);
                }
            }
        }
    }
}

// Calculate running count
int getRunningCount(Strategy *strat, const int *seenCards) {
    int running = 0;
    for (int i = MINIMUM_CARD_VALUE; i <= MAXIMUM_CARD_VALUE; i++) {
        running += strat->Counts[i] * seenCards[i];
    }
    return running;
}

// Calculate true count
int getTrueCount(Strategy *strat, const int *seenCards, int runningCount) {
    int unseen = strat->number_of_cards;
    for (int i = MINIMUM_CARD_VALUE; i <= MAXIMUM_CARD_VALUE; i++) {
        unseen -= seenCards[i];
    }

    if (unseen > 0) {
        return (int)((float)runningCount / ((float)unseen / (float)TRUE_COUNT_MULTIPLIER));
    }

    return 0;
}

// Process string value for decision-making
bool processValue(const char *value, int trueCount, bool missing_value) {
    if (value == NULL || strlen(value) == 0) {
        return missing_value;
    }
    if (strcasecmp(value, "Y") == 0 || strcasecmp(value, "YES") == 0) {
        return true;
    }
    if (strcasecmp(value, "N") == 0 || strcasecmp(value, "NO") == 0) {
        return false;
    }
    if (value[0] == 'R' || value[0] == 'r') {
        return trueCount <= atoi(&value[1]);
    }
    return trueCount >= atoi(value);
}

