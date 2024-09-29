#ifndef TABLE_H
#define TABLE_H

#include <stdbool.h>

#include "player.h"
#include "dealer.h"
#include "report.h"
#include "parameters.h"

typedef struct {
	Parameters* parameters;
	Rules* rules;
	Shoe* shoe;
	Dealer* dealer;
	Player* player;
	Report report;
} Table;

// Function prototypes
Table* newTable(Parameters *parameters, Rules *rules);
void tableSession(Table *table, bool mimic);
Card* tableDealCards(Player* player, Hand* hand, Dealer* dealer, Shoe* shoe);
void tableShow(Table *table, Card *up);

#endif // TABLE_H
