#ifndef SHOE_H
#define SHOE_H

#include <stdbool.h>
#include "card.h"

// Shoe structure to hold cards and state of the shoe
typedef struct {
    Card** cards;          // Array of card pointers
    int number_of_cards;   // Total number of cards
    int cut_card;          // Cut card position
    int next_card;         // Index of the next card to be drawn
    int last_discard;      // Last card discarded
    bool force_shuffle;    // Flag to force shuffle
} Shoe;

// Function prototypes
Shoe* newShoe(int number_of_decks, float penetration);
void shoeCleanup(Shoe* shoe);
Card* shoeDrawCard(Shoe* shoe);
void shoeShuffle(Shoe* shoe);
bool shoeShouldShuffle(Shoe* shoe);
bool  cardIsAce(const Card* card);
void shoeDisplay(Shoe* shoe);

#endif // SHOE_H
