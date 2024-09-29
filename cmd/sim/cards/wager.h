#ifndef WAGER_H
#define WAGER_H

#include <stdbool.h>
#include <stdint.h>
#include "hand.h" // Include the Hand struct

// Define the Wager struct
typedef struct {
	Hand hand;				// The hand associated with the wager
	int64_t amount_bet;		// The amount of the initial bet
	int64_t amount_won;		// The amount won from the wager
	int64_t insurance_bet;
	int64_t insurance_won;
} Wager;

// Function declarations
extern void wagerReset(Wager* wager);
extern void wagerSplit(Wager* wager, Wager* split);
extern void wagerPlaceBet(Wager* wager, int64_t bet);
extern void wagerDoubleBet(Wager* wager);
extern bool wagerIsBlackjack(Wager* wager);
extern void wagerWonBlackjack(Wager* wager, int64_t pays, int64_t bet);
extern void wagerWon(Wager* wager);
extern void wagerLost(Wager* wager);
extern void wagerPush(Wager* wager);
extern void wagerWonInsurance(Wager* wager);
extern void wagerLostInsurance(Wager* wager);

#endif // WAGER_H
