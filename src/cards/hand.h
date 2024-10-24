#ifndef HAND_H
#define HAND_H

#include <stdbool.h>
#include "card.h"

// Define the Hand struct
typedef struct {
	Card *cards[24];	// Array of cards in the hand
	int card_count;		// Number of cards in the hand
	int hand_total;		// Total value of the hand
	int soft_ace;		// Number of aces valued as 11
	bool surrender;		// Whether the hand has surrendered
	int have_cards[13];
} Hand;

// Global functions
void handReset(Hand *hand);
Card *handDrawCard(Hand *hand, Card *card);
bool handIsBlackjack(Hand *hand);
bool handIsPair(Hand *hand);
bool handIsPairOfAces(Hand *hand);
bool handIsBusted(Hand *hand);
bool handIsSoft(Hand *hand);
int handTotal(Hand *hand);
bool handIsSoft17(Hand *hand);
Card *handSplitPair(Hand *hand);
int *getHaveCards(Hand *hand);

#endif // HAND_H
