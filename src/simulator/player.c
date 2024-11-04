#include <stdlib.h>
#include <string.h>
#include "shoe.h"
#include "rules.h"
#include "player.h"
#include "strategy.h"
#include "constants.h"

//
void playerPayoffHand(Player *p, Wager *w, int dealer_blackjack, int dealer_busted, int dealer_total);
void payoffSplit(Player *p, Wager *w, int dealer_busted, int dealer_total);
bool mimicStand(Player* p);

// Utility function to create a new Player object
Player* newPlayer(Rules *rules, Strategy* strategy, int number_of_cards) {
	Player* p = (Player*)malloc(sizeof(Player));
	p->rules = rules;
	p->strategy = strategy;
	p->number_of_cards = number_of_cards;
	p->play.do_play = false;

	initReport(&p->report);
	initWager(&p->wager, MINIMUM_BET, MAXIMUM_BET);
	for (int i = 0; i < MAX_SPLIT_HANDS; i++) {
		initWager(&p->splits[i], MINIMUM_BET, MAXIMUM_BET);
	}

	return p;
}

// Shuffle function (reinitializes seen cards)
void playerShuffle(Player* p) {
	memset(p->seen_cards, 0, sizeof(p->seen_cards));
}

// Place a bet for the player
void playerPlaceBet(Player* p, bool mimic) {
	wagerReset(&p->wager);
	for (int i = 0; i < MAX_SPLIT_HANDS; i++) {
		p->splits[i].amount_bet = 0;
	}
	p->split_count = 0;
	wagerPlaceBet(&p->wager, mimic ? MINIMUM_BET : strategyGetBet(p->strategy, p->seen_cards));
}

//
void playerInsurance(Player* player) {
	if(strategyGetInsurance(player->strategy, player->seen_cards)) {
		player->wager.insurance_bet = player->wager.amount_bet / 2;
	}
}

//
void playerPlay(Player* player, Shoe* s, Card* up, bool mimic) {
	if (handIsBlackjack(&player->wager.hand)) {
		return;
	}

    if(mimic) {
        while (!mimicStand(player)) {
			playerDrawCard(player, &player->wager.hand, s);
        }
        return;
    }

	// Check for double
	if (strategyGetDouble(player->strategy, player->seen_cards, player->wager.hand.hand_total, handIsSoft(&player->wager.hand), up)) {
		wagerDoubleBet(&player->wager);
		playerDrawCard(player, &player->wager.hand, s);
		return;
	}

	if (handIsPair(&player->wager.hand) && strategyGetSplit(player->strategy, player->seen_cards, player->wager.hand.cards[0], up)) {
		Wager* split = &player->splits[player->split_count];
		player->split_count++;
		
		if (handIsPairOfAces(&player->wager.hand)) {
			wagerSplit(&player->wager, split);
			playerDrawCard(player, &player->wager.hand, s);
			playerDrawCard(player, &split->hand, s);
			return;
		}

		wagerSplit(&player->wager, split);
		playerDrawCard(player, &player->wager.hand, s);
		playerPlaySplit(player, &player->wager, s, up);
		playerDrawCard(player, &split->hand, s);
		playerPlaySplit(player, split, s, up);
		return;
	}

	// Handle the stand logic
	bool doStand = strategyGetStand(player->strategy, player->seen_cards, player->wager.hand.hand_total, handIsSoft(&player->wager.hand), up);
	while (!handIsBusted(&player->wager.hand) && !doStand) {
		playerDrawCard(player, &player->wager.hand, s);
		if (!handIsBusted(&player->wager.hand)) {
			doStand = strategyGetStand(player->strategy, player->seen_cards, player->wager.hand.hand_total, handIsSoft(&player->wager.hand), up);
		}
	}
}

// Simulate a split action
void playerPlaySplit(Player* p, Wager* w, Shoe* s, Card* up) {
	if (handIsPair(&w->hand) && p->split_count < MAX_SPLIT_HANDS && strategyGetSplit(p->strategy, p->seen_cards, w->hand.cards[0], up)) {
		Wager* split = &p->splits[p->split_count];
		p->split_count++;

		wagerSplit(w, split);
		playerDrawCard(p, &w->hand, s);
		playerPlaySplit(p, w, s, up);
		playerDrawCard(p, &split->hand, s);
		playerPlaySplit(p, split, s, up);
		return;
	}

	bool doStand = strategyGetStand(p->strategy, p->seen_cards, w->hand.hand_total, handIsSoft(&w->hand), up);
	while (!handIsBusted(&w->hand) && !doStand) {
		playerDrawCard(p, &w->hand, s);
		if (!handIsBusted(&w->hand)) {
			doStand = strategyGetStand(p->strategy, p->seen_cards, w->hand.hand_total, handIsSoft(&w->hand), up);
		}
	}
}

//
Card* playerDrawCard(Player* p, Hand* h, Shoe* s) {
	Card* card = shoeDrawCard(s);
	playerShowCard(p, card);
	handDrawCard(h, card);
	return card;
}

//
void playerShowCard(Player* p, Card* c) {
	p->seen_cards[c->offset]++;
}

// Function to check if the player busted or has blackjack
int playerBustedOrBlackjack(Player* p) {
	if (p->split_count == 0) {
		return handIsBusted(&p->wager.hand) || handIsBlackjack(&p->wager.hand);
	}
	for (int i = 0; i < p->split_count; i++) {
		if (!handIsBusted(&p->splits[i].hand)) {
			return 0;
		}
	}
	return 1;
}

//
void playerPayoff(Player *p, bool dealer_blackjack, bool dealer_busted, int dealer_total) {
	if (p->split_count == 0) {
		playerPayoffHand(p, &(p->wager), dealer_blackjack, dealer_busted, dealer_total);
		return;
	}

	payoffSplit(p, &(p->wager), dealer_busted, dealer_total);
	for (int i = 0; i < p->split_count; i++) {
		payoffSplit(p, &(p->splits[i]), dealer_busted, dealer_total);
	}
}

//
void playerPayoffHand(Player *p, Wager *w, int dealer_blackjack, int dealer_busted, int dealer_total) {
	if (dealer_blackjack) {
		wagerWonInsurance(w);
	} else {
		wagerLostInsurance(w);
	}

	if (dealer_blackjack) {
		if (wagerIsBlackjack(w)) {
			wagerPush(w);
		} else {
			wagerLost(w);
		}
	} else if (wagerIsBlackjack(w)) {
		wagerWonBlackjack(w, p->rules->blackjack_pays, p->rules->blackjack_bets);
	} else if (handIsBusted(&w->hand)) {
		wagerLost(w);
	} else if (dealer_busted || (w->hand.hand_total > dealer_total)) {
		wagerWon(w);
	} else if (dealer_total > w->hand.hand_total) {
		wagerLost(w);
	} else {
		wagerPush(w);
	}
	p->report.total_won += w->amount_won;
	p->report.total_bet += w->amount_bet + w->insurance_bet;
}

//
void payoffSplit(Player *p, Wager *w, int dealer_busted, int dealer_total) {
	if (handIsBusted(&w->hand)) {
		wagerLost(w);
	} else if (dealer_busted || (w->hand.hand_total > dealer_total)) {
		wagerWon(w);
	} else if (dealer_total > w->hand.hand_total) {
		wagerLost(w);
	} else {
		wagerPush(w);
	}

	p->report.total_won += w->amount_won;
	p->report.total_bet += w->amount_bet;
}

//
bool mimicStand(Player* p) {
    if (handIsSoft17(&p->wager.hand)) {
        return false;
    }
    return handTotal(&p->wager.hand) >= 17;
}

