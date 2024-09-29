#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "table.h"
#include "player.h"
#include "logger.h"
#include "simulator.h"

//
char* serializeParameters(Parameters* parameters);

// Function to create a new simulation
Simulation* newSimulation(Parameters *parameters, Rules *rules) {
	Simulation *s = (Simulation*)malloc(sizeof(Simulation));
	time_t t = time(NULL);
	struct tm *tm_info = localtime(&t);

	snprintf(s->name, sizeof(s->name), "striker-c--%4d_%02d_%02d_%012ld", s->year, s->month, s->day, t);
	s->year = tm_info->tm_year + 1900;
	s->month = tm_info->tm_mon + 1;
	s->day = tm_info->tm_mday;

	s->table = newTable(parameters, rules);
	s->parameters = parameters;
	s->rules = rules;
	reportInit(&s->report);

	return s;
}

void simulationDelete(Simulation* sim) {
	free(sim);
}

// The SimulatorProcess function
void simulatorRunOnce(Simulation *s) {
	SimulationDatabaseTable tbs;

	Logger_simulation(s->parameters->logger, "  Starting striker-c simulation ...\n");
	simulatorRunSimulation(s);
	Logger_simulation(s->parameters->logger, "  Ending striker-c simulation ...\n\n");

	// Populate the rest of the SimulationDatabaseTable
	strcpy(tbs.playbook, s->parameters->playbook);
	strcpy(tbs.guid, s->parameters->guid);
	tbs.simulator = STRIKER_WHO_AM_I;
	tbs.summary = "no";
	tbs.simulations = "1";
	strcpy(tbs.parameters, serializeParameters(s->parameters));

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
	sprintf(tbs.advantage, "%+04.3f%%", ((double)s->report.total_won / s->report.total_bet) * 100);

	// Print out the results
    char buffer[256];

    Logger_simulation(s->parameters->logger, "\n  -- results ---------------------------------------------------------------------\n");
	sprintf(buffer, "    %-24s: %s\n", "Number of rounds", tbs.rounds);
	Logger_simulation(s->parameters->logger, buffer);
	sprintf(buffer, "    %-24s: %lld\n", "Number of hands", s->report.total_hands);
	Logger_simulation(s->parameters->logger, buffer);
	sprintf(buffer, "    %-24s: %lld %f average bet per hand\n", "Total bet", s->report.total_bet, (double)s->report.total_bet / s->report.total_hands);
	Logger_simulation(s->parameters->logger, buffer);
	sprintf(buffer, "    %-24s: %lld %f average win per hand\n", "Total won", s->report.total_won, (double)s->report.total_won / s->report.total_hands);
	Logger_simulation(s->parameters->logger, buffer);
	sprintf(buffer, "    %-24s: %s seconds\n", "Total time", tbs.total_time);
	Logger_simulation(s->parameters->logger, buffer);
	sprintf(buffer, "    %-24s: %s per 1,000,000 hands\n", "Average time", tbs.average_time);
	Logger_simulation(s->parameters->logger, buffer);
	sprintf(buffer, "    %-24s: %s\n", "Player advantage", tbs.advantage);
	Logger_simulation(s->parameters->logger, buffer);
    Logger_simulation(s->parameters->logger, "  --------------------------------------------------------------------------------\n");

	if(s->report.total_rounds >= getDatabaseMinimumRounds()) {
		simulatorInsert(&tbs, s->parameters->playbook, s->parameters->logger);
	}
}

// Function to run the simulation
void simulatorRunSimulation(Simulation *sim) {
    char buffer[256];

	sprintf(buffer, "    Starting %s table session ...\n", sim->parameters->strategy);
	Logger_simulation(sim->parameters->logger, buffer);
	tableSession(sim->table, strcmp("mimic", sim->parameters->strategy) == 0);
	sprintf(buffer, "    Ending %s table session ...\n", sim->parameters->strategy);
	Logger_simulation(sim->parameters->logger, buffer);

	sim->report.total_bet += sim->table->player->report.total_bet;
	sim->report.total_won += sim->table->player->report.total_won;
	sim->report.total_rounds += sim->table->report.total_rounds;
	sim->report.total_hands += sim->table->report.total_hands;
	sim->report.duration += sim->table->report.duration;
}

// Function to insert a simulation into the database (HTTP POST)
void simulatorInsert(SimulationDatabaseTable *sdt, const char *playbook, Logger *logger) {
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
		snprintf(url, sizeof(url), "http://%s/%s/%s/%s", getSimulationUrl(), sdt->simulator, playbook, sdt->guid);
		curl_easy_setopt(curl, CURLOPT_URL, url);

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
		cJSON_AddStringToObject(json, "payload", "n/a");
		// Add remaining fields...
		char *jsonStr = cJSON_Print(json);

		// Set POST fields
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr);

    	char buffer[2048];
		sprintf(buffer, "curl -X POST %s -H \"Content-Type: application/json\" -d %s\n\n", url, jsonStr);
		Logger_insert(logger, buffer);

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

