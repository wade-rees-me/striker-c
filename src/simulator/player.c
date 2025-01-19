#include <stdlib.h>
#include <string.h>
#include "shoe.h"
#include "rules.h"
#include "player.h"
#include "strategy.h"
#include "constants.h"

//
void playerPayoffHand(Player *player, Wager *wager, int dealer_blackjack, int dealer_busted, int dealer_total);
void payoffSplit(Player *player, Wager *wager, int dealer_busted, int dealer_total);
bool mimicStand(Player *player);

// Utility function to create a new Player object
Player *newPlayer(Rules *rules, Strategy *strategy, int number_of_cards) {
	Player* player = (Player*)malloc(sizeof(Player));
	player->rules = rules;
	player->strategy = strategy;
	player->number_of_cards = number_of_cards;
	player->play.do_play = false;

	initReport(&player->report);
	initWager(&player->wager, MINIMUM_BET, MAXIMUM_BET);
	for (int i = 0; i < MAX_SPLIT_HANDS; i++) {
		initWager(&player->splits[i], MINIMUM_BET, MAXIMUM_BET);
	}

	return player;
}

// Shuffle function (reinitializes seen cards)
void playerShuffle(Player *player) {
	memset(player->seen_cards, 0, sizeof(player->seen_cards));
}

// Place a bet for the player
void playerPlaceBet(Player *player, bool mimic) {
	wagerReset(&player->wager);
	for (int i = 0; i < MAX_SPLIT_HANDS; i++) {
		player->splits[i].amount_bet = 0;
	}
	player->split_count = 0;
	wagerPlaceBet(&player->wager, mimic ? MINIMUM_BET : strategyGetBet(player->strategy, player->seen_cards));
}

//
void playerInsurance(Player *player) {
	if(strategyGetInsurance(player->strategy, player->seen_cards)) {
		player->wager.insurance_bet = player->wager.amount_bet / 2;
	}
}

//
void playerPlay(Player *player, Shoe *shoe, Card *up, bool mimic) {
	if (handIsBlackjack(&player->wager.hand)) {
		player->report.total_blackjacks++;
		return;
	}

	if(mimic) {
		while (!mimicStand(player)) {
			playerDrawCard(player, &player->wager.hand, shoe);
		}
		return;
	}

	// Check for double
	if (strategyGetDouble(player->strategy, player->seen_cards, player->wager.hand.hand_total, handIsSoft(&player->wager.hand), up)) {
		wagerDoubleBet(&player->wager);
		playerDrawCard(player, &player->wager.hand, shoe);
		player->report.total_doubles++;
		return;
	}

	if (handIsPair(&player->wager.hand) && strategyGetSplit(player->strategy, player->seen_cards, player->wager.hand.cards[0], up)) {
		Wager* split = &player->splits[player->split_count];
		player->split_count++;
		player->report.total_splits++;
		
		if (handIsPairOfAces(&player->wager.hand)) {
			wagerSplit(&player->wager, split);
			playerDrawCard(player, &player->wager.hand, shoe);
			playerDrawCard(player, &split->hand, shoe);
			return;
		}

		wagerSplit(&player->wager, split);
		playerDrawCard(player, &player->wager.hand, shoe);
		playerPlaySplit(player, &player->wager, shoe, up);
		playerDrawCard(player, &split->hand, shoe);
		playerPlaySplit(player, split, shoe, up);
		return;
	}

	// Handle the stand logic
	bool doStand = strategyGetStand(player->strategy, player->seen_cards, player->wager.hand.hand_total, handIsSoft(&player->wager.hand), up);
	while (!handIsBusted(&player->wager.hand) && !doStand) {
		playerDrawCard(player, &player->wager.hand, shoe);
		if (!handIsBusted(&player->wager.hand)) {
			doStand = strategyGetStand(player->strategy, player->seen_cards, player->wager.hand.hand_total, handIsSoft(&player->wager.hand), up);
		}
	}
}

