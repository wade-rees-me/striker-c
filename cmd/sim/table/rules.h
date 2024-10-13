#ifndef RULES_H
#define RULES_H

#include <stdbool.h>

// Struct to hold table rules
typedef struct {
	char playbook[64];
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

// Function declarations
void loadRules(Rules *rules, const char *decks);
void printRules(Rules* rules);

#endif // RULES_H
