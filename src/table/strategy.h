#ifndef STRATEGY_H
#define STRATEGY_H

#include <stdbool.h>
#include "request.h"
#include "chart.h"
#include "card.h"

//
typedef struct {
	Request request;
	char Playbook[MAX_STRING_SIZE];
	char Insurance[MAX_STRING_SIZE];
	Chart SoftDouble;
	Chart HardDouble;
	Chart PairSplit;
	Chart SoftStand;
	Chart HardStand;
	int Counts[MAXIMUM_CARD_VALUE + 1];
	int number_of_cards;
} Strategy;

// Function prototypes
Strategy *newStrategy(const char *decks, const char *strategy, int number_of_cards);
int strategyGetBet(Strategy *strat, const int *seenCards);
bool strategyGetInsurance(Strategy *strat, const int *seenCards);
bool strategyGetDouble(Strategy *strat, const int *seenCards, const int total, bool soft, Card *up);
bool strategyGetSplit(Strategy *strat, const int *seenCards, Card *pair, Card *up);
bool strategyGetStand(Strategy *strat, const int *seenCards, const int total, bool soft, Card *up);

#endif // STRATEGY_H
