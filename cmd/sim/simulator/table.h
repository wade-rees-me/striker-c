#ifndef TABLE_H
#define TABLE_H

#include "player.h"
#include "dealer.h"
#include "report.h"

typedef struct {
    Parameters* parameters;
    Shoe* shoe;
    Dealer* dealer;
    Player* player;
    Report report;
} Table;

// Function prototypes
extern Table* new_table(Parameters *parameters);
extern void table_session(Table *table);
extern void table_session_mimic(Table *table);
extern Card* table_deal_cards(Player* player, Hand* hand, Dealer* dealer, Shoe* shoe);
extern void table_show(Table *table, Card *up);

#endif // TABLE_H
