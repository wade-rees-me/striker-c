#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>
#include <time.h>
#include <getopt.h>
#include "simulator.h"
#include "rules.h"
#include "arguments_simulation.h"
#include "arguments_strategy.h"
#include "arguments_table.h"
#include "constants.h"

//
void generate_uuid(char* buffer);
void get_current_time(char* buffer, size_t size);

//
int main(int argc, char* argv[]) {
	printf("Starting striker-c ...\n");

	simulation_parse_flags(argc, argv);
	strategy_parse_flags(argc, argv);
	table_parse_flags(argc, argv);

	Parameters params;
	int number_of_decks = 0;
	char const *decks = table_get(&number_of_decks);

	// Simulation Parameters structure
	generate_uuid(params.guid);
	params.processor = STRIKER_WHO_AM_I;
	get_current_time(params.timestamp, sizeof(params.timestamp));
	params.decks = decks;
	params.number_of_decks = number_of_decks; // Assuming the deck string is a number
	params.strategy = strategy_get();
	sprintf(params.playbook, "%s-%s", params.decks, params.strategy);
    params.blackjack_pays = CLSimulation.blackjack_pays;
    params.rounds = CLSimulation.rounds;
    params.penetration = CLSimulation.penetration;

	rules_load_table(params.decks);

	printf("\n");
	printf("%-18s: %s\n", "Guid", params.guid);
	printf("%-18s: %s\n", "Processor", params.processor);
	printf("%-18s: %s\n", "Timestamp", params.timestamp);
	printf("%-18s: %s\n", "Decks", params.decks);
	printf("%-18s: %d\n", "NumberOfDecks", params.number_of_decks);
	printf("%-18s: %s\n", "Strategy", params.strategy);
	printf("%-18s: %s\n", "Playbook", params.playbook);
    printf("%-18s: %s\n", "Blackjack Pays", params.blackjack_pays);
    printf("%-18s: %lld\n", "Number of rounds", params.rounds);
    printf("%-18s: %f\n", "Penetration", params.penetration);
    printf("%-18s: %s\n", "Rules", TableRules.playbook);
	printf("\n");

	Simulation* sim = new_simulation(&params);
	simulator_run_once(sim);
	simulation_delete(sim);

	return 0;
}

// Function to generate a UUID (similar to uuid.New() in Go)
void generate_uuid(char* buffer) {
	uuid_t uuid;
	uuid_generate(uuid);
	uuid_unparse(uuid, buffer);
}

// Function to get current time and format it (like time.Now().Format in Go)
void get_current_time(char* buffer, size_t size) {
	time_t t = time(NULL);
	struct tm* tm_info = localtime(&t);
	strftime(buffer, size, TIME_LAYOUT, tm_info);
}

