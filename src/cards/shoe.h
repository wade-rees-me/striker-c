#ifndef SHOE_H
#define SHOE_H

#include <stdbool.h>
#include "card.h"

//
#define SPADES "spades"
#define DIAMONDS "diamonds"
#define CLUBS "clubs"
#define HEARTS "hearts"

//
#define TWO "two"
#define THREE "three"
#define FOUR "four"
#define FIVE "five"
#define SIX "six"
#define SEVEN "seven"
#define EIGHT "eight"
#define NINE "nine"
#define TEN "ten"
#define JACK "jack"
#define QUEEN "queen"
#define KING "king"
#define ACE "ace"

// Shoe structure to hold cards and state of the shoe
typedef struct {
	Card **cards;			// Array of card pointers
	int number_of_cards;	// Total number of cards
	int cut_card;			// Cut card position
	int next_card;			// Index of the next card to be drawn
	int last_discard;		// Last card discarded
	bool force_shuffle;		// Flag to force shuffle
} Shoe;

// Global functions
Shoe *newShoe(int number_of_decks, float penetration);
void shoeCleanup(Shoe *shoe);
Card *shoeDrawCard(Shoe *shoe);
void shoeShuffle(Shoe *shoe);
bool shoeShouldShuffle(Shoe *shoe);
bool cardIsAce(const Card *card);
void shoeDisplay(Shoe *shoe);

#endif // SHOE_H
