#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "table.h"
#include "player.h"
#include "simulator.h"

// Function to create a new simulation
Simulator* newSimulator(Parameters *parameters, Rules *rules) {
	Simulator *s = (Simulator*)malloc(sizeof(Simulator));

	s->table = newTable(parameters, rules);
	s->parameters = parameters;
	s->rules = rules;
	initReport(&s->report);

	return s;
}

//
void simulatorDelete(Simulator* sim) {
	free(sim);
}

// The SimulatorProcess function
void simulatorRunOnce(Simulator *s) {
	SimulationDatabaseTable tbs;

	printf("\n  Start: simulation(%s)\n", s->parameters->name);
	simulatorRunSimulation(s);
	printf("  End: simulation\n");

	// Populate the rest of the SimulationDatabaseTable
	strcpy(tbs.playbook, s->parameters->playbook);
	strcpy(tbs.guid, s->parameters->name);
	tbs.simulator = STRIKER_WHO_AM_I;
	tbs.summary = "no";
	tbs.simulations = "1";
	serializeParameters(s->parameters, tbs.parameters, MAX_BUFFER_SIZE);
	serializeRules(s->rules, tbs.rules, MAX_BUFFER_SIZE);

	snprintf(tbs.rounds, 128, "%lld", s->report.total_rounds);
	snprintf(tbs.hands, 128, "%lld", s->report.total_hands);
	snprintf(tbs.total_bet, 128, "%lld", s->report.total_bet);
	snprintf(tbs.total_won, 128, "%lld", s->report.total_won);
	snprintf(tbs.total_time, 128, "%lld", s->report.duration);
	snprintf(tbs.average_time, 128, "%06.2f seconds", (float)s->report.duration * 1000000 / (float)s->report.total_hands);
	snprintf(tbs.advantage, 128, "%+04.3f%%", ((double)s->report.total_won / s->report.total_bet) * 100);

	// Format the rounds, hands, and other values into strings
	sprintf(tbs.rounds, "%lld", s->report.total_rounds);
	sprintf(tbs.hands, "%lld", s->report.total_hands);
	sprintf(tbs.total_bet, "%lld", s->report.total_bet);
	sprintf(tbs.total_won, "%lld", s->report.total_won);
	sprintf(tbs.total_time, "%lld", s->report.duration);
	sprintf(tbs.average_time, "%06.2f seconds", (float)s->report.duration * (float)1000000 / (float)s->report.total_hands);
	sprintf(tbs.advantage, "%+04.3f %%", ((double)s->report.total_won / s->report.total_bet) * 100);

	// Print out the results
    printf("\n  -- results ---------------------------------------------------------------------\n");
	printf("    %-24s: %lld\n", "Number of hands", s->report.total_hands);
	printf("    %-24s: %lld\n", "Number of rounds", s->report.total_rounds);
	printf("    %-24s: %lld %+04.3f average bet per hand\n", "Total bet", s->report.total_bet, (double)s->report.total_bet / s->report.total_hands);
	printf("    %-24s: %lld %+04.3f average win per hand\n", "Total won", s->report.total_won, (double)s->report.total_won / s->report.total_hands);
	printf("    %-24s: %s seconds\n", "Total time", tbs.total_time);
	printf("    %-24s: %s per 1,000,000 hands\n", "Average time", tbs.average_time);
	printf("    %-24s: %s\n", "Player advantage", tbs.advantage);
    printf("  --------------------------------------------------------------------------------\n");

	if(s->report.total_hands >= DATABASE_NUMBER_OF_HANDS) {
		simulatorInsert(s, &tbs, s->parameters->playbook);
	}
}

// Function to run the simulation
void simulatorRunSimulation(Simulator *sim) {
	printf("    Start: %s table session\n", sim->parameters->strategy);
	tableSession(sim->table, strcmp("mimic", sim->parameters->strategy) == 0);
	printf("    End: table session\n");

	sim->report.total_bet += sim->table->player->report.total_bet;
	sim->report.total_won += sim->table->player->report.total_won;
	sim->report.total_rounds += sim->table->report.total_rounds;
	sim->report.total_hands += sim->table->report.total_hands;
	sim->report.duration += sim->table->report.duration;
}

// Function to insert a simulation into the database (HTTP POST)
void simulatorInsert(Simulator *sim, SimulationDatabaseTable *sdt, const char *playbook) {
	struct curl_slist *headers = NULL;
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if (curl) {
		char url[MAX_BUFFER_SIZE];
		snprintf(url, MAX_BUFFER_SIZE, "http://%s/%s/%s/%s", getSimulationUrl(), sdt->simulator, playbook, sdt->guid);
    	printf("\n  -- insert ----------------------------------------------------------------------\n");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

		// Set headers
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		// Convert SimulationDatabaseTable to JSON
		cJSON *json = cJSON_CreateObject();
		cJSON_AddStringToObject(json, "playbook", sdt->playbook);
		cJSON_AddStringToObject(json, "guid", sdt->guid);
		cJSON_AddStringToObject(json, "simulator", sdt->simulator);
		cJSON_AddStringToObject(json, "summary", "no");
		cJSON_AddStringToObject(json, "simulations", "1");
		cJSON_AddStringToObject(json, "rounds", sdt->rounds);
		cJSON_AddStringToObject(json, "hands", sdt->hands);
		cJSON_AddStringToObject(json, "total_bet", sdt->total_bet);
		cJSON_AddStringToObject(json, "total_won", sdt->total_won);
		cJSON_AddStringToObject(json, "advantage", sdt->advantage);
		cJSON_AddStringToObject(json, "total_time", sdt->total_time);
		cJSON_AddStringToObject(json, "average_time", sdt->average_time);
		cJSON_AddStringToObject(json, "parameters", sdt->parameters);
		cJSON_AddStringToObject(json, "rules", sdt->rules);
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

