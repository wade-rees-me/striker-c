#ifndef TABLE_H
#define TABLE_H

#include <stdbool.h>
#include "player.h"
#include "dealer.h"
#include "report.h"
#include "parameters.h"
#include "strategy.h"

//
typedef struct {
	Parameters* parameters;
	Rules* rules;
	Strategy* strategy;
	Shoe* shoe;
	Dealer* dealer;
	Player* player;
	Report report;
} Table;

// Global functions
Table* newTable(Parameters *parameters, Rules *rules, Strategy* strategy);
void tableSession(Table *table, bool mimic);
Card* tableDealCards(Player* player, Hand* hand, Dealer* dealer, Shoe* shoe);
void tableShow(Table *table, Card *up);

#endif // TABLE_H
