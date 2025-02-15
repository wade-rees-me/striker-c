#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "shoe.h"
#include "card.h"

//
const char *suits[4] = {SPADES, DIAMONDS, CLUBS, HEARTS};

// Helper function to shuffle the deck (Fisher-Yates algorithm)
static void shuffleRandom(Shoe *shoe) {
	for (int i = shoe->last_discard - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		Card* temp = shoe->cards[i];
		shoe->cards[i] = shoe->cards[j];
		shoe->cards[j] = temp;
	}
	shoe->next_card = 1;  // Start drawing from the second card (burn the first)
}

// Initialize the shoe with multiple decks
Shoe *newShoe(int number_of_decks, float penetration) {
	Shoe *shoe = (Shoe*)malloc(sizeof(Shoe));
	if (!shoe) {
		printf("Memory allocation failed for shoe\n");
		exit(1);
	}

	shoe->number_of_cards = number_of_decks * 52; // 52 cards per deck
	shoe->cards = (Card**)malloc(shoe->number_of_cards * sizeof(Card*));

	int card_index = 0;
	for (int i = 0; i < number_of_decks; i++) {
		for (int suit = 0; suit < 4; suit++) {
			shoe->cards[card_index++] = newCard(suits[suit], TWO, "2", 2);
			shoe->cards[card_index++] = newCard(suits[suit], THREE, "3", 3);
			shoe->cards[card_index++] = newCard(suits[suit], FOUR, "4", 4);
			shoe->cards[card_index++] = newCard(suits[suit], FIVE, "5", 5);
			shoe->cards[card_index++] = newCard(suits[suit], SIX, "6", 6);
			shoe->cards[card_index++] = newCard(suits[suit], SEVEN, "7", 7);
			shoe->cards[card_index++] = newCard(suits[suit], EIGHT, "8", 8);
			shoe->cards[card_index++] = newCard(suits[suit], NINE, "9", 9);
			shoe->cards[card_index++] = newCard(suits[suit], TEN, "X", 10);
			shoe->cards[card_index++] = newCard(suits[suit], JACK, "X", 10);
			shoe->cards[card_index++] = newCard(suits[suit], QUEEN, "X", 10);
			shoe->cards[card_index++] = newCard(suits[suit], KING, "X", 10);
			shoe->cards[card_index++] = newCard(suits[suit], ACE, "A", 11);
		}
	}

	shoe->next_card = 0;
	shoe->last_discard = shoe->number_of_cards;
	shoe->cut_card = (int)(shoe->number_of_cards * penetration);
	shoe->force_shuffle = false;

	srand(time(NULL));
	shoeShuffle(shoe);
	return shoe;
}

// Cleanup the shoe and free allocated memory
void shoeCleanup(Shoe *shoe) {
	free(shoe->cards);  // Free the memory allocated for the cards array
}

// Shuffle the shoe of cards
void shoeShuffle(Shoe *shoe) {
	shoe->last_discard = shoe->number_of_cards;
	shoe->force_shuffle = false;
	shuffleRandom(shoe);
}

// Draw a card from the shoe
Card *shoeDrawCard(Shoe *shoe) {
	if (shoe->next_card >= shoe->number_of_cards) {
		shoe->force_shuffle = true;
		shoeShuffle(shoe);
	}
	Card *card = shoe->cards[shoe->next_card];
	shoe->next_card++;
	return card;
}

// Check if the shoe should be shuffled
bool shoeShouldShuffle(Shoe *shoe) {
	shoe->last_discard = shoe->next_card;
	return (shoe->next_card >= shoe->cut_card) || shoe->force_shuffle;
}

// Check if a card is an ace
bool cardIsAce(const Card *card) {
	return card->value == 11;
}

