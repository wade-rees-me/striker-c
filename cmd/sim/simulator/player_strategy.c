#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
#include "cjson/cJSON.h"
#include "player_strategy.h"
#include "memory.h"
#include "constants.h"

struct AuxData aux;

// Function to get bet value
int player_get_bet(Player *p) {
	char url[256];
	sprintf(url, "http://%s/%s", get_strategy_url(), "bet");
	char *params = build_params(p->seen_cards, NULL, 0, p->parameters->playbook, p->number_of_cards, 0);

	if (!httpGet(url, params)) {
		return MINIMUM_BET;
	}
	if (aux.bet < MINIMUM_BET) {
		return MINIMUM_BET;
	}
	if (aux.bet > MAXIMUM_BET) {
		return MAXIMUM_BET;
	}

	return aux.bet;
}

// Function to get insurance decision
bool player_get_insurance(Player *p) {
	char url[256];
	sprintf(url, "http://%s/%s", get_strategy_url(), "insurance");
	char *params = build_params(p->seen_cards, NULL, 0, p->parameters->playbook, p->number_of_cards, 0);

	if (!httpGet(url, params)) {
		return false;
	}

	return aux.do_insurance;
}

// Function to get surrender decision
bool player_get_surrender(Player *p, const int *have, int up) {
	char url[256];
	sprintf(url, "http://%s/%s", get_strategy_url(), "surrender");
	char *params = build_params(p->seen_cards, have, 0, p->parameters->playbook, p->number_of_cards, up);

	if (!httpGet(url, params)) {
		return false;
	}

	return aux.do_surrender;
}

bool player_get_double(Player *p, const int *have, int up) {
	char url[256];
	sprintf(url, "http://%s/%s", get_strategy_url(), "double");
	char *params = build_params(p->seen_cards, have, 0, p->parameters->playbook, p->number_of_cards, up);

	if (!httpGet(url, params)) {
		return false;
	}

	return aux.do_double;
}

bool player_get_split(Player *p, int pair, int up) {
	char url[256];
	sprintf(url, "http://%s/%s", get_strategy_url(), "split");
	char *params = build_params(p->seen_cards, NULL, pair, p->parameters->playbook, p->number_of_cards, up);

	if (!httpGet(url, params)) {
		return false;
	}

	return aux.do_split;
}

bool player_get_stand(Player *p, const int *have, int up) {
	char url[256];
	sprintf(url, "http://%s/%s", get_strategy_url(), "stand");
	char *params = build_params(p->seen_cards, have, 0, p->parameters->playbook, p->number_of_cards, up);

	if (!httpGet(url, params)) {
		return true;
	}

	return aux.do_stand;
}

// Function to build the URL
char* build_params(const int *seenData, const int *haveData, int pair, const char *playbook, int cards, int up) {
	char *params = malloc(512);
	sprintf(params, "playbook=%s&cards=%d&up=%d&pair=%d", playbook, cards, up, pair);

	if (seenData != NULL) {
		// Add seen data as JSON string
		cJSON *seenJson = cJSON_CreateIntArray(seenData, 13);
		char *seenString = cJSON_Print(seenJson);
		removeSpaces(seenString);
		strcat(params, "&seen=");
		strcat(params, seenString);
		free(seenString);
		cJSON_Delete(seenJson);
	}

	if (haveData != NULL) {
		// Add have data as JSON string
		cJSON *haveJson = cJSON_CreateIntArray(haveData, 13);
		char *haveString = cJSON_Print(haveJson);
		removeSpaces(haveString);
		strcat(params, "&have=");
		strcat(params, haveString);
		free(haveString);
		cJSON_Delete(haveJson);
	}

	return params;
}

bool parse_aux_tag(cJSON* json, const char* tag, bool missing) {
	cJSON *item = cJSON_GetObjectItem(json, tag);
	if (item != NULL) {
    	return cJSON_IsTrue(item);
	}
	return missing;
}

bool parse_aux_data(const char *response) {
	cJSON *json = cJSON_Parse(response);
	if (json == NULL) {
		printf("Failed to parse JSON\n");
		return false;
	}

	cJSON *betItem = cJSON_GetObjectItem(json, "bet");
	if (betItem != NULL) {
		aux.bet = cJSON_GetObjectItem(json, "bet")->valueint;
	}

	aux.do_insurance = parse_aux_tag(json, "insurance", false);
	aux.do_double = parse_aux_tag(json, "double", false);
	aux.do_split = parse_aux_tag(json, "split", false);
	aux.do_surrender = parse_aux_tag(json, "surrender", false);
	aux.do_stand = parse_aux_tag(json, "stand", true);

	cJSON_Delete(json);
	return true;
}

// Function to perform HTTP GET request
bool httpGet(const char* url, const char* params) {
	struct MemoryStruct chunk;
	CURL *curl_handle;
	CURLcode res;
	char fullUrl[256];
	sprintf(fullUrl, "%s?%s", url, params);

//printf("httpGet(%s)\n", fullUrl);
	chunk.memory = malloc(1);  // Will be grown as needed by realloc
	chunk.size = 0;			// No data at this point

	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();

	if (!curl_handle) {
		curl_global_cleanup();
		return false;
	}

	curl_easy_setopt(curl_handle, CURLOPT_URL, fullUrl);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);

	res = curl_easy_perform(curl_handle);

	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		return false;
	}
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();

	if (!parse_aux_data(chunk.memory)) {
		free(chunk.memory);
		return false;
	}
	free(chunk.memory);

	return true;
}

// Function to URL-encode a string
char *url_encode(const char *str) {
    const char *hex = "0123456789ABCDEF";  // Hexadecimal characters
    char *encoded = malloc(strlen(str) * 3 + 1);  // Allocate memory (worst case: every character is encoded)
    char *pencoded = encoded;

    if (encoded == NULL) {
        return NULL;  // Memory allocation failed
    }

    while (*str) {
        if (isalnum((unsigned char)*str) || *str == '-' || *str == '_' || *str == '.' || *str == '~' || *str == '=') {
            // Characters that don't need encoding
            *pencoded++ = *str;
        } else {
            // Characters that need encoding
            *pencoded++ = '%';
            *pencoded++ = hex[(*str >> 4) & 0xF];  // First hex digit
            *pencoded++ = hex[*str & 0xF];         // Second hex digit
        }
        str++;
    }

    *pencoded = '\0';  // Null-terminate the encoded string
    return encoded;
}

void removeSpaces(char *str) {
    int i = 0, j = 0;
    
    // Iterate through the string
    while (str[i]) {
        // If the character is not a space, copy it to the current `j` index
        if (str[i] != ' ') {
            str[j++] = str[i];
        }
        i++;
    }
    
    // Null-terminate the modified string
    str[j] = '\0';
}
