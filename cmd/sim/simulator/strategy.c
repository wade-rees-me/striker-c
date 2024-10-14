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

//
char* buildUrl(const char *baseUrl, const int *seenData, const int *haveData, Card* pair, const char *playbook, int cards, Card* up);
char* buildParams(char* params, const int *seenData, const int *haveData, Card* pair, const char *playbook, int cards, Card* up);
void parseAuxData(struct PlayData* aux, const char *response);
void httpGet(const char* url, struct PlayData* aux, const char* params);

//
static char url[MAX_BUFFER_SIZE];
static char urlBet[MAX_BUFFER_SIZE];
static char urlInsurance[MAX_BUFFER_SIZE];
static char urlSurrender[MAX_BUFFER_SIZE];
static char urlDouble[MAX_BUFFER_SIZE];
static char urlSplit[MAX_BUFFER_SIZE];
static char urlStand[MAX_BUFFER_SIZE];
static char urlPlay[MAX_BUFFER_SIZE];
static CURL *curl_handle = NULL;

//
void initStrategy() {
	snprintf(url, MAX_BUFFER_SIZE, "http://%s", getStrategyUrl());
	snprintf(urlBet, MAX_BUFFER_SIZE, "%s/%s", url, BET);
	snprintf(urlInsurance, MAX_BUFFER_SIZE, "%s/%s", url, INSURANCE);
	snprintf(urlSurrender, MAX_BUFFER_SIZE, "%s/%s", url, SURRENDER);
	snprintf(urlDouble, MAX_BUFFER_SIZE, "%s/%s", url, DOUBLE);
	snprintf(urlSplit, MAX_BUFFER_SIZE, "%s/%s", url, SPLIT);
	snprintf(urlStand, MAX_BUFFER_SIZE, "%s/%s", url, STAND);
	snprintf(urlPlay, MAX_BUFFER_SIZE, "%s/%s", url, PLAY);
}

//
int playerGetBet(Player *p) {
	char params[MAX_MEMORY_SIZE];
	httpGet(urlBet, &p->play, buildParams(params, p->seen_cards, NULL, 0, p->parameters->playbook, p->number_of_cards, 0));
	return p->play.bet;
}

//
bool playerGetInsurance(Player *p) {
	char params[MAX_MEMORY_SIZE];
	httpGet(urlInsurance, &p->play, buildParams(params, p->seen_cards, NULL, NULL, p->parameters->playbook, p->number_of_cards, NULL));
	return p->play.do_insurance;
}

//
bool playerGetSurrender(Player *p, const int *have, Card* up) {
	if(!p->play.do_play) {
		char params[MAX_MEMORY_SIZE];
		httpGet(urlSurrender, &p->play, buildParams(params, p->seen_cards, have, NULL, p->parameters->playbook, p->number_of_cards, up));
	}
	return p->play.do_surrender;
}

//
bool playerGetDouble(Player *p, const int *have, Card* up) {
	if(!p->play.do_play) {
		char params[MAX_MEMORY_SIZE];
		httpGet(urlDouble, &p->play, buildParams(params, p->seen_cards, have, NULL, p->parameters->playbook, p->number_of_cards, up));
	}
	return p->play.do_double;
}

//
bool playerGetSplit(Player *p, Card* pair, Card* up) {
	if(!p->play.do_play) {
		char params[MAX_MEMORY_SIZE];
		httpGet(urlSplit, &p->play, buildParams(params, p->seen_cards, NULL, pair, p->parameters->playbook, p->number_of_cards, up));
	}
	return p->play.do_split;
}

//
bool playerGetStand(Player *p, const int *have, Card* up) {
	if(!p->play.do_play) {
		char params[MAX_MEMORY_SIZE];
		httpGet(urlStand, &p->play, buildParams(params, p->seen_cards, have, NULL, p->parameters->playbook, p->number_of_cards, up));
	}
	return p->play.do_stand;
}

//
void playerGetPlay(Player *p, const int *have, Card* pair, Card* up) {
	char params[MAX_MEMORY_SIZE];
	p->play.do_play = true;
	httpGet(urlStand, &p->play, buildParams(params, p->seen_cards, have, pair, p->parameters->playbook, p->number_of_cards, up));
}

//
void playerClear(Player *p) {
	p->play.do_play = false;
}

//
char* buildParams(char* params, const int *seenData, const int *haveData, Card* pair, const char *playbook, int cards, Card* up) {
	char tmp[MAX_BUFFER_SIZE];

	memset(params, '\0', MAX_MEMORY_SIZE);
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
void parseAuxData(struct PlayData* aux, const char *response) {
	cJSON *json = cJSON_Parse(response);
	if (json == NULL) {
		printf("Failed to parse JSON\n");
		exit(1);
	}

	cJSON *betItem = cJSON_GetObjectItem(json, BET);
	if (betItem != NULL) {
		aux->bet = cJSON_GetObjectItem(json, BET)->valueint;
	}

	aux->do_insurance = parseAuxBool(json, INSURANCE, false);
	aux->do_double = parseAuxBool(json, DOUBLE, false);
	aux->do_split = parseAuxBool(json, SPLIT, false);
	aux->do_surrender = parseAuxBool(json, SURRENDER, false);
	aux->do_stand = parseAuxBool(json, STAND, true);

	cJSON_Delete(json);
}

// Function to perform HTTP GET request
void httpGet(const char* url, struct PlayData* aux, const char* params) {
	CURLcode res;
	char fullUrl[MAX_BUFFER_SIZE];
    long http_code = 0;

	struct MemoryStruct chunk;
	chunk.size = 0;

	sprintf(fullUrl, "%s?%s", url, params);
//printf("httpGet(%s)\n", fullUrl);

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
		fprintf(stderr, "  httpGet(%s)\n", fullUrl);
		exit(1);
	}

    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200) {
        fprintf(stderr, "HTTP Status Code: %ld\n", http_code);
		fprintf(stderr, "  httpGet(%s)\n", fullUrl);
		exit(1);
    }
	parseAuxData(aux, chunk.memory);
}