// Simulate a split action
void playerPlaySplit(Player *player, Wager *wager, Shoe *shoe, Card *up) {
	if (handIsPair(&wager->hand) && player->split_count < MAX_SPLIT_HANDS && strategyGetSplit(player->strategy, player->seen_cards, wager->hand.cards[0], up)) {
		Wager* split = &player->splits[player->split_count];
		player->split_count++;
		player->report.total_splits++;

		wagerSplit(wager, split);
		playerDrawCard(player, &wager->hand, shoe);
		playerPlaySplit(player, wager, shoe, up);
		playerDrawCard(player, &split->hand, shoe);
		playerPlaySplit(player, split, shoe, up);
		return;
	}

	bool doStand = strategyGetStand(player->strategy, player->seen_cards, wager->hand.hand_total, handIsSoft(&wager->hand), up);
	while (!handIsBusted(&wager->hand) && !doStand) {
		playerDrawCard(player, &wager->hand, shoe);
		if (!handIsBusted(&wager->hand)) {
			doStand = strategyGetStand(player->strategy, player->seen_cards, wager->hand.hand_total, handIsSoft(&wager->hand), up);
		}
	}
}

//
Card *playerDrawCard(Player *player, Hand *hand, Shoe *shoe) {
	Card *card = shoeDrawCard(shoe);
	playerShowCard(player, card);
	handDrawCard(hand, card);
	return card;
}

//
void playerShowCard(Player *player, Card *card) {
	player->seen_cards[card->value]++;
}

// Function to check if the player busted or has blackjack
int playerBustedOrBlackjack(Player *player) {
	if (player->split_count == 0) {
		return handIsBusted(&player->wager.hand) || handIsBlackjack(&player->wager.hand);
	}
	for (int i = 0; i < player->split_count; i++) {
		if (!handIsBusted(&player->splits[i].hand)) {
			return 0;
		}
	}
	return 1;
}

//
void playerPayoff(Player *player, bool dealer_blackjack, bool dealer_busted, int dealer_total) {
	if (player->split_count == 0) {
		playerPayoffHand(player, &(player->wager), dealer_blackjack, dealer_busted, dealer_total);
		return;
	}

	payoffSplit(player, &(player->wager), dealer_busted, dealer_total);
	for (int i = 0; i < player->split_count; i++) {
		payoffSplit(player, &(player->splits[i]), dealer_busted, dealer_total);
	}
}

//
void playerPayoffHand(Player *player, Wager *wager, int dealer_blackjack, int dealer_busted, int dealer_total) {
	if (dealer_blackjack) {
		wagerWonInsurance(wager);
	} else {
		wagerLostInsurance(wager);
	}

	if (dealer_blackjack) {
		if (wagerIsBlackjack(wager)) {
			wagerPush(wager);
			player->report.total_pushes++;
		} else {
			wagerLost(wager);
			player->report.total_loses++;
		}
	} else {
		if (wagerIsBlackjack(wager)) {
			wagerWonBlackjack(wager, player->rules->blackjack_pays, player->rules->blackjack_bets);
		} else if (handIsBusted(&wager->hand)) {
			wagerLost(wager);
			player->report.total_loses++;
		} else if (dealer_busted || (wager->hand.hand_total > dealer_total)) {
			wagerWon(wager);
			player->report.total_wins++;
		} else if (dealer_total > wager->hand.hand_total) {
			wagerLost(wager);
			player->report.total_loses++;
		} else {
			wagerPush(wager);
			player->report.total_pushes++;
		}
	}

	player->report.total_won += wager->amount_won;
	player->report.total_bet += wager->amount_bet + wager->insurance_bet;
}

//
void payoffSplit(Player *player, Wager *wager, int dealer_busted, int dealer_total) {
	if (handIsBusted(&wager->hand)) {
		wagerLost(wager);
		player->report.total_loses++;
	} else if (dealer_busted || (wager->hand.hand_total > dealer_total)) {
		wagerWon(wager);
		player->report.total_wins++;
	} else if (dealer_total > wager->hand.hand_total) {
		wagerLost(wager);
		player->report.total_loses++;
	} else {
		wagerPush(wager);
		player->report.total_pushes++;
	}

	player->report.total_won += wager->amount_won;
	player->report.total_bet += wager->amount_bet;
}

//
bool mimicStand(Player *player) {
	if (handIsSoft17(&player->wager.hand)) {
		return false;
	}
	return handTotal(&player->wager.hand) >= 17;
}

