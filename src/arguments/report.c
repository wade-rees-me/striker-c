#define _GNU_SOURCE // Enable GNU extensions (needed on some systems)

#include "report.h"
#include "constants.h"
#include <curl/curl.h>
#include <stdio.h>

//
cJSON *toJsonObject(Report *report);

//
void initReportFinal(Report *report, Parameters *parameters) {
    initReport(report);
    report->name = parameters->name;
    report->version = STRIKER_VERSION;
    report->playbook = parameters->playbook;
    report->simulator = parameters->processor;
    report->strategy = parameters->strategy;
    report->decks = parameters->decks;
    report->epoch = parameters->epoch;
    report->total_threads = parameters->number_of_threads;
    report->start = time(NULL);
    report->end = 0;
    report->duration = 0;
    report->advantage = 0.0;
    report->per_billion = 0.0;
}

//
void initReport(Report *report) {
    report->total_rounds = 0;
    report->total_hands = 0;
    report->total_bet = 0;
    report->total_won = 0;
    report->total_blackjacks = 0;
    report->total_doubles = 0;
    report->total_splits = 0;
    report->total_splits_ace = 0;
    report->total_wins = 0;
    report->total_loses = 0;
    report->total_pushes = 0;
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
    a->total_splits_ace += b->total_splits_ace;
    a->total_wins += b->total_wins;
    a->total_loses += b->total_loses;
    a->total_pushes += b->total_pushes;
}

//
void finishReport(Report *report) {
    report->end = time(NULL);
    report->duration = report->end - report->start;
    report->advantage = ((double)report->total_won / report->total_bet) * 100;
    report->per_billion = ((float)report->duration * (float)BILLION / (float)report->total_hands);
}

// Print out the results
void printReport(Report *report) {
    char buffer[MAX_BUFFER_SIZE];
    char hands[MAX_BUFFER_SIZE];

    printf("    %-26s: %17s\n", "Number of hands",
           convertToStringWithCommas(report->total_hands, buffer, MAX_BUFFER_SIZE));
    printf("    %-26s: %17s\n", "Number of rounds",
           convertToStringWithCommas(report->total_rounds, buffer, MAX_BUFFER_SIZE));
    printf("    %-26s: %17s %+08.3f average bet per hand\n", "Total bet",
           convertToStringWithCommas(report->total_bet, buffer, MAX_BUFFER_SIZE),
           (double)report->total_bet / report->total_hands);
    printf("    %-26s: %17s %+08.3f average win per hand\n", "Total won",
           convertToStringWithCommas(report->total_won, buffer, MAX_BUFFER_SIZE),
           (double)report->total_won / report->total_hands);
    printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of blackjacks",
           convertToStringWithCommas(report->total_blackjacks, buffer, MAX_BUFFER_SIZE),
           (double)report->total_blackjacks / report->total_hands * 100.0);
    printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of doubles",
           convertToStringWithCommas(report->total_doubles, buffer, MAX_BUFFER_SIZE),
           (double)report->total_doubles / report->total_hands * 100.0);
    printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of splits",
           convertToStringWithCommas(report->total_splits, buffer, MAX_BUFFER_SIZE),
           (double)report->total_splits / report->total_hands * 100.0);
    printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of splits - Aces",
           convertToStringWithCommas(report->total_splits_ace, buffer, MAX_BUFFER_SIZE),
           (double)report->total_splits_ace / report->total_hands * 100.0);
    printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of wins",
           convertToStringWithCommas(report->total_wins, buffer, MAX_BUFFER_SIZE),
           (double)report->total_wins / report->total_hands * 100.0);
    printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of pushes",
           convertToStringWithCommas(report->total_pushes, buffer, MAX_BUFFER_SIZE),
           (double)report->total_pushes / report->total_hands * 100.0);
    printf("    %-26s: %17s %+08.3f %% of total hands\n", "Number of loses",
           convertToStringWithCommas(report->total_loses, buffer, MAX_BUFFER_SIZE),
           (double)report->total_loses / report->total_hands * 100.0);
    printf("    %-26s: %17s seconds\n", "Total time",
           convertToStringWithCommas(report->duration, buffer, MAX_BUFFER_SIZE));
    printf("    %-26s: %17ld threads\n", "Number of threads", report->total_threads);
    convertToStringWithCommas((float)BILLION, hands, MAX_BUFFER_SIZE);
    printf("    %-26s: %17s seconds per %s hands\n", "Average time",
           convertToStringWithCommas((int)report->per_billion, buffer, MAX_BUFFER_SIZE), hands);
    printf("    %-26s: %17s %+08.3f %%\n", "Player advantage", "", report->advantage);
}

