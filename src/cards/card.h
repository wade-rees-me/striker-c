#ifndef CARD_H
#define CARD_H

//
typedef struct {
	const char *rank;	// Rank of the card (e.g., "ace")
	const char *suit;	// Suit of the card (e.g., "hearts")
	const char *key;
	int value;			// Value of the card for game calculations - 2 thru 11
	int offset;			// Index of the card in a suit - 0 thru 12
} Card;

// Global functions
Card *newCard(const char *suit, const char *rank, const char *key, int value, int offset);
const char *cardGetRank(Card *card);
const char *cardGetSuit(Card *card);
const char *cardGetKey(Card *card);
int cardGetValue(Card *card);
int cardGetOffset(Card *card);

#endif // CARD_H
