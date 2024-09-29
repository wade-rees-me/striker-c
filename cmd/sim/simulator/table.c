#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "table.h"
#include "strategy.h"
#include "shoe.h"

const int STATUS_DOT = 25000;
const int STATUS_LINE = 1000000;

void status(int64_t round, Logger *logger);

// Function to create a new table
Table* newTable(Parameters *parameters, Rules *rules) {
	Table *table = (Table*)malloc(sizeof(Table));

	table->parameters = parameters;
	table->rules = rules;
	table->shoe = newShoe(parameters->number_of_decks, 0.75/*parameters->penetration*/);
	table->dealer = newDealer(rules->hit_soft_17);
	table->player = newPlayer(parameters, rules, table->shoe->number_of_cards);
	reportInit(&table->report);

	return table;
}

void tableSession(Table *table, bool mimic) {
    char buffer[256];

	sprintf(buffer, "      Starting table rounds: %lld\n", table->parameters->rounds);
	Logger_simulation(table->parameters->logger, buffer);

	table->report.start = time(NULL);
	table->report.total_rounds = table->parameters->rounds;

	for (int64_t i = 0; i < table->parameters->rounds; i++) {
		status(i, table->parameters->logger);
		shoeShuffle(table->shoe);
		playerShuffle(table->player);

		while (!shoeShouldShuffle(table->shoe)) {
			table->report.total_hands++;
			dealerReset(table->dealer);
			playerPlaceBet(table->player, mimic);
			Card* up = tableDealCards(table->player, &table->player->wager.hand, table->dealer, table->shoe);

			if (!mimic && cardIsAce(up)) {
				playerInsurance(table->player);
			}

			if (!handIsBlackjack(&table->dealer->hand)) {
				playerPlay(table->player, table->shoe, up, mimic);
				if (!playerBustedOrBlackjack(table->player)) {
					dealerPlay(table->dealer, table->shoe);
				}
			}

			playerPayoff(table->player, handIsBlackjack(&table->dealer->hand), handIsBusted(&table->dealer->hand), table->dealer->hand.hand_total);
			playerShowCard(table->player, up);
		}
	}

	table->report.end = time(NULL);
	table->report.duration = table->report.end - table->report.start;
	sprintf(buffer, "\n      Ending table: total elapsed time: %lld seconds\n", table->report.duration);
	Logger_simulation(table->parameters->logger, buffer);
}

// Function to deal cards
Card* tableDealCards(Player* player, Hand* hand, Dealer* dealer, Shoe* shoe) {
	playerDrawCard(player, hand, shoe);
	Card* up = dealerDrawCard(dealer, shoeDrawCard(shoe));
	playerDrawCard(player, hand, shoe);
	dealerDrawCard(dealer, shoeDrawCard(shoe));
	playerShowCard(player, up);
	return up;
}

// Function to show cards
void tableShow(Table *table, Card *card) {
	playerShowCard(table->player, card);
}

//
void status(int64_t round, Logger *logger) {
    char buffer[256];

    if(round == 0) {
		Logger_simulation(logger, "      ");
		//fflush(stdout);
    }
    if((round + 1) % STATUS_DOT == 0) {
		Logger_simulation(logger, ".");
        //printf(".");
		//fflush(stdout);
    }
    if((round + 1) % STATUS_LINE == 0) {
        sprintf(buffer, "%12lld\n", (round + 1));
		Logger_simulation(logger, buffer);
		Logger_simulation(logger, "      ");
		//fflush(stdout);
    }
}

