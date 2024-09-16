#ifndef WAGER_H
#define WAGER_H

#include <stdbool.h>
#include <stdint.h>
#include "hand.h" // Include the Hand struct

// Define the Wager struct
typedef struct {
    Hand hand;           // The hand associated with the wager
    int64_t amount_bet;  // The amount of the initial bet
    int64_t amount_won;  // The amount won from the wager
    int64_t double_bet;  // The amount of the double bet
    int64_t double_won;  // The amount won from the double bet
    int64_t insurance_bet;
    int64_t insurance_won;
} Wager;

// Function declarations
extern void wager_reset(Wager* wager);
extern void wager_split(Wager* wager, Wager* split);
extern void wager_place_bet(Wager* wager, int64_t bet);
extern void wager_double_bet(Wager* wager);
extern bool wager_is_blackjack(Wager* wager);
extern void wager_won_blackjack(Wager* wager, int64_t pays, int64_t bet);
extern void wager_won(Wager* wager);
extern void wager_lost(Wager* wager);
extern void wager_push(Wager* wager);
extern void wager_won_insurance(Wager* wager);
extern void wager_lost_insurance(Wager* wager);

#endif // WAGER_H

