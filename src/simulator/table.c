#include "table.h"
#include "shoe.h"
#include "strategy.h"
#include <stdio.h>
#include <stdlib.h>

//
void status(int64_t round, int64_t hand);

// Function to create a new table
Table *newTable(Parameters *parameters, Rules *rules, Strategy *strategy) {
    Table *table = (Table *)malloc(sizeof(Table));

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
    table->report.start = time(NULL);
    while (table->report.total_hands < table->parameters->share_of_hands) {
        if (table->parameters->verbose) {
            status(table->report.total_rounds, table->report.total_hands);
        }
        shoeShuffle(table->shoe);
        playerShuffle(table->player);
        table->report.total_rounds++;

        while (!shoeShouldShuffle(table->shoe)) {
            table->report.total_hands++;
            dealerReset(table->dealer);
            playerPlaceBet(table->player, mimic);
            tableDealCards(table, table->player, &table->player->wager.hand, table->dealer, table->shoe);

            if (!mimic) {
                if (cardIsAce(table->up)) {
                    playerInsurance(table->player);
                }
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
            playerPayoff(table->player, handIsBlackjack(&table->dealer->hand), handIsBusted(&table->dealer->hand),
                         table->dealer->hand.hand_total);
        }
    }
    if (table->parameters->verbose) {
        printf("\r");
    }

    table->report.end = time(NULL);
    table->report.duration = table->report.end - table->report.start;
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
    const char *spinner = "|/-\\";
    printf("\r%c Simulating...", spinner[hand % 4]);
    fflush(stdout);
}

