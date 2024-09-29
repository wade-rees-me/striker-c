#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include "wager.h"
#include "shoe.h"
#include "report.h"
#include "parameters.h"
#include "rules.h"
#include "constants.h"

//
typedef struct {
	Wager wager;
	Wager splits[MAX_SPLIT_HANDS];
	int split_count;
	int64_t blackjack_pays;
	int64_t blackjack_bets;
	int number_of_cards;
	int seen_cards[13];
	Parameters* parameters;
	Rules* rules;
	Report report;
} Player;

//
Player* newPlayer(Parameters* parameters, Rules *rules, int number_of_cards);
extern void playerShuffle(Player* p);
extern void playerPlaceBet(Player* p, bool bet);
extern void playerInsurance(Player* p);
void playerPlay(Player* p, Shoe* s, Card* up, bool mimic);
extern void playerPlaySplit(Player* p, Wager* w, Shoe* s, Card* up);
extern Card* playerDrawCard(Player* p, Hand* h, Shoe* s);
extern void playerShowCard(Player* p, Card* c);
extern int playerBustedOrBlackjack(Player* p);

extern void playerPayoff(Player *p, bool dealer_blackjack, bool dealer_busted, int dealer_total);
extern void playerPayoffHand(Player *p, Wager *w, int dealer_blackjack, int dealer_busted, int dealer_total);
extern void payoffSplit(Player *p, Wager *w, int dealer_busted, int dealer_total);

#endif // PLAYER_H
