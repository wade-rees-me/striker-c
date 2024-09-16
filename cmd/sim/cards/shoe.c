#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "shoe.h"

// Define the suits array
const char* suits[4] = {"spades", "diamonds", "clubs", "hearts"};

// Define the card ranks and values
Rank ranks[13] = {
	{"two", 2, 0},
	{"three", 3, 1},
	{"four", 4, 2},
	{"five", 5, 3},
	{"six", 6, 4},
	{"seven", 7, 5},
	{"eight", 8, 6},
	{"nine", 9, 7},
	{"ten", 10, 8},
	{"jack", 10, 9},
	{"queen", 10, 10},
	{"king", 10, 11},
	{"ace", 11, 12}
};

// Create a new shoe of cards
Shoe* new_shoe(int number_of_decks, float penetration) {
	Shoe* shoe = (Shoe*)malloc(sizeof(Shoe));
	if (!shoe) {
		printf("Memory allocation failed for shoe\n");
		exit(1);
	}

	shoe->number_of_decks = number_of_decks;
	shoe->number_of_cards = 0;
	shoe->cards_not_in_play = 0;
	shoe->cards_in_play = 0;
	shoe->cards_in_discard = 0;

	for (int i = 0; i < number_of_decks; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 13; k++) {
				Card* card = (Card*)malloc(sizeof(Card));
				if (!card) {
					printf("Memory allocation failed for card\n");
					exit(1);
				}

				card->suit = suits[j];
				card->rank = ranks[k].rank;
				card->value = ranks[k].value;
				card->offset = ranks[k].offset;
				card->index = shoe->number_of_cards;
				shoe->discards[shoe->number_of_cards] = card;
				shoe->number_of_cards++;
			}
		}
	}
	shoe->cards_in_discard = shoe->number_of_cards;

	shoe->cut_card = (int)(shoe->number_of_cards * penetration);
	shoe->number_of_shuffles = 0;
	shoe->number_out_of_cards = 0;
	shoe->force_shuffle = false;
	
	return shoe;
}

void shoe_shuffle(Shoe* shoe) {
	for (int i = 0; i < shoe->cards_not_in_play; i++) {
		shoe->discards[shoe->cards_in_discard] = shoe->cards[i];
		shoe->cards_in_discard++;
	}
	shoe->cards_not_in_play = 0;
	for (int i = 0; i < shoe->cards_in_play; i++) {
		shoe->discards[shoe->cards_in_discard] = shoe->inplay[i];
		shoe->cards_in_discard++;
	}
	shoe->cards_in_play = 0;

	shoe->force_shuffle = false;
	shoe_shuffle_fisher_yates(shoe->discards, shoe->cards_in_discard);

	for (int i = shoe->cards_in_discard - 1; i >= 0; i--) {
		shoe->cards[shoe->cards_not_in_play] = shoe->discards[i];
		shoe->cards_not_in_play++;
	}
	shoe->cards_in_discard = 0;
}

// Fisher-Yates shuffle algorithm for shuffling cards
void shoe_shuffle_fisher_yates(Card* cards[MAX_NUMBER_OF_DECKS * NUMBER_OF_CARDS], int num_cards) {
	for (int i = num_cards - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		Card* temp = cards[i];
		cards[i] = cards[j];
		cards[j] = temp;
	}
}

// Shuffle discards and reset the shoe
void shoe_shuffle_discard(Shoe* shoe) {
	shoe->force_shuffle = true;
	shoe->number_out_of_cards++;

	// Shuffle using Fisher-Yates algorithm
	shoe_shuffle_fisher_yates(shoe->discards, shoe->cards_in_discard);

	// Reset the cards and inplay arrays
	for (int i = shoe->cards_in_discard - 1; i >= 0; i--) {
		shoe->discards[i] = shoe->cards[shoe->cards_not_in_play];
		shoe->cards_not_in_play++;
	}
	shoe->cards_in_discard = 0;

	// Burn a card
	shoe_discard_card(shoe, shoe_draw_card(shoe));

	shoe->number_of_shuffles++;
}

// Draw a card from the shoe
Card* shoe_draw_card(Shoe* shoe) {
	if (shoe->cards_not_in_play == 0) {
		shoe_shuffle_discard(shoe); // If no cards in the discard pile, shuffle the discards back
	}

	// Draw the top card
	Card* card = shoe->cards[0];

	shoe->inplay[shoe->cards_in_play] = card;
	shoe->cards_in_play++;

	// Shift the remaining cards down
	for (int i = 0; i < shoe->cards_not_in_play - 1; i++) {
		shoe->cards[i] = shoe->cards[i + 1];
	}
	shoe->cards_not_in_play--;

	return card;
}

// Discard a card into the discard pile
void shoe_discard_card(Shoe* shoe, Card* card) {
	shoe->discards[shoe->cards_in_discard] = card;
	shoe->cards_in_discard++;
}

// Check if the shoe should be shuffled
bool shoe_should_shuffle(Shoe* shoe) {
	if (shoe->cards_not_in_play < (shoe->number_of_cards - shoe->cut_card) || shoe->force_shuffle) {
		return true;
	}
	return false;
}

bool card_is_ace(Card* card) {
	return card->value == 11;
}

