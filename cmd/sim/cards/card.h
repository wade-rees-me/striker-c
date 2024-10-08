#ifndef CARD_H
#define CARD_H

typedef struct {
	const char* rank;	// Rank of the card (e.g., "ace")
	const char* suit;	// Suit of the card (e.g., "hearts")
	int value;			// Value of the card for game calculations - 2 thru 11
	int offset;			// Index of the card in a suit - 0 thru 12
} Card;

Card* newCard(const char* suit, const char* rank, int value, int offset);

#endif // CARD_H
