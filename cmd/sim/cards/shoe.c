#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "shoe.h"
#include "rank.h"
#include "card.h"

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
            for (int rank = 0; rank < 13; rank++) {
				Card* card = (Card*)malloc(sizeof(Card));
				if (!card) {
					printf("Memory allocation failed for card\n");
					exit(1);
				}

				card->suit = suits[suit];
				card->rank = ranks[rank].rank;
				card->value = ranks[rank].value;
				card->offset = ranks[rank].offset;
                shoe->cards[card_index] = card;
                card_index++;
            }
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

