#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
#include "cjson/cJSON.h"
#include "strategy.h"
#include "memory.h"
#include "constants.h"

void httpGet(const char* url, const char* params);

struct AuxData aux;

static char* urlBet;
static char* urlInsurance;
static char* urlSurrender;
static char* urlDouble;
static char* urlSplit;
static char* urlStand;
static char* urlPlay;
static CURL *curl_handle = NULL;

//
void initStrategy() {
	urlBet = malloc(256);
	sprintf(urlBet, "http://%s/%s", getStrategyUrl(), BET);

	urlInsurance = malloc(256);
	sprintf(urlInsurance, "http://%s/%s", getStrategyUrl(), INSURANCE);

	urlSurrender = malloc(256);
	sprintf(urlSurrender, "http://%s/%s", getStrategyUrl(), SURRENDER);

	urlDouble = malloc(256);
	sprintf(urlDouble, "http://%s/%s", getStrategyUrl(), DOUBLE);

	urlSplit = malloc(256);
	sprintf(urlSplit, "http://%s/%s", getStrategyUrl(), SPLIT);

	urlStand = malloc(256);
	sprintf(urlStand, "http://%s/%s", getStrategyUrl(), STAND);

	urlPlay = malloc(256);
	sprintf(urlPlay, "http://%s/%s", getStrategyUrl(), PLAY);
}

// Function to get bet value
int playerGetBet(Player *p) {
	httpGet(urlBet, buildParams(p->seen_cards, NULL, 0, p->parameters->playbook, p->number_of_cards, 0));
	return aux.bet;
}

// Function to get insurance decision
bool playerGetInsurance(Player *p) {
	httpGet(urlInsurance, buildParams(p->seen_cards, NULL, NULL, p->parameters->playbook, p->number_of_cards, NULL));
	return aux.do_insurance;
}

// Function to get surrender decision
bool playerGetSurrender(Player *p, const int *have, Card* up) {
	if(!p->do_play) {
		httpGet(urlSurrender, buildParams(p->seen_cards, have, NULL, p->parameters->playbook, p->number_of_cards, up));
	}
	return aux.do_surrender;
}

bool playerGetDouble(Player *p, const int *have, Card* up) {
	if(!p->do_play) {
		httpGet(urlDouble, buildParams(p->seen_cards, have, NULL, p->parameters->playbook, p->number_of_cards, up));
	}
	return aux.do_double;
}

bool playerGetSplit(Player *p, Card* pair, Card* up) {
	if(!p->do_play) {
		httpGet(urlSplit, buildParams(p->seen_cards, NULL, pair, p->parameters->playbook, p->number_of_cards, up));
	}
	return aux.do_split;
}

bool playerGetStand(Player *p, const int *have, Card* up) {
	if(!p->do_play) {
		httpGet(urlStand, buildParams(p->seen_cards, have, NULL, p->parameters->playbook, p->number_of_cards, up));
	}
	return aux.do_stand;
}

void playerGetPlay(Player *p, const int *have, Card* pair, Card* up) {
	p->do_play = true;
	httpGet(urlStand, buildParams(p->seen_cards, have, pair, p->parameters->playbook, p->number_of_cards, up));
}

void playerClear(Player *p) {
	p->do_play = false;
}

// Function to build the URL
char* buildParams(const int *seenData, const int *haveData, Card* pair, const char *playbook, int cards, Card* up) {
	char* params = malloc(512);
	char tmp[128];

	strcat(params, "playbook=");
	strcat(params, playbook);
	strcat(params, "&cards=");
	sprintf(tmp, "%d", cards);
	strcat(params, tmp);
	if(up != NULL) {
		strcat(params, "&up=");
		sprintf(tmp, "%d", up->offset);
		strcat(params, tmp);
	}
	if(pair != NULL) {
		strcat(params, "&pair=");
		sprintf(tmp, "%d", pair->value);
		strcat(params, tmp);
	}

	if (seenData != NULL) {
		cJSON *seenJson = cJSON_CreateIntArray(seenData, 13);
		char *seenString = cJSON_Print(seenJson);
		removeSpaces(seenString);
		strcat(params, "&seen=");
		strcat(params, seenString);
		free(seenString);
		cJSON_Delete(seenJson);
	}

	if (haveData != NULL) {
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

//
bool parseAuxTag(cJSON* json, const char* tag, bool missing) {
	cJSON *item = cJSON_GetObjectItem(json, tag);
	if (item != NULL) {
		return cJSON_IsTrue(item);
	}
	return missing;
}

//
void parseAuxData(const char *response) {
	cJSON *json = cJSON_Parse(response);
	if (json == NULL) {
		printf("Failed to parse JSON\n");
		exit(1);
	}

	cJSON *betItem = cJSON_GetObjectItem(json, BET);
	if (betItem != NULL) {
		aux.bet = cJSON_GetObjectItem(json, BET)->valueint;
	}

	aux.do_insurance = parseAuxTag(json, INSURANCE, false);
	aux.do_double = parseAuxTag(json, DOUBLE, false);
	aux.do_split = parseAuxTag(json, SPLIT, false);
	aux.do_surrender = parseAuxTag(json, SURRENDER, false);
	aux.do_stand = parseAuxTag(json, STAND, true);

	cJSON_Delete(json);
}

// Function to perform HTTP GET request
void httpGet(const char* url, const char* params) {
	struct MemoryStruct chunk;
	CURLcode res;
	char fullUrl[256];
    long http_code = 0;

	sprintf(fullUrl, "%s?%s", url, params);

//printf("httpGet(%s)\n", fullUrl);
	chunk.memory = malloc(1);  // Will be grown as needed by realloc
	chunk.size = 0;			// No data at this point

	curl_global_init(CURL_GLOBAL_ALL);
	if(!curl_handle) {
		curl_handle = curl_easy_init();
		if (!curl_handle) {
			curl_global_cleanup();
			exit(1);
		}
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
	}

	curl_easy_setopt(curl_handle, CURLOPT_URL, fullUrl);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);

	res = curl_easy_perform(curl_handle);
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		exit(1);
	}

    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200) {
        fprintf(stderr, "HTTP Status Code: %ld\n", http_code);
		exit(1);
    }

//curl_easy_cleanup(curl_handle);
//curl_global_cleanup();

	parseAuxData(chunk.memory);
	free(chunk.memory);
}

// Function to URL-encode a string
char *urlEncode(const char *str) {
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
			*pencoded++ = hex[*str & 0xF];		 // Second hex digit
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
