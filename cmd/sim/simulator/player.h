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
struct PlayData {
	int bet;
	bool do_insurance;
	bool do_double;
	bool do_split;
	bool do_surrender;
	bool do_stand;
	bool do_play;
};

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
	struct PlayData play;
} Player;

// Global functions
Player* newPlayer(Parameters* parameters, Rules *rules, int number_of_cards);
void playerShuffle(Player* p);
void playerPlaceBet(Player* p, bool bet);
void playerInsurance(Player* p);
void playerPlay(Player* p, Shoe* s, Card* up, bool mimic);
void playerPlaySplit(Player* p, Wager* w, Shoe* s, Card* up);
Card* playerDrawCard(Player* p, Hand* h, Shoe* s);
void playerShowCard(Player* p, Card* c);
int playerBustedOrBlackjack(Player* p);
void playerPayoff(Player *p, bool dealer_blackjack, bool dealer_busted, int dealer_total);

#endif // PLAYER_H
