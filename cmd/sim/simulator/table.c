#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "table.h"
#include "player_strategy.h"
#include "shoe.h"

// Function to create a new table
Table* new_table(Parameters *parameters) {
    Table *table = (Table*)malloc(sizeof(Table));

    table->parameters = parameters;
	table->shoe = new_shoe(parameters->number_of_decks, parameters->penetration);
	table->dealer = new_dealer();
	table->player = new_player(parameters, table->shoe->number_of_cards);
	report_init(&table->report);

    return table;
}

// Function to simulate a session (mimicking Go routine behavior)
void table_session(Table *table) {
    printf("    Beg table rounds: %lld\n", table->parameters->rounds);

    table->report.start = time(NULL);
    table->report.total_rounds = table->parameters->rounds;

    for (int64_t i = 0; i < table->parameters->rounds; i++) {
        shoe_shuffle(table->shoe);
        player_shuffle(table->player);

        while (!shoe_should_shuffle(table->shoe)) {
        	table->report.total_hands++;
            dealer_reset(table->dealer);
            player_place_bet(table->player, player_get_bet(table->player));
            Card* up = table_deal_cards(table->player, &table->player->wager.hand, table->dealer, table->shoe);

            if (card_is_ace(up)) {
                player_insurance(table->player);
            }

            if (!hand_is_blackjack(&table->dealer->hand)) {
				player_play(table->player, table->shoe, up);
                if (!player_busted_or_blackjack(table->player)) {
                    dealer_play(table->dealer, table->shoe);
                }
            }

            player_payoff(table->player, hand_is_blackjack(&table->dealer->hand), hand_is_busted(&table->dealer->hand), table->dealer->hand.hand_total);
            player_show_card(table->player, up);
		}
    }

    table->report.end = time(NULL);
    table->report.duration = table->report.end - table->report.start;
    printf("    End table: total elapsed time: %lld seconds\n", table->report.duration);
}

//
void table_session_mimic(Table *table) {
    printf("    Beg table (mimic): rounds: %lld\n", table->parameters->rounds);

    table->report.start = time(NULL);
    table->report.total_rounds = table->parameters->rounds;

    for (int64_t i = 0; i < table->parameters->rounds; i++) {
        shoe_shuffle(table->shoe);
        player_shuffle(table->player);

        while (!shoe_should_shuffle(table->shoe)) {
            table->report.total_hands++;
            dealer_reset(table->dealer);
            player_place_bet(table->player, MINIMUM_BET);
            table_deal_cards(table->player, &table->player->wager.hand, table->dealer, table->shoe);

            if (!hand_is_blackjack(&table->dealer->hand)) {
                player_mimic_dealer(table->player, table->shoe);
                if (!player_busted_or_blackjack(table->player)) {
                    dealer_play(table->dealer, table->shoe);
                }
            }

            player_payoff(table->player, hand_is_blackjack(&table->dealer->hand), hand_is_busted(&table->dealer->hand), table->dealer->hand.hand_total);
        }
    }

    table->report.end = time(NULL);
    table->report.duration = table->report.end - table->report.start;

    printf("    End table (mimic): total elapsed time: %.2lld seconds\n\n", table->report.duration);
}

// Function to deal cards
Card* table_deal_cards(Player* player, Hand* hand, Dealer* dealer, Shoe* shoe) {
	player_draw_card(player, hand, shoe);
    Card* up = dealer_draw_card(dealer, shoe_draw_card(shoe));
	player_draw_card(player, hand, shoe);
    dealer_draw_card(dealer, shoe_draw_card(shoe));
	player_show_card(player, up);
    return up;
}

// Function to show cards
void table_show(Table *table, Card *card) {
	player_show_card(table->player, card);
}

