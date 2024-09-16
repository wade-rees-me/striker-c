#ifndef SHOE_H
#define SHOE_H

#include <stdbool.h>

#define MAX_NUMBER_OF_DECKS 8
#define NUMBER_OF_CARDS 52

typedef struct {
	const char* rank;	// Rank of the card (e.g., "ace")
	const char* suit;	// Suit of the card (e.g., "hearts")
	int value;			// Value of the card for game calculations - 2 thru 11
	int offset;			// Index of the card in a suit - 0 thru 12
	int index;			// Index of the card in a shoe - 0 to number_of_cards
} Card;

typedef struct {
	const char* rank;
	int value;
	int offset;
} Rank;

// Define Shoe struct in C
typedef struct {
	int number_of_decks;									// Number of decks in the shoe
	Card* cards[MAX_NUMBER_OF_DECKS * NUMBER_OF_CARDS];		// Cards currently in the shoe
	Card* inplay[MAX_NUMBER_OF_DECKS * NUMBER_OF_CARDS];	// Cards that are in play
	Card* discards[MAX_NUMBER_OF_DECKS * NUMBER_OF_CARDS];	// Cards that have been discarded
	int number_of_cards;									// Total number of cards
	int cards_not_in_play;									// Track number of cards in play
	int cards_in_play;										// Track number of cards in play
	int cards_in_discard;									// Track number of cards in discard
	int cut_card;											// The cut card position in the shoe
	bool force_shuffle;										// Flag to force a shuffle
	Card* downcard;											// Dealer hole card
	int64_t number_of_shuffles;								// Number of times the deck has been shuffled
	int64_t number_out_of_cards;							// Number of times out of cards
} Shoe;

// Function declarations
extern Shoe* new_shoe(int number_of_decks, float penetration);
extern void shoe_shuffle(Shoe* shoe);
extern void shoe_shuffle_fisher_yates(Card* cards[MAX_NUMBER_OF_DECKS * NUMBER_OF_CARDS], int num_cards);
extern void shoe_shuffle_discard(Shoe* shoe);
extern Card* shoe_draw_card(Shoe* shoe);
extern void shoe_discard_card(Shoe* shoe, Card* card);
extern bool shoe_should_shuffle(Shoe* shoe);
extern bool card_is_ace(Card* card);

#endif // SHOE_H

