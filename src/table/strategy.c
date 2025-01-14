#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strategy.h"
#include "constants.h"
#include "cjson/cJSON.h"

//
void strategyFetchTable(const char *decks, const char *strategy, cJSON *json, Strategy *table);
void strategyLoadTable(cJSON *strategy, Chart *chart);
int getRunningCount(Strategy *strat, const int *seenCards);
int getTrueCount(Strategy *strat, const int *seenCards, int runningCount);
bool processValue(const char *value, int trueCount, bool missing_value);

// Function to create a new Strategy
Strategy *newStrategy(const char *decks, const char *playbook, int number_of_cards) {
	Strategy *strategy = (Strategy*)malloc(sizeof(Strategy));

	strategy->number_of_cards = number_of_cards;
	initChart(&strategy->SoftDouble, "Soft Double");
	initChart(&strategy->HardDouble, "Hard Double");
	initChart(&strategy->PairSplit, "Pair Split");
	initChart(&strategy->SoftStand, "Soft Stand");
	initChart(&strategy->HardStand, "Hard Stand");

	if (strcasecmp("mimic", playbook) != 0) {
		requestFetchJson(&strategy->request, getStrategyUrl());
		strategyFetchTable(decks, playbook, strategy->request.jsonResponse, strategy);

		chartPrint(&strategy->SoftDouble);
		chartPrint(&strategy->HardDouble);
		chartPrint(&strategy->PairSplit);
		chartPrint(&strategy->SoftStand);
		chartPrint(&strategy->HardStand);
		countPrint(strategy->Counts);
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
	const char *value = soft ? chartGetValueByTotal(&strategy->SoftDouble, total, cardGetValue(up)) : chartGetValueByTotal(&strategy->HardDouble, total, cardGetValue(up));
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
	const char *value = soft ? chartGetValueByTotal(&strategy->SoftStand, total, cardGetValue(up)) : chartGetValueByTotal(&strategy->HardStand, total, cardGetValue(up));
	return processValue(value, trueCount, false);
}

//
void strategyFetchTable(const char *decks, const char *strategy, cJSON *json, Strategy *table) {
	cJSON *item;
	cJSON_ArrayForEach(item, json) {
		cJSON *playbookJson = cJSON_GetObjectItem(item, "playbook");
		cJSON *handJson = cJSON_GetObjectItem(item, "hand");

		if (playbookJson != NULL && handJson != NULL && strcmp(decks, playbookJson->valuestring) == 0 && strcmp(strategy, handJson->valuestring) == 0) {
			cJSON *payloadJson = cJSON_GetObjectItem(item, "payload");
			if (payloadJson == NULL) {
				printf("Error fetching strategy table payload\n");
				cJSON_Delete(json);
				exit(-1);
			}

			cJSON *payload = cJSON_Parse(payloadJson->valuestring);
			if (payload == NULL) {
				printf("Error parsing payload\n");
				printf("Payload: %s\n", payloadJson->valuestring);
				cJSON_Delete(json);
				exit(-1);
			}

			// Set Playbook
			cJSON *playbook = cJSON_GetObjectItem(payload, "playbook");
			if (playbook != NULL) {
				strncpy(table->Playbook, playbook->valuestring, MAX_STRING_SIZE);
			}

			// Set Counts
			cJSON *counts = cJSON_GetObjectItem(payload, "counts");
			if (counts != NULL) {
				cJSON *countItem;
				int index = MINIMUM_CARD_VALUE;
				cJSON_ArrayForEach(countItem, counts) {
					table->Counts[index++] = countItem->valueint;
				}
			}

			// Set Insurance
			cJSON *insurance = cJSON_GetObjectItem(payload, "insurance");
			if (insurance != NULL) {
				strncpy(table->Insurance, insurance->valuestring, MAX_STRING_SIZE);
			}

			strategyLoadTable(cJSON_GetObjectItem(payload, "soft-double"), &table->SoftDouble);
			strategyLoadTable(cJSON_GetObjectItem(payload, "hard-double"), &table->HardDouble);
			strategyLoadTable(cJSON_GetObjectItem(payload, "pair-split"), &table->PairSplit);
			strategyLoadTable(cJSON_GetObjectItem(payload, "soft-stand"), &table->SoftStand);
			strategyLoadTable(cJSON_GetObjectItem(payload, "hard-stand"), &table->HardStand);

			cJSON_Delete(payload);
			break; // We've found and processed the relevant item, no need to loop further
		}
	}

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
	if (strcasecmp(value, "yes") == 0 || strcasecmp(value, "y") == 0) {
		return true;
	}
	if (strcasecmp(value, "no") == 0 || strcasecmp(value, "n") == 0) {
		return false;
	}
	if (value[0] == 'R' || value[0] == 'r') {
		return trueCount <= atoi(&value[1]);
	}
	return trueCount >= atoi(value);
}

