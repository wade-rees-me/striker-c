#ifndef STRATEGY_H
#define STRATEGY_H

#include <stdbool.h>
#include "request.h"
#include "chart.h"
#include "card.h"

#define MAX_VALUES 13
//#define MAX_ENTRIES 22

//
typedef struct {
	Request request;

	char Playbook[MAX_STRING_SIZE];
	int Counts[MAX_VALUES];
	int Bets[MAX_VALUES];
	char Insurance[MAX_STRING_SIZE];

	Chart SoftDouble;
	Chart HardDouble;
	Chart PairSplit;
	Chart SoftStand;
	Chart HardStand;

	int number_of_cards;
} Strategy;

// Function prototypes
Strategy *newStrategy(const char *decks, const char *strategy, int number_of_cards);
int strategyGetBet(Strategy *strat, const int *seenCards);
bool strategyGetInsurance(Strategy *strat, const int *seenCards);
bool strategyGetDouble(Strategy *strat, const int *seenCards, const int total, bool soft, Card *up);
bool strategyGetSplit(Strategy *strat, const int *seenCards, Card *pair, Card *up);
bool strategyGetStand(Strategy *strat, const int *seenCards, const int total, bool soft, Card *up);
void freeStrategy(Strategy *strat);

#endif // STRATEGY_H
