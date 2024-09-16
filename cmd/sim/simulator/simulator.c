#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "table.h"
#include "player.h"
#include "simulator.h"

//
char* serialize_parameters(Parameters* parameters);

// Function to create a new simulation
Simulation* new_simulation(Parameters *parameters) {
	Simulation *s = (Simulation*)malloc(sizeof(Simulation));
	time_t t = time(NULL);
	struct tm *tm_info = localtime(&t);

	snprintf(s->name, sizeof(s->name), "striker-c--%4d_%02d_%02d_%012ld", s->year, s->month, s->day, t);
	s->year = tm_info->tm_year + 1900;
	s->month = tm_info->tm_mon + 1;
	s->day = tm_info->tm_mday;

	s->table = new_table(parameters);
	s->parameters = parameters;
	report_init(&s->report);

	return s;
}

void simulation_delete(Simulation* sim) {
	free(sim);
}

// The SimulatorProcess function
void simulator_run_once(Simulation *s) {
	SimulationDatabaseTable tbs;

	printf("Starting striker-c simulation ...\n");
	simulator_run_simulation(s);
	printf("Ending striker-c simulation ...\n\n");

	// Populate the rest of the SimulationDatabaseTable
	strcpy(tbs.playbook, s->parameters->playbook);
	strcpy(tbs.guid, s->parameters->guid);
	tbs.simulator = "StrikerWhoAmI";
	tbs.summary = "no";
	tbs.simulations = "1";
	tbs.parameters = serialize_parameters(s->parameters);

	// Format the rounds, hands, and other values into strings
	sprintf(tbs.rounds, "%lld", s->report.total_rounds);
	sprintf(tbs.hands, "%lld", s->report.total_hands);
	sprintf(tbs.total_bet, "%lld", s->report.total_bet);
	sprintf(tbs.total_won, "%lld", s->report.total_won);
	sprintf(tbs.total_time, "%lld", s->report.duration);
	sprintf(tbs.average_time, "%06.2f seconds", (float)s->report.duration * (float)1000000 / (float)s->report.total_hands);
	sprintf(tbs.advantage, "%+04.3f%%", ((double)s->report.total_won / s->report.total_bet) * 100);

	// Print out the results
	printf("Number of rounds:  %s\n", tbs.rounds);
	printf("Number of hands:   %lld\n", s->report.total_hands);
	printf("Total bet:		 %lld, average bet per hand: %.2f\n", s->report.total_bet, (double)s->report.total_bet / s->report.total_hands);
	printf("Total won:		 %lld, average win per hand: %.2f\n", s->report.total_won, (double)s->report.total_won / s->report.total_hands);
	printf("Total time:		%s seconds\n", tbs.total_time);
	printf("Average time:	  %s per 1,000,000 hands\n", tbs.average_time);
	printf("Player advantage:  %s\n", tbs.advantage);
	printf("\n");

	simulator_insert(&tbs, s->parameters->playbook);
}

// Function to run the simulation
void simulator_run_simulation(Simulation *sim) {
	if (strcmp("mimic", sim->parameters->strategy) == 0) {
		printf("  Starting mimic table session ...\n");
		table_session_mimic(sim->table);
		printf("  Ending mimic table session ...\n");
	} else {
		printf("  Starting table session ...\n");
		table_session(sim->table);
		printf("  Ending table session ...\n");
	}

	sim->report.total_bet += sim->table->player->report.total_bet;
	sim->report.total_won += sim->table->player->report.total_won;
	sim->report.total_rounds += sim->table->report.total_rounds;
	sim->report.total_hands += sim->table->report.total_hands;
	sim->report.duration += sim->table->report.duration;
}

// Function to insert a simulation into the database (HTTP POST)
void simulator_insert(SimulationDatabaseTable *sdt, const char *playbook) {
	CURL *curl;
	CURLcode res;
	struct curl_slist *headers = NULL;
	struct MemoryStruct chunk;

	chunk.memory = malloc(1);
	chunk.size = 0;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if (curl) {
		// Set URL
		char url[256];
		snprintf(url, sizeof(url), "http://%s/%s/%s/%s", get_simulation_url(), sdt->simulator, playbook, sdt->guid);
		curl_easy_setopt(curl, CURLOPT_URL, url);

		// Set headers
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		// Convert SimulationDatabaseTable to JSON
		cJSON *json = cJSON_CreateObject();
		cJSON_AddStringToObject(json, "playbook", sdt->playbook);
		cJSON_AddStringToObject(json, "guid", sdt->guid);
		cJSON_AddStringToObject(json, "simulator", sdt->simulator);
		// Add remaining fields...
		char *jsonStr = cJSON_Print(json);

		// Set POST fields
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr);

		// Perform the request
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			return;
		}

		// Cleanup
		curl_easy_cleanup(curl);
		free(chunk.memory);
		cJSON_Delete(json);
		free(jsonStr);
	}

	curl_global_cleanup();
}

// Function to serialize the struct to JSON
char* serialize_parameters(Parameters* parameters) {
	cJSON *json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "guid", parameters->guid);
	cJSON_AddStringToObject(json, "processor", parameters->processor);
	cJSON_AddStringToObject(json, "timestamp", parameters->timestamp);
	cJSON_AddStringToObject(json, "decks", parameters->decks);
	cJSON_AddStringToObject(json, "strategy", parameters->strategy);
	cJSON_AddStringToObject(json, "playbook", parameters->playbook);
	cJSON_AddStringToObject(json, "blackjack_pays", parameters->blackjack_pays);
	cJSON_AddNumberToObject(json, "tables", parameters->tables);
	cJSON_AddNumberToObject(json, "rounds", parameters->rounds);
	cJSON_AddNumberToObject(json, "number_of_decks", parameters->number_of_decks);
	cJSON_AddNumberToObject(json, "penetration", parameters->penetration);

	// Convert the cJSON object to a string
	char *json_string = cJSON_Print(json);

	// Free the cJSON object
	cJSON_Delete(json);

	return json_string;
}

