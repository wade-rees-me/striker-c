#include <stdlib.h>
#include <string.h>
#include "shoe.h"
#include "rules.h"
#include "player.h"
#include "strategy.h"

//
void playerPayoffHand(Player *p, Wager *w, int dealer_blackjack, int dealer_busted, int dealer_total);
void payoffSplit(Player *p, Wager *w, int dealer_busted, int dealer_total);
bool mimicStand(Player* p);

// Utility function to create a new Player object
Player* newPlayer(Parameters* parameters, Rules *rules, Strategy* strategy, int number_of_cards) {
	Player* p = (Player*)malloc(sizeof(Player));
	p->parameters = parameters;
	p->rules = rules;
	p->strategy = strategy;
	p->number_of_cards = number_of_cards;
	p->play.do_play = false;

	initReport(&p->report);

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

	// Check for surrender
	if (player->rules->surrender && strategyGetSurrender(player->strategy, getHaveCards(&player->wager.hand), player->wager.hand.hand_total, handIsSoft(&player->wager.hand), up)) {
		player->wager.hand.surrender = true;
		return;
	}

	// Check for double
	if ((player->rules->double_any_two_cards || (handTotal(&player->wager.hand) == 10 || handTotal(&player->wager.hand) == 11)) && strategyGetDouble(player->strategy, getHaveCards(&player->wager.hand), player->wager.hand.hand_total, handIsSoft(&player->wager.hand), up)) {
		wagerDoubleBet(&player->wager);
		playerDrawCard(player, &player->wager.hand, s);
		return;
	}

	// Check for splitting
	if (handIsPair(&player->wager.hand) && strategyGetSplit(player->strategy, player->seen_cards, player->wager.hand.cards[0], up)) {
		Wager* split = &player->splits[player->split_count];
		player->split_count++;
		
		if (handIsPairOfAces(&player->wager.hand)) {
			if (!player->rules->resplit_aces && !player->rules->hit_split_aces) {
				wagerSplit(&player->wager, split);
				playerDrawCard(player, &player->wager.hand, s);
				playerDrawCard(player, &split->hand, s);
				return;
			}
		}

		wagerSplit(&player->wager, split);
		playerDrawCard(player, &player->wager.hand, s);
		playerPlaySplit(player, &player->wager, s, up);
		playerDrawCard(player, &split->hand, s);
		playerPlaySplit(player, split, s, up);
		return;
	}

	// Handle the stand logic
	bool doStand = strategyGetStand(player->strategy, getHaveCards(&player->wager.hand), player->wager.hand.hand_total, handIsSoft(&player->wager.hand), up);
	while (!handIsBusted(&player->wager.hand) && !doStand) {
		playerDrawCard(player, &player->wager.hand, s);
		if (!handIsBusted(&player->wager.hand)) {
			doStand = strategyGetStand(player->strategy, getHaveCards(&player->wager.hand), player->wager.hand.hand_total, handIsSoft(&player->wager.hand), up);
		}
	}
}

// Simulate a split action
void playerPlaySplit(Player* p, Wager* w, Shoe* s, Card* up) {
	// Check if player can double after split
	if (p->rules->double_after_split && strategyGetDouble(p->strategy, getHaveCards(&p->wager.hand), p->wager.hand.hand_total, handIsSoft(&p->wager.hand), up)) {
		wagerDoubleBet(&p->wager);
		playerDrawCard(p, &w->hand, s);
		return;
	}

	// Check if the hand is a pair and the player can split
	if (handIsPair(&w->hand) && p->split_count < 4) {  // Assuming 4 is the max number of split hands
		if (strategyGetSplit(p->strategy, p->seen_cards, w->hand.cards[0], up)) {
			Wager* split = &p->splits[p->split_count];
			p->split_count++;

			if (handIsPairOfAces(&w->hand)) {
				if (!p->rules->resplit_aces && !p->rules->hit_split_aces) {
					wagerSplit(w, split);
					playerDrawCard(p, &w->hand, s);
					playerDrawCard(p, &split->hand, s);
					return;
				}
			}

			wagerSplit(w, split);
			playerDrawCard(p, &w->hand, s);
			playerPlaySplit(p, w, s, up);
			playerDrawCard(p, &split->hand, s);
			playerPlaySplit(p, split, s, up);
			return;
		}
	}

	// Check if the hand contains a Blackjack Ace and if the rule allows hitting split Aces
	if (w->hand.cards[0]->value == 11 && !p->rules->hit_split_aces) {
		return;
	}

	// Implement stand logic
	bool doStand = strategyGetStand(p->strategy, getHaveCards(&p->wager.hand), p->wager.hand.hand_total, handIsSoft(&p->wager.hand), up);
	while (!handIsBusted(&w->hand) && !doStand) {
		playerDrawCard(p, &w->hand, s);
		if (!handIsBusted(&w->hand)) {
			doStand = strategyGetStand(p->strategy, getHaveCards(&p->wager.hand), p->wager.hand.hand_total, handIsSoft(&p->wager.hand), up);
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

	if (w->hand.surrender) {
		p->report.total_won -= w->amount_bet / 2;
	} else {
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
	}
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