// Function to insert a simulation into the database (HTTP POST)
void insertReport(Report *report) {
    if (!is_my_computer()) {
        printf("    This code is restricted to running only on my computer.\n");
        return;
    }

    if (report->total_hands < NUMBER_OF_HANDS_DATABASE) {
        char hands[MAX_BUFFER_SIZE];
        char minimum[MAX_BUFFER_SIZE];
        convertToStringWithCommas(report->total_hands, hands, MAX_BUFFER_SIZE);
        convertToStringWithCommas(NUMBER_OF_HANDS_DATABASE, minimum, MAX_BUFFER_SIZE);
        printf("    Error: Not enough hands played (%s). Minimum required is %s\n", hands, minimum);
        return;
    }

    struct curl_slist *headers = NULL;
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (!curl) {
        printf("    Curl failed to generate\n");
        return;
    }

    char *url = NULL;
    if (asprintf(&url, "http://%s/%s/%s/%s", getSimulationUrl(), report->simulator, report->playbook, report->name) ==
        -1) {
        printf("    URL failed to generate\n");
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fopen("/dev/null", "w"));
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, fopen("/dev/null", "w"));

    // Set headers
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Convert Simulation to JSON
    cJSON *json = toJsonObject(report);
    char *jsonStr = cJSON_Print(json);

    // Set POST fields
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr);

    // Perform the request
    res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        printf("    Insert successful\n");
    } else {
        printf("    curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    // Cleanup
    curl_easy_cleanup(curl);
    cJSON_Delete(json);
    free(jsonStr);
    curl_global_cleanup();
}

// Convert Simulation to JSON
cJSON *toJsonObject(Report *report) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "guid", report->name);
    cJSON_AddStringToObject(json, "version", report->version);
    cJSON_AddStringToObject(json, "simulator", report->simulator);
    cJSON_AddNumberToObject(json, "threads", report->total_threads);
    cJSON_AddStringToObject(json, "playbook", report->playbook);
    cJSON_AddStringToObject(json, "decks", report->decks);
    cJSON_AddStringToObject(json, "strategy", report->strategy);
    cJSON_AddNumberToObject(json, "rounds", report->total_rounds);
    cJSON_AddNumberToObject(json, "hands", report->total_hands);
    cJSON_AddNumberToObject(json, "total_bet", report->total_bet);
    cJSON_AddNumberToObject(json, "total_won", report->total_won);
    cJSON_AddNumberToObject(json, "total_blackjacks", report->total_blackjacks);
    cJSON_AddNumberToObject(json, "total_doubles", report->total_doubles);
    cJSON_AddNumberToObject(json, "total_splits", report->total_splits);
    cJSON_AddNumberToObject(json, "total_splits_aces", report->total_splits_ace);
    cJSON_AddNumberToObject(json, "total_wins", report->total_wins);
    cJSON_AddNumberToObject(json, "total_loses", report->total_loses);
    cJSON_AddNumberToObject(json, "total_pushes", report->total_pushes);
    cJSON_AddNumberToObject(json, "advantage", report->advantage);
    cJSON_AddStringToObject(json, "epoch", report->epoch);
    cJSON_AddNumberToObject(json, "start", report->start);
    cJSON_AddNumberToObject(json, "end", report->end);
    cJSON_AddNumberToObject(json, "duration", report->duration);
    cJSON_AddNumberToObject(json, "per_billion", report->per_billion);
    return json;
}

