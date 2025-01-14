#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "wager.h"
#include "shoe.h"
#include "report.h"
#include "rules.h"
#include "strategy.h"
#include "constants.h"

//
struct PlayData {
	int bet;
	bool do_insurance;
	bool do_double;
	bool do_split;
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
	int seen_cards[MAXIMUM_CARD_VALUE + 1];
	Rules *rules;
	Strategy *strategy;
	Report report;
	struct PlayData play;
} Player;

// Global functions
Player *newPlayer(Rules *rules, Strategy *strategy, int number_of_cards);
void playerShuffle(Player *player);
void playerPlaceBet(Player *player, bool bet);
void playerInsurance(Player *player);
void playerPlay(Player *player, Shoe *shoe, Card *up, bool mimic);
void playerPlaySplit(Player *player, Wager *wager, Shoe *shoe, Card* up);
Card* playerDrawCard(Player *player, Hand *hand, Shoe *shoe);
void playerShowCard(Player *player, Card *ard);
int playerBustedOrBlackjack(Player *player);
void playerPayoff(Player *player, bool dealer_blackjack, bool dealer_busted, int dealer_total);

#endif // PLAYER_H
