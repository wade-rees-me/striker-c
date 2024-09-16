#ifndef HAND_H
#define HAND_H

#include <stdbool.h>
#include "shoe.h"

// Define the Hand struct
typedef struct {
	Card* cards[24];	// Array of cards in the hand
	int card_count;		// Number of cards in the hand
	int hand_total;		// Total value of the hand
	int soft_ace;		// Number of aces valued as 11
	bool surrender;		// Whether the hand has surrendered
} Hand;

// Function declarations
void hand_reset(Hand* hand);
Card* hand_draw_card(Hand* hand, Card* card);
bool hand_is_blackjack(Hand* hand);
bool hand_is_pair(Hand* hand);
bool hand_is_pair_of_aces(Hand* hand);
bool hand_is_busted(Hand* hand);
bool hand_is_soft(Hand* hand);
int hand_total(Hand* hand);
bool hand_is_soft_17(Hand* hand);
Card* hand_split_pair(Hand* hand);
void hand_calculate_total(Hand* hand);

#endif // HAND_H

