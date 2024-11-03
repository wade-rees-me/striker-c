#ifndef WAGER_H
#define WAGER_H

#include <stdbool.h>
#include <stdint.h>
#include "hand.h" // Include the Hand struct

//
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

// Define the Wager struct
typedef struct {
	Hand hand;
	int64_t minimum_bet;
	int64_t maximum_bet;
	int64_t amount_bet;
	int64_t amount_won;
	int64_t insurance_bet;
	int64_t insurance_won;
} Wager;

// Global functions
void initWager(Wager *wager, int minimum_bet, int maximum_bet);
void wagerReset(Wager *wager);
void wagerSplit(Wager *wager, Wager *split);
void wagerPlaceBet(Wager *wager, int64_t bet);
void wagerDoubleBet(Wager *wager);
bool wagerIsBlackjack(Wager *wager);
void wagerWonBlackjack(Wager *wager, int64_t pays, int64_t bet);
void wagerWon(Wager *wager);
void wagerLost(Wager *wager);
void wagerPush(Wager *wager);
void wagerWonInsurance(Wager *wager);
void wagerLostInsurance(Wager *wager);

#endif // WAGER_H
