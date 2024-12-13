#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "table.h"
#include "player.h"
#include "simulator.h"

// Function to create a new simulation
Simulator *newSimulator(Parameters *parameters, Rules *rules, Strategy *strategy) {
	Simulator *simulator = (Simulator*)malloc(sizeof(Simulator));

	simulator->table = newTable(parameters, rules, strategy);
	simulator->parameters = parameters;
	simulator->rules = rules;
	simulator->strategy = strategy;
	initReport(&simulator->report);

	return simulator;
}

//
void simulatorDelete(Simulator *simulator) {
	free(simulator);
}

// The SimulatorProcess function
void simulatorRunOnce(Simulator *simulator) {
	Simulation simulation;

	printf("\n  Start: simulation(%s)\n", simulator->parameters->name);
	simulatorRunSimulation(simulator);
	printf("  End: simulation\n");

	// Populate the rest of the Simulation
	strcpy(simulation.playbook, simulator->parameters->playbook);
	strcpy(simulation.guid, simulator->parameters->name);
	simulation.simulator = STRIKER_WHO_AM_I;
	simulation.summary = "no";
	simulation.simulations = "1";
	serializeParameters(simulator->parameters, simulation.parameters, MAX_BUFFER_SIZE);
	serializeRules(simulator->rules, simulation.rules, MAX_BUFFER_SIZE);

	snprintf(simulation.rounds, 128, "%lld", simulator->report.total_rounds);
	snprintf(simulation.hands, 128, "%lld", simulator->report.total_hands);
	snprintf(simulation.total_bet, 128, "%lld", simulator->report.total_bet);
	snprintf(simulation.total_won, 128, "%lld", simulator->report.total_won);
	snprintf(simulation.total_time, 128, "%lld", simulator->report.duration);
	snprintf(simulation.average_time, 128, "%06.2f seconds", (float)simulator->report.duration * 1000000 / (float)simulator->report.total_hands);
	snprintf(simulation.advantage, 128, "%+04.3f%%", ((double)simulator->report.total_won / simulator->report.total_bet) * 100);

	// Format the rounds, hands, and other values into strings
	sprintf(simulation.rounds, "%lld", simulator->report.total_rounds);
	sprintf(simulation.hands, "%lld", simulator->report.total_hands);
	sprintf(simulation.total_bet, "%lld", simulator->report.total_bet);
	sprintf(simulation.total_won, "%lld", simulator->report.total_won);
	sprintf(simulation.total_time, "%lld", simulator->report.duration);
	sprintf(simulation.average_time, "%06.2f seconds", (float)simulator->report.duration * (float)1000000 / (float)simulator->report.total_hands);
	sprintf(simulation.advantage, "%+04.3f %%", ((double)simulator->report.total_won / simulator->report.total_bet) * 100);

	// Print out the results
    printf("\n  -- results ---------------------------------------------------------------------\n");
	printf("    %-24s: %lld\n", "Number of hands", simulator->report.total_hands);
	printf("    %-24s: %lld\n", "Number of rounds", simulator->report.total_rounds);
	printf("    %-24s: %lld %+04.3f average bet per hand\n", "Total bet", simulator->report.total_bet, (double)simulator->report.total_bet / simulator->report.total_hands);
	printf("    %-24s: %lld %+04.3f average win per hand\n", "Total won", simulator->report.total_won, (double)simulator->report.total_won / simulator->report.total_hands);
	printf("    %-24s: %lld %+04.3f percent of total hands\n", "Number of blackjacks", simulator->report.total_blackjacks, (double)simulator->report.total_blackjacks / simulator->report.total_hands * 100.0);
	printf("    %-24s: %lld %+04.3f percent of total hands\n", "Number of doubles", simulator->report.total_doubles, (double)simulator->report.total_doubles / simulator->report.total_hands * 100.0);
	printf("    %-24s: %lld %+04.3f percent of total hands\n", "Number of splits", simulator->report.total_splits, (double)simulator->report.total_splits / simulator->report.total_hands * 100.0);
	printf("    %-24s: %lld %+04.3f percent of total hands\n", "Number of wins", simulator->report.total_wins, (double)simulator->report.total_wins / simulator->report.total_hands * 100.0);
	printf("    %-24s: %lld %+04.3f percent of total hands\n", "Number of pushes", simulator->report.total_pushes, (double)simulator->report.total_pushes / simulator->report.total_hands * 100.0);
	printf("    %-24s: %lld %+04.3f percent of total hands\n", "Number of loses", simulator->report.total_loses, (double)simulator->report.total_loses / simulator->report.total_hands * 100.0);
	printf("    %-24s: %s seconds\n", "Total time", simulation.total_time);
	printf("    %-24s: %s per 1,000,000 hands\n", "Average time", simulation.average_time);
	printf("    %-24s: %s\n", "Player advantage", simulation.advantage);
    printf("  --------------------------------------------------------------------------------\n");

	if(simulator->report.total_hands >= DATABASE_NUMBER_OF_HANDS) {
		simulatorInsert(&simulation, simulator->parameters->playbook);
	}
}

