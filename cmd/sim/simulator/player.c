#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "shoe.h"
#include "rules.h"
#include "player.h"
#include "player_strategy.h"

// Utility function to create a new Player object
Player* new_player(Parameters* parameters, int number_of_cards) {
    Player* p = (Player*)malloc(sizeof(Player));
    p->parameters = parameters;
    p->number_of_cards = number_of_cards;

    if (sscanf(parameters->blackjack_pays, "%lld:%lld", &p->blackjack_pays, &p->blackjack_bets) != 2) {
        printf("Failed to parse blackjack pays\n");
        exit(1);
    }

	report_init(&p->report);

    return p;
}

// Shuffle function (reinitializes seen cards)
void player_shuffle(Player* p) {
    memset(p->seen_cards, 0, sizeof(p->seen_cards));
}

// Place a bet for the player
void player_place_bet(Player* p, int bet) {
    // Reset wagers
    wager_reset(&p->wager);
    for (int i = 0; i < MAX_SPLIT_HANDS; i++) {
        p->splits[i].amount_bet = 0;
    }
    p->split_count = 0;
	p->wager.amount_bet = bet;
}

// Simulate an insurance bet
void player_insurance(Player* p) {
    bool doInsurance = player_get_insurance(p);
    if (doInsurance) {
        p->wager.insurance_bet = p->wager.amount_bet / 2;
    }
}

void player_play(Player* p, Shoe* s, Card* up) {
    if (hand_is_blackjack(&p->wager.hand)) {
        return;
    }

    // Get cards in hand
    int have_cards[13] = {0}; // Assuming 13 ranks
    get_have(&p->wager.hand, have_cards);

    // Check for surrender
    bool doSurrender = player_get_surrender(p, have_cards, up->offset);
    if (doSurrender) {
        p->wager.hand.surrender = true;
        return;
    }

    // Check for double
    bool doDouble = player_get_double(p, have_cards, up->offset);
    if (doDouble && (TableRules.double_any_two_cards || (hand_total(&p->wager.hand) == 10 || hand_total(&p->wager.hand) == 11))) {
		wager_double_bet(&p->wager);
        player_draw_card(p, &p->wager.hand, s);
        return;
    }

    // Check for splitting
    if (hand_is_pair(&p->wager.hand) && player_get_split(p, p->wager.hand.cards[0]->value, up->offset)) {
        Wager* split = &p->splits[p->split_count];
        p->split_count++;
        
        if (hand_is_pair_of_aces(&p->wager.hand)) {
            if (!TableRules.resplit_aces && !TableRules.hit_split_aces) {
                wager_split(&p->wager, split);
                player_draw_card(p, &p->wager.hand, s);
                player_draw_card(p, &split->hand, s);
                return;
            }
        }

        wager_split(&p->wager, split);
        player_draw_card(p, &p->wager.hand, s);
        player_play_split(p, &p->wager, s, up);
        player_draw_card(p, &split->hand, s);
        player_play_split(p, split, s, up);
        return;
    }

    // Handle the stand logic
    bool doStand = player_get_stand(p, have_cards, up->offset);
    while (!hand_is_busted(&p->wager.hand) && !doStand) {
        player_draw_card(p, &p->wager.hand, s);
    	get_have(&p->wager.hand, have_cards);
        doStand = player_get_stand(p, have_cards, up->offset);
    }
}

// Simulate a split action
void player_play_split(Player* p, Wager* w, Shoe* s, Card* up) {
    int have_cards[13] = {0};  // Placeholder for the actual hand composition
    get_have(&w->hand, have_cards);

    // Check if player can double after split
    if (TableRules.double_after_split && player_get_double(p, have_cards, up->offset)) {
		wager_double_bet(&p->wager);
        player_draw_card(p, &w->hand, s);
        return;
    }

    // Check if the hand is a pair and the player can split
    if (hand_is_pair(&w->hand) && p->split_count < 4) {  // Assuming 4 is the max number of split hands
        if (player_get_split(p, w->hand.cards[0]->value, up->offset)) {
            Wager* split = &p->splits[p->split_count];
            p->split_count++;

            if (hand_is_pair_of_aces(&w->hand)) {
                if (!TableRules.resplit_aces && !TableRules.hit_split_aces) {
                    wager_split(w, split);
                    player_draw_card(p, &w->hand, s);
                    player_draw_card(p, &split->hand, s);
                    return;
                }
            }

            wager_split(w, split);
            player_draw_card(p, &w->hand, s);
            player_play_split(p, w, s, up);
            player_draw_card(p, &split->hand, s);
            player_play_split(p, split, s, up);
            return;
        }
    }

    // Check if the hand contains a Blackjack Ace and if the rule allows hitting split Aces
    if (w->hand.cards[0]->value == 11 && !TableRules.hit_split_aces) {
        return;
    }

    // Implement stand logic
    bool doStand = player_get_stand(p, have_cards, up->offset);
    while (!hand_is_busted(&w->hand) && !doStand) {
        player_draw_card(p, &w->hand, s);
    	get_have(&w->hand, have_cards);
        doStand = player_get_stand(p, have_cards, up->offset);
    }
}

