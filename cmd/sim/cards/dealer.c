#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "rules.h"
#include "dealer.h"

//
bool dealer_stand(Dealer* dealer);

// Create a new Dealer
Dealer* new_dealer() {
    Dealer* dealer = (Dealer*)malloc(sizeof(Dealer));
    if (!dealer) {
        printf("Memory allocation failed for Dealer\n");
        exit(1);
    }
    hand_reset(&dealer->hand);
    return dealer;
}

// Reset the dealer's hand
void dealer_reset(Dealer* dealer) {
    hand_reset(&dealer->hand);
}

// Dealer draws a card
Card* dealer_draw_card(Dealer* dealer, Card* card) {
    return hand_draw_card(&dealer->hand, card);
}

// Dealer plays by drawing cards from the shoe until standing
void dealer_play(Dealer* dealer, Shoe* shoe) {
    while (!dealer_stand(dealer)) {
        dealer_draw_card(dealer, shoe_draw_card(shoe));
    }
}

// Determine if the dealer should stand
bool dealer_stand(Dealer* dealer) {
    if (TableRules.hit_soft_17 && hand_is_soft_17(&dealer->hand)) {
        return false;
    }
    return hand_total(&dealer->hand) >= 17;
}

