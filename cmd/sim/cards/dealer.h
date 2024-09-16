#ifndef DEALER_H
#define DEALER_H

#include "hand.h"

// Define the Dealer struct
typedef struct {
    Hand hand;
} Dealer;

// Function declarations
Dealer* new_dealer();
void dealer_reset(Dealer* dealer);
void dealer_play(Dealer* dealer, Shoe* shoe);
Card* dealer_draw_card(Dealer* dealer, Card* card);

#endif // DEALER_H

