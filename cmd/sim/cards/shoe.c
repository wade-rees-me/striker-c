#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "shoe.h"
#include "card.h"

//
const char* suits[4] = {SPADES, DIAMONDS, CLUBS, HEARTS};

// Helper function to shuffle the deck (Fisher-Yates algorithm)
static void shuffleRandom(Shoe* shoe) {
    int i;
    srand(time(NULL));  // Seed the random number generator
    for (i = shoe->last_discard - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card* temp = shoe->cards[i];
        shoe->cards[i] = shoe->cards[j];
        shoe->cards[j] = temp;
    }
    shoe->next_card = 1;  // Start drawing from the second card (burn the first)
}

// Initialize the shoe with multiple decks
Shoe* newShoe(int number_of_decks, float penetration) {
	Shoe* shoe = (Shoe*)malloc(sizeof(Shoe));
	if (!shoe) {
		printf("Memory allocation failed for shoe\n");
		exit(1);
	}

    shoe->number_of_cards = number_of_decks * 52; // 52 cards per deck
    shoe->cards = (Card**)malloc(shoe->number_of_cards * sizeof(Card*));

    int card_index = 0;
    for (int i = 0; i < number_of_decks; i++) {
        for (int suit = 0; suit < 4; suit++) {
            shoe->cards[card_index++] = newCard(suits[suit], TWO, 2, 0);
            shoe->cards[card_index++] = newCard(suits[suit], THREE, 3, 1);
            shoe->cards[card_index++] = newCard(suits[suit], FOUR, 4, 2);
            shoe->cards[card_index++] = newCard(suits[suit], FIVE, 5, 3);
            shoe->cards[card_index++] = newCard(suits[suit], SIX, 6, 4);
            shoe->cards[card_index++] = newCard(suits[suit], SEVEN, 7, 5);
            shoe->cards[card_index++] = newCard(suits[suit], EIGHT, 8, 6);
            shoe->cards[card_index++] = newCard(suits[suit], NINE, 9, 7);
            shoe->cards[card_index++] = newCard(suits[suit], TEN, 10, 8);
            shoe->cards[card_index++] = newCard(suits[suit], JACK, 10, 9);
            shoe->cards[card_index++] = newCard(suits[suit], QUEEN, 10, 10);
            shoe->cards[card_index++] = newCard(suits[suit], KING, 10, 11);
            shoe->cards[card_index++] = newCard(suits[suit], ACE, 11, 12);
        }
    }

    shoe->next_card = 0;
    shoe->last_discard = shoe->number_of_cards;
    shoe->cut_card = (int)(shoe->number_of_cards * penetration);
    shoe->force_shuffle = false;

    //shoeShuffle(shoe);  // Shuffle the cards
	return shoe;
}

// Cleanup the shoe and free allocated memory
void shoeCleanup(Shoe* shoe) {
    //for (int i = 0; i < shoe->number_of_cards; i++) {
        //cardDestroy(shoe->cards[i]);  // Card_destroy should properly free card resources
    //}
    free(shoe->cards);  // Free the memory allocated for the cards array
}

// Shuffle the shoe of cards
void shoeShuffle(Shoe* shoe) {
    shoe->last_discard = shoe->number_of_cards;
    shoe->force_shuffle = false;
    shuffleRandom(shoe);
}

// Draw a card from the shoe
Card* shoeDrawCard(Shoe* shoe) {
    if (shoe->next_card >= shoe->number_of_cards) {
        shoe->force_shuffle = true;
        shoeShuffle(shoe);
    }
    Card* card = shoe->cards[shoe->next_card];
    shoe->next_card++;
    return card;
}

// Check if the shoe should be shuffled
bool shoeShouldShuffle(Shoe* shoe) {
    shoe->last_discard = shoe->next_card;
    return (shoe->next_card >= shoe->cut_card) || shoe->force_shuffle;
}

// Check if a card is an ace
bool  cardIsAce(const Card* card) {
	return card->value == 11;
}

// Display the shoe of cards (for debugging purposes)
void shoeDisplay(Shoe* shoe) {
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < shoe->number_of_cards; i++) {
        printf("%03d: ", i);
        //cardDisplay(shoe->cards[i]);  // Card_display should output the card information
    }
    printf("--------------------------------------------------------------------------------\n");
}

