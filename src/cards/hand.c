#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hand.h"

//
void handCalculateTotal(Hand *hand);

// Reset the hand
void handReset(Hand *hand) {
	hand->card_count = 0;
	hand->hand_total = 0;
	hand->soft_ace = 0;
}

// Draw a card into the hand
Card *handDrawCard(Hand *hand, Card *card) {
	hand->cards[hand->card_count] = card;
	hand->card_count++;
	handCalculateTotal(hand);  // Recalculate hand total
	return card;
}

// Check if the hand is a blackjack
bool handIsBlackjack(Hand *hand) {
	return hand->card_count == 2 && hand->hand_total == 21;
}

// Check if the hand is a pair
bool handIsPair(Hand *hand) {
	return hand->card_count == 2 && strcmp(hand->cards[0]->rank, hand->cards[1]->rank) == 0;
}

// Check if the hand is a pair of aces
bool handIsPairOfAces(Hand *hand) {
	return handIsPair(hand) && strcmp(hand->cards[0]->rank, "ace") == 0;
}

// Check if the hand is busted (over 21)
bool handIsBusted(Hand *hand) {
	return hand->hand_total > 21;
}

// Check if the hand has a soft ace
bool handIsSoft(Hand *hand) {
	return hand->soft_ace > 0;
}

// Get the total value of the hand
int handTotal(Hand *hand) {
	return hand->hand_total;
}

// Check if the hand is a soft 17
bool handIsSoft17(Hand *hand) {
	return hand->hand_total == 17 && handIsSoft(hand);
}

// Split a pair from the hand
Card *handSplitPair(Hand *hand) {
	if (handIsPair(hand)) {
		Card* split_card = hand->cards[1];
		hand->card_count = 1;
		handTotal(hand);  // Recalculate hand total after splitting
		return split_card;
	} else {
		printf("Error: Trying to split a non-pair\n");
		return NULL;
	}
}

//
int *getHaveCards(Hand *hand) {
	return hand->have_cards;
}

// Recalculate the total value of the hand
void handCalculateTotal(Hand *hand) {
	hand->hand_total = 0;
	hand->soft_ace = 0;
	memset(hand->have_cards, 0, sizeof(hand->have_cards));
	for (int i = 0; i < hand->card_count; i++) {
		hand->hand_total += hand->cards[i]->value;
		if (hand->cards[i]->value == 11) {
			hand->soft_ace++;
		}
		hand->have_cards[hand->cards[i]->offset]++;
	}

	// Adjust hand total if it's over 21 and there are soft aces
	while (hand->hand_total > 21 && hand->soft_ace > 0) {
		hand->hand_total -= 10;
		hand->soft_ace--;
	}
}

