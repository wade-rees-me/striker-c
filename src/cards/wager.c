#include <stdio.h>
#include <stdlib.h>
#include "wager.h"

void initWager(Wager *wager, int minimum_bet, int maximum_bet) {
	wager->minimum_bet = minimum_bet;
	wager->maximum_bet = maximum_bet;
}

// Reset the wager to its initial state
void wagerReset(Wager *wager) {
	handReset(&wager->hand);  // Reset the associated hand
	wager->amount_bet = 0;
	wager->amount_won = 0;
	wager->insurance_bet = 0;
	wager->insurance_won = 0;
}

// Split the wager
void wagerSplit(Wager *wager, Wager *split) {
	wagerReset(split);
	split->amount_bet = wager->amount_bet;
	handDrawCard(&split->hand, handSplitPair(&wager->hand));  // Draw the split card into the new wager's hand
}

// Place a bet (must be a multiple of 2)
void wagerPlaceBet(Wager *wager, int64_t bet) {
	wager->amount_bet = (min(wager->maximum_bet, max(wager->minimum_bet, bet)) + 1) / 2 * 2;
}

// Double the bet
void wagerDoubleBet(Wager *wager) {
	wager->amount_bet *= 2;
}

// Check if the wager's hand is a blackjack
bool wagerIsBlackjack(Wager *wager) {
	return handIsBlackjack(&wager->hand);
}

// Calculate the winnings for a blackjack
void wagerWonBlackjack(Wager *wager, int64_t pays, int64_t bet) {
	wager->amount_won = (wager->amount_bet * pays) / bet;
}

// Mark the wager as won
void wagerWon(Wager *wager) {
	wager->amount_won = wager->amount_bet;
}

// Mark the wager as lost
void wagerLost(Wager *wager) {
	wager->amount_won = -wager->amount_bet;
}

// Push the wager (no win, no loss)
void wagerPush(Wager *wager) {
	// No action needed for a push in this case
}

// Mark insurance as won
void wagerWonInsurance(Wager *wager) {
	wager->insurance_won = wager->insurance_bet * 2;
}

// Mark insurance as lost
void wagerLostInsurance(Wager *wager) {
	wager->insurance_won = -wager->insurance_bet;
}

