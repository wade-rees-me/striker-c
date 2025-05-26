#ifndef DEALER_H
#define DEALER_H

#include "hand.h"
#include "shoe.h"
#include <stdbool.h>

// Define the Dealer
typedef struct {
    Hand hand;
    bool hit_soft_17;
} Dealer;

// Global functions
Dealer *newDealer(bool hit_soft_17);
void dealerReset(Dealer *dealer);
Card *dealerDrawCard(Dealer *dealer, Card *card);
bool dealerStand(Dealer *dealer);

#endif // DEALER_H
