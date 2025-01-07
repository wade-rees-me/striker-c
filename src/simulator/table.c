#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "strategy.h"
#include "shoe.h"

//
void status(int64_t round, int64_t hand);

// Function to create a new table
Table *newTable(Parameters *parameters, Rules *rules, Strategy *strategy) {
	Table *table = (Table*)malloc(sizeof(Table));

	table->parameters = parameters;
	table->rules = rules;
	table->strategy = strategy;
	table->shoe = newShoe(parameters->number_of_decks, rules->penetration);
	table->dealer = newDealer(rules->hit_soft_17);
	table->player = newPlayer(rules, strategy, table->shoe->number_of_cards);
	initReport(&table->report);

	return table;
}

//
void tableSession(Table *table, bool mimic) {
	char buffer[MAX_BUFFER_SIZE];

	printf("      Start: table, playing %s hands\n", convertToStringWithCommas(table->parameters->number_of_hands, buffer, MAX_BUFFER_SIZE));

	table->report.start = time(NULL);
	while (table->report.total_hands < table->parameters->number_of_hands) {
		status(table->report.total_rounds, table->report.total_hands);
		shoeShuffle(table->shoe);
		playerShuffle(table->player);
		table->report.total_rounds++;

		while (!shoeShouldShuffle(table->shoe)) {
			table->report.total_hands++;
			dealerReset(table->dealer);
			playerPlaceBet(table->player, mimic);
			tableDealCards(table, table->player, &table->player->wager.hand, table->dealer, table->shoe);

			if (!mimic && cardIsAce(table->up)) {
				playerInsurance(table->player);
			}

			if (!handIsBlackjack(&table->dealer->hand)) {
				playerPlay(table->player, table->shoe, table->up, mimic);
				if (!playerBustedOrBlackjack(table->player)) {
					while (!dealerStand(table->dealer)) {
						Card *card = shoeDrawCard(table->shoe);
						dealerDrawCard(table->dealer, card);
						playerShowCard(table->player, card);
					}
				}
			}

			playerShowCard(table->player, table->down);
			playerPayoff(table->player, handIsBlackjack(&table->dealer->hand), handIsBusted(&table->dealer->hand), table->dealer->hand.hand_total);
		}
	}

	table->report.end = time(NULL);
	table->report.duration = table->report.end - table->report.start;
	printf("\n      End: table\n");
}

// Function to deal cards
void tableDealCards(Table *table, Player *player, Hand *hand, Dealer *dealer, Shoe *shoe) {
	playerDrawCard(player, hand, shoe);
	table->up = shoeDrawCard(shoe);
	dealerDrawCard(dealer, table->up);
	playerShowCard(player, table->up);

	playerDrawCard(player, hand, shoe);
	table->down = shoeDrawCard(shoe);
	dealerDrawCard(dealer, table->down);
}

//
void status(int64_t round, int64_t hand) {
	char buffer1[MAX_BUFFER_SIZE];
	char buffer2[MAX_BUFFER_SIZE];

    if(round == 0) {
		printf("        ");
    }
    if((round + 1) % STATUS_DOT == 0) {
		printf(".");
    }
    if((round + 1) % STATUS_LINE == 0) {
        printf(" : %s (rounds), %s (hands)\n", convertToStringWithCommas(round + 1, buffer1, MAX_BUFFER_SIZE), convertToStringWithCommas(hand, buffer2, MAX_BUFFER_SIZE));
		printf("        ");
    }
	fflush(stdout);
}

