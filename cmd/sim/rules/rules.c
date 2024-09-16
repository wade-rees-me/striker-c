#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "rules.h"
#include "memory.h"
#include "constants.h"

//
bool rules_fetch_table(const char *url);

// Global variable for rules table
RulesTableStruct TableRules;

// Function to load table rules by calling FetchRulesTable
void rules_load_table(const char *decks) {
    char url[256];
    snprintf(url, sizeof(url), "%s/%s", get_rules_url(), decks);

    if (!rules_fetch_table(url)) {
        printf("Error fetching rules table.\n");
        exit(1);
    }
}

// Function to fetch rules table using libcurl
bool rules_fetch_table(const char *url) {
    CURL *curl_handle;
    CURLcode res;

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  // Will be grown as needed by realloc
    chunk.size = 0;            // No data at this point

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    if(curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl_handle);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return false;
        }

        // Process the JSON response
        cJSON *json = cJSON_Parse(chunk.memory);
        if(json == NULL) {
            printf("Error parsing JSON response\n");
            return false;
        }

        // Extract data from JSON into TableRules struct
        cJSON *playbook = cJSON_GetObjectItemCaseSensitive(json, "playbook");
        cJSON *hitSoft17 = cJSON_GetObjectItemCaseSensitive(json, "hitSoft17");
        cJSON *surrender = cJSON_GetObjectItemCaseSensitive(json, "surrender");
        cJSON *doubleAnyTwoCards = cJSON_GetObjectItemCaseSensitive(json, "doubleAnyTwoCards");
        cJSON *doubleAfterSplit = cJSON_GetObjectItemCaseSensitive(json, "doubleAfterSplit");
        cJSON *resplitAces = cJSON_GetObjectItemCaseSensitive(json, "resplitAces");
        cJSON *hitSplitAces = cJSON_GetObjectItemCaseSensitive(json, "hitSplitAces");

        if (playbook) strcpy(TableRules.playbook, playbook->valuestring);
        if (hitSoft17) TableRules.hit_soft_17 = hitSoft17->valueint;
        if (surrender) TableRules.surrender = surrender->valueint;
        if (doubleAnyTwoCards) TableRules.double_any_two_cards = doubleAnyTwoCards->valueint;
        if (doubleAfterSplit) TableRules.double_after_split = doubleAfterSplit->valueint;
        if (resplitAces) TableRules.resplit_aces = resplitAces->valueint;
        if (hitSplitAces) TableRules.hit_split_aces = hitSplitAces->valueint;

        // Cleanup
        cJSON_Delete(json);
        curl_easy_cleanup(curl_handle);
        free(chunk.memory);
        curl_global_cleanup();
    	return true;
    }
    return false;
}