//
Card* player_draw_card(Player* p, Hand* h, Shoe* s) {
	Card* card = shoe_draw_card(s);
    player_show_card(p, card);
    hand_draw_card(h, card);
    return card;
}

// Equivalent to Go's Show function
void player_show_card(Player* p, Card* c) {
    p->seen_cards[c->offset]++;
}

// Function to check if the player busted or has blackjack
int player_busted_or_blackjack(Player* p) {
    if (p->split_count == 0) {
        return hand_is_busted(&p->wager.hand) || hand_is_blackjack(&p->wager.hand);
    }
    for (int i = 0; i < p->split_count; i++) {
        if (!hand_is_busted(&p->splits[i].hand)) {
            return 0;
        }
    }
    return 1;
}

void player_payoff(Player *p, bool dealer_blackjack, bool dealer_busted, int dealer_total) {
    if (p->split_count == 0) {
        player_payoff_hand(p, &(p->wager), dealer_blackjack, dealer_busted, dealer_total);
        return;
    }

    payoff_split(p, &(p->wager), dealer_busted, dealer_total);
    for (int i = 0; i < p->split_count; i++) {
        payoff_split(p, &(p->splits[i]), dealer_busted, dealer_total);
    }
}

void player_payoff_hand(Player *p, Wager *w, int dealer_blackjack, int dealer_busted, int dealer_total) {
    if (dealer_blackjack) {
        wager_won_insurance(w);
    } else {
        wager_lost_insurance(w);
    }

    if (w->hand.surrender) {
        p->report.total_won -= w->amount_bet / 2;
    } else {
        if (dealer_blackjack) {
            if (wager_is_blackjack(w)) {
                wager_push(w);
            } else {
                wager_lost(w);
            }
        } else if (wager_is_blackjack(w)) {
            wager_won_blackjack(w, p->blackjack_pays, p->blackjack_bets);
        } else if (hand_is_busted(&w->hand)) {
            wager_lost(w);
        } else if (dealer_busted || (w->hand.hand_total > dealer_total)) {
            wager_won(w);
        } else if (dealer_total > w->hand.hand_total) {
            wager_lost(w);
        } else {
            wager_push(w);
        }
        p->report.total_won += (w->amount_won + w->double_won);
    }
    p->report.total_bet += w->amount_bet + w->double_bet + w->insurance_bet;
}

void payoff_split(Player *p, Wager *w, int dealer_busted, int dealer_total) {
    if (hand_is_busted(&w->hand)) {
        wager_lost(w);
    } else if (dealer_busted || (w->hand.hand_total > dealer_total)) {
        wager_won(w);
    } else if (dealer_total > w->hand.hand_total) {
        wager_lost(w);
    } else {
        wager_push(w);
    }

    p->report.total_won += (w->amount_won + w->double_won);
    p->report.total_bet += w->amount_bet + w->double_bet;
}

// Function to mimic the dealer's play
void player_mimic_dealer(Player* p, Shoe* s) {
    while (!(hand_total(&p->wager.hand) >= 17)) {
        player_draw_card(p, &p->wager.hand, s);
    }
}

void get_have(Hand* h, int have_cards[13]) {
	for (int i = 0; i < 13; i++) {
    	have_cards[i] = 0;
	}
	for (int i = 0; i < h->card_count; i++) {
    	Card* card = h->cards[i];
        have_cards[card->offset]++;
    }
}
