#include <stdio.h>
#include <stdlib.h>
#include "card.h"

//
Card *newCard(const char *suit, const char *rank, const char *key, int value, int offset) {
	Card *card = (Card*)malloc(sizeof(Card));
	if (!card) {
		printf("Memory allocation failed for card\n");
		exit(1);
	}

	card->suit = suit;
	card->rank = rank;
	card->key = key;
	card->value = value;
	card->offset = offset;

	return card;
}

const char *cardGetRank(Card *card) {
	return card->rank;
}

const char *cardGetSuit(Card *card) {
	return card->suit;
}

const char *cardGetKey(Card *card) {
	return card->key;
}

int cardGetValue(Card *card) {
	return card->value;
}

int cardGetOffset(Card *card) {
	return card->offset;
}

