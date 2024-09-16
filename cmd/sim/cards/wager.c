#include <stdio.h>
#include <stdlib.h>
#include "wager.h"

// Reset the wager to its initial state
void wager_reset(Wager* wager) {
    hand_reset(&wager->hand);  // Reset the associated hand
    wager->amount_bet = 0;
    wager->amount_won = 0;
    wager->double_bet = 0;
    wager->double_won = 0;
    wager->insurance_bet = 0;
    wager->insurance_won = 0;
}

// Split the wager
void wager_split(Wager* wager, Wager* split) {
    wager_reset(split);
    split->amount_bet = wager->amount_bet;
    hand_draw_card(&split->hand, hand_split_pair(&wager->hand));  // Draw the split card into the new wager's hand
}

// Place a bet (must be a multiple of 2)
void wager_place_bet(Wager* wager, int64_t bet) {
    if (bet % 2 != 0) {
        printf("Error: All bets must be in multiples of 2.\n");
        return;
    }
    wager->amount_bet = bet;
}

// Double the bet
void wager_double_bet(Wager* wager) {
    wager->double_bet = wager->amount_bet;
}

// Check if the wager's hand is a blackjack
bool wager_is_blackjack(Wager* wager) {
    return hand_is_blackjack(&wager->hand);
}

// Calculate the winnings for a blackjack
void wager_won_blackjack(Wager* wager, int64_t pays, int64_t bet) {
    wager->amount_won = (wager->amount_bet * pays) / bet;
}

// Mark the wager as won
void wager_won(Wager* wager) {
    wager->amount_won = wager->amount_bet;
    wager->double_won = wager->double_bet;
}

// Mark the wager as lost
void wager_lost(Wager* wager) {
    wager->amount_won = -wager->amount_bet;
    wager->double_won = -wager->double_bet;
}

// Push the wager (no win, no loss)
void wager_push(Wager* wager) {
    // No action needed for a push in this case
}

// Mark insurance as won
void wager_won_insurance(Wager* wager) {
    wager->insurance_won = wager->insurance_bet * 2;
}

// Mark insurance as lost
void wager_lost_insurance(Wager* wager) {
    wager->insurance_won = -wager->insurance_bet;
}

