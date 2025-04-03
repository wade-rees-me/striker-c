#include "report.h"
#include <stdio.h>
#include <curl/curl.h>

//
void initReport(Report *report, int number_of_threads) {
	report->total_rounds = 0;
	report->total_hands = 0;
	report->total_bet = 0;
	report->total_won = 0;
	report->total_blackjacks = 0;
	report->total_doubles = 0;
	report->total_splits = 0;
	report->total_wins = 0;
	report->total_loses = 0;
	report->total_pushes = 0;
	report->total_threads = (int64_t)number_of_threads;
	report->start = 0;
	report->end = 0;
	report->duration = 0;
}

//
void mergeReport(Report *a, Report *b) {
    a->total_rounds += b->total_rounds;
    a->total_hands += b->total_hands;
    a->total_bet += b->total_bet;
    a->total_won += b->total_won;
    a->total_blackjacks += b->total_blackjacks;
    a->total_doubles += b->total_doubles;
    a->total_splits += b->total_splits;
    a->total_wins += b->total_wins;
    a->total_loses += b->total_loses;
    a->total_pushes += b->total_pushes;
}

// Print out the results
void printReport(Report *report) {
	char buffer[MAX_BUFFER_SIZE];
	char average[MAX_BUFFER_SIZE];
	char hands[MAX_BUFFER_SIZE];

	report->advantage = ((double)report->total_won / report->total_bet) * 100;

	printf("\n  -- results ---------------------------------------------------------------------\n");
	printf("    %-26s: %17s\n", "Number of hands", convertToStringWithCommas(report->total_hands, buffer, MAX_BUFFER_SIZE));
	printf("    %-26s: %17s\n", "Number of rounds", convertToStringWithCommas(report->total_rounds, buffer, MAX_BUFFER_SIZE));
	printf("    %-26s: %17s %+08.3f average bet per hand\n", "Total bet",
			convertToStringWithCommas(report->total_bet, buffer, MAX_BUFFER_SIZE), (double)report->total_bet / report->total_hands);
	printf("    %-26s: %17s %+08.3f average win per hand\n", "Total won",
			convertToStringWithCommas(report->total_won, buffer, MAX_BUFFER_SIZE), (double)report->total_won / report->total_hands);
	printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of blackjacks",
			convertToStringWithCommas(report->total_blackjacks, buffer, MAX_BUFFER_SIZE), (double)report->total_blackjacks / report->total_hands * 100.0);
	printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of doubles",
			convertToStringWithCommas(report->total_doubles, buffer, MAX_BUFFER_SIZE), (double)report->total_doubles / report->total_hands * 100.0);
	printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of splits",
			convertToStringWithCommas(report->total_splits, buffer, MAX_BUFFER_SIZE), (double)report->total_splits / report->total_hands * 100.0);
	printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of wins",
			convertToStringWithCommas(report->total_wins, buffer, MAX_BUFFER_SIZE), (double)report->total_wins / report->total_hands * 100.0);
	printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of pushes",
			convertToStringWithCommas(report->total_pushes, buffer, MAX_BUFFER_SIZE), (double)report->total_pushes / report->total_hands * 100.0);
	printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of loses",
			convertToStringWithCommas(report->total_loses, buffer, MAX_BUFFER_SIZE), (double)report->total_loses / report->total_hands * 100.0);
	printf("    %-26s: %17s seconds\n", "Total time", convertToStringWithCommas(report->duration, buffer, MAX_BUFFER_SIZE));
	printf("    %-26s: %17ld threads\n", "Number of threads", report->total_threads);
	convertToStringWithCommas((int)((float)report->duration * (float)BILLION / (float)report->total_hands), average, MAX_BUFFER_SIZE);
	convertToStringWithCommas((float)BILLION, hands, MAX_BUFFER_SIZE);
	printf("    %-26s: %17s seconds per %s hands\n", "Average time", average, hands);
	printf("    %-26s: %17s %+08.3f %%\n", "Player advantage", "", report->advantage);
	printf("  --------------------------------------------------------------------------------\n");
}

// Function to insert a simulation into the database (HTTP POST)
void insertReport(Report *report, Parameters *parameters, Rules *rules) {
	struct curl_slist *headers = NULL;
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if (curl) {
		char url[MAX_BUFFER_SIZE];
		char tmp[MAX_BUFFER_SIZE];
		snprintf(url, MAX_BUFFER_SIZE, "http://%s/%s/%s/%s", getSimulationUrl(), parameters->processor, parameters->playbook, parameters->name);
		printf("\n  -- insert ----------------------------------------------------------------------\n");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

		// Set headers
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		// Convert Simulation to JSON
		cJSON *json = cJSON_CreateObject();
		cJSON_AddStringToObject(json, "guid", parameters->name);
		cJSON_AddStringToObject(json, "version", STRIKER_VERSION);
		cJSON_AddStringToObject(json, "simulator", parameters->processor);
		snprintf(tmp, MAX_BUFFER_SIZE, "%d", parameters->number_of_threads);
		cJSON_AddStringToObject(json, "threads", tmp);
		cJSON_AddStringToObject(json, "playbook", parameters->playbook);
		cJSON_AddStringToObject(json, "decks", parameters->decks);
		cJSON_AddStringToObject(json, "strategy", parameters->strategy);
		snprintf(tmp, MAX_BUFFER_SIZE, "%ld", report->total_rounds);
		cJSON_AddStringToObject(json, "rounds", tmp);
		snprintf(tmp, MAX_BUFFER_SIZE, "%ld", report->total_hands);
		cJSON_AddStringToObject(json, "hands", tmp);
		snprintf(tmp, MAX_BUFFER_SIZE, "%ld", report->total_bet);
		cJSON_AddStringToObject(json, "total_bet", tmp);
		snprintf(tmp, MAX_BUFFER_SIZE, "%ld", report->total_won);
		cJSON_AddStringToObject(json, "total_won", tmp);
		snprintf(tmp, MAX_BUFFER_SIZE, "%f", report->advantage);
		cJSON_AddStringToObject(json, "advantage", tmp);
		snprintf(tmp, MAX_BUFFER_SIZE, "%ld", report->start);
		cJSON_AddStringToObject(json, "start", tmp);
		snprintf(tmp, MAX_BUFFER_SIZE, "%ld", report->end);
		cJSON_AddStringToObject(json, "end", tmp);
		snprintf(tmp, MAX_BUFFER_SIZE, "%ld", report->duration);
		cJSON_AddStringToObject(json, "duration", tmp);
		serializeRules(rules, tmp, MAX_BUFFER_SIZE);
		cJSON_AddStringToObject(json, "rules", tmp);
		// Add remaining fields...
		char *jsonStr = cJSON_Print(json);

		// Set POST fields
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr);
		//printf("\n\ncurl -X POST %s -H \"Content-Type: application/json\" -d %s\n\n", url, jsonStr);

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

