#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"

//
Card* newCard(const char* suit, const char* rank, int value, int offset) {
	Card* card = (Card*)malloc(sizeof(Card));
	if (!card) {
		printf("Memory allocation failed for card\n");
		exit(1);
	}

	card->suit = suit;
	card->rank = rank;
	card->value = value;
	card->offset = offset;

	return card;
}

