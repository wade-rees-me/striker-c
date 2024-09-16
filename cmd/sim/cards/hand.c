#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hand.h"

// Reset the hand
void hand_reset(Hand* hand) {
	hand->card_count = 0;
	hand->hand_total = 0;
	hand->soft_ace = 0;
	hand->surrender = false;
}

// Draw a card into the hand
Card* hand_draw_card(Hand* hand, Card* card) {
	hand->cards[hand->card_count] = card;
	hand->card_count++;
	hand_calculate_total(hand);  // Recalculate hand total
	return card;
}

// Check if the hand is a blackjack
bool hand_is_blackjack(Hand* hand) {
	return hand->card_count == 2 && hand->hand_total == 21;
}

// Check if the hand is a pair
bool hand_is_pair(Hand* hand) {
	return hand->card_count == 2 && strcmp(hand->cards[0]->rank, hand->cards[1]->rank) == 0;
}

// Check if the hand is a pair of aces
bool hand_is_pair_of_aces(Hand* hand) {
	return hand_is_pair(hand) && strcmp(hand->cards[0]->rank, "ace") == 0;
}

// Check if the hand is busted (over 21)
bool hand_is_busted(Hand* hand) {
	return hand->hand_total > 21;
}

// Check if the hand has a soft ace
bool hand_is_soft(Hand* hand) {
	return hand->soft_ace > 0;
}

// Get the total value of the hand
int hand_total(Hand* hand) {
	return hand->hand_total;
}

// Check if the hand is a soft 17
bool hand_is_soft_17(Hand* hand) {
	return hand->hand_total == 17 && hand_is_soft(hand);
}

// Split a pair from the hand
Card* hand_split_pair(Hand* hand) {
	if (hand_is_pair(hand)) {
		Card* split_card = hand->cards[1];
		hand->card_count = 1;
		hand_total(hand);  // Recalculate hand total after splitting
		return split_card;
	} else {
		printf("Error: Trying to split a non-pair\n");
		return NULL;
	}
}

// Recalculate the total value of the hand
void hand_calculate_total(Hand* hand) {
	hand->hand_total = 0;
	hand->soft_ace = 0;
	for (int i = 0; i < hand->card_count; i++) {
		hand->hand_total += hand->cards[i]->value;
		if (hand->cards[i]->value == 11) {
			hand->soft_ace++;
		}
	}

	// Adjust hand total if it's over 21 and there are soft aces
	while (hand->hand_total > 21 && hand->soft_ace > 0) {
		hand->hand_total -= 10;
		hand->soft_ace--;
	}
}

