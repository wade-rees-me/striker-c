#ifndef RULES_H
#define RULES_H

#include <stdbool.h>
#include "request.h"
#include "constants.h"

// Struct to hold table rules
typedef struct {
	Request request;

	char playbook[MAX_STRING_SIZE];
	bool hit_soft_17;
	bool surrender;
	bool double_any_two_cards;
	bool double_after_split;
	bool resplit_aces;
	bool hit_split_aces;
	int blackjack_bets;
	int blackjack_pays;
	float penetration;
} Rules;

// Global functions
Rules* newRules(const char *decks);
void rulesDelete(Rules *rules);
void printRules(Rules *rules);
void serializeRules(Rules *rules, char *buffer, int buffer_size);

#endif // RULES_H