// Function to run the simulation
void simulatorRunSimulation(Simulator *simulator) {
	printf("    Start: %s table session\n", simulator->parameters->strategy);
	tableSession(simulator->table, strcmp("mimic", simulator->parameters->strategy) == 0);
	printf("    End: table session\n");

	simulator->report.total_bet += simulator->table->player->report.total_bet;
	simulator->report.total_won += simulator->table->player->report.total_won;
	simulator->report.total_rounds += simulator->table->report.total_rounds;
	simulator->report.total_hands += simulator->table->report.total_hands;
	simulator->report.total_blackjacks += simulator->table->player->report.total_blackjacks;
	simulator->report.total_doubles += simulator->table->player->report.total_doubles;
	simulator->report.total_splits += simulator->table->player->report.total_splits;
	simulator->report.total_wins += simulator->table->player->report.total_wins;
	simulator->report.total_pushes += simulator->table->player->report.total_pushes;
	simulator->report.total_loses += simulator->table->player->report.total_loses;
	simulator->report.duration += simulator->table->report.duration;
}

// Function to insert a simulation into the database (HTTP POST)
void simulatorInsert(Simulation *simulation, const char *playbook) {
	struct curl_slist *headers = NULL;
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if (curl) {
		char url[MAX_BUFFER_SIZE];
		snprintf(url, MAX_BUFFER_SIZE, "http://%s/%s/%s/%s", getSimulationUrl(), simulation->simulator, playbook, simulation->guid);
    	printf("\n  -- insert ----------------------------------------------------------------------\n");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

		// Set headers
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		// Convert Simulation to JSON
		cJSON *json = cJSON_CreateObject();
		cJSON_AddStringToObject(json, "playbook", simulation->playbook);
		cJSON_AddStringToObject(json, "guid", simulation->guid);
		cJSON_AddStringToObject(json, "simulator", simulation->simulator);
		cJSON_AddStringToObject(json, "summary", "no");
		cJSON_AddStringToObject(json, "simulations", "1");
		cJSON_AddStringToObject(json, "rounds", simulation->rounds);
		cJSON_AddStringToObject(json, "hands", simulation->hands);
		cJSON_AddStringToObject(json, "total_bet", simulation->total_bet);
		cJSON_AddStringToObject(json, "total_won", simulation->total_won);
		cJSON_AddStringToObject(json, "advantage", simulation->advantage);
		cJSON_AddStringToObject(json, "total_time", simulation->total_time);
		cJSON_AddStringToObject(json, "average_time", simulation->average_time);
		cJSON_AddStringToObject(json, "parameters", simulation->parameters);
		cJSON_AddStringToObject(json, "rules", simulation->rules);
		cJSON_AddStringToObject(json, "payload", "n/a");
		// Add remaining fields...
		char *jsonStr = cJSON_Print(json);

		// Set POST fields
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr);
		//printf("curl -X POST %s -H \"Content-Type: application/json\" -d %s\n\n", url, jsonStr);

		// Perform the request
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			return;
		}

		// Cleanup
		curl_easy_cleanup(curl);
		cJSON_Delete(json);
		free(jsonStr);
    	printf("\n  --------------------------------------------------------------------------------\n");
	}

	curl_global_cleanup();
}

