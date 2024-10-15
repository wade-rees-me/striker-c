#include <stdlib.h>
#include <stdio.h>
#include "dealer.h"

//
bool dealerStand(Dealer* dealer);

// Create a new Dealer
Dealer* newDealer(bool hit_soft_17) {
	Dealer* dealer = (Dealer*)malloc(sizeof(Dealer));
	if (!dealer) {
		printf("Memory allocation failed for Dealer\n");
		exit(1);
	}
	dealer->hit_soft_17 = hit_soft_17;
	handReset(&dealer->hand);
	return dealer;
}

// Reset the dealer's hand
void dealerReset(Dealer* dealer) {
	handReset(&dealer->hand);
}

// Dealer draws a card
Card* dealerDrawCard(Dealer* dealer, Card* card) {
	return handDrawCard(&dealer->hand, card);
}

// Dealer plays by drawing cards from the shoe until standing
void dealerPlay(Dealer* dealer, Shoe* shoe) {
	while (!dealerStand(dealer)) {
		dealerDrawCard(dealer, shoeDrawCard(shoe));
	}
}

// Determine if the dealer should stand
bool dealerStand(Dealer* dealer) {
	if (dealer->hit_soft_17 && handIsSoft17(&dealer->hand)) {
		return false;
	}
	return handTotal(&dealer->hand) >= 17;
}

