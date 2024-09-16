#ifndef PLAYER_H
#define PLAYER_H

#include "wager.h"
#include "shoe.h"
#include "report.h"
#include "constants.h"

typedef struct {
    Wager wager;
    Wager splits[MAX_SPLIT_HANDS];
    int split_count;
    int64_t blackjack_pays;
    int64_t blackjack_bets;
    int number_of_cards;
    int seen_cards[13];
    Parameters* parameters;
    Report report;
} Player;

extern Player* new_player(Parameters* parameters, int number_of_cards);
extern void player_shuffle(Player* p);
extern void player_place_bet(Player* p, int bet);
extern void player_insurance(Player* p);
extern void player_play(Player* p, Shoe* s, Card* up);
extern void player_play_split(Player* p, Wager* w, Shoe* s, Card* up);
extern Card* player_draw_card(Player* p, Hand* h, Shoe* s);
extern void player_show_card(Player* p, Card* c);
extern int player_busted_or_blackjack(Player* p);

extern void player_payoff(Player *p, bool dealer_blackjack, bool dealer_busted, int dealer_total);
extern void player_payoff_hand(Player *p, Wager *w, int dealer_blackjack, int dealer_busted, int dealer_total);
extern void payoff_split(Player *p, Wager *w, int dealer_busted, int dealer_total);
extern void player_mimic_dealer(Player* p, Shoe* s);
extern void get_have(Hand* h, int have_cards[13]);

#endif // PLAYER_H
