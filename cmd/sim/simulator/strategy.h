#ifndef PLAYER_STRATEGY_H
#define PLAYER_STRATEGY_H

#include <stdbool.h>
#include "player.h"
#include "shoe.h"

// Struct to hold aux data
struct AuxData {
	int bet;
	bool do_insurance;
	bool do_double;
	bool do_split;
	bool do_surrender;
	bool do_stand;
};

void initStrategy();

int playerGetBet(Player *p);
bool playerGetInsurance(Player *p);
bool playerGetSurrender(Player *p, const int *have, Card* up);
bool playerGetDouble(Player *p, const int *have, Card* up);
bool playerGetSplit(Player *p, Card* pair, Card* up);
bool playerGetStand(Player *p, const int *have, Card* up);
void playerGetPlay(Player *p, const int *have, Card* pair, Card* up);
void playerClear(Player *p);
char* buildUrl(const char *baseUrl, const int *seenData, const int *haveData, Card* pair, const char *playbook, int cards, Card* up);
char* buildParams(const int *seenData, const int *haveData, Card* pair, const char *playbook, int cards, Card* up);
char *urlEncode(const char *str);
void removeSpaces(char *str);
void parseAuxData(const char *response);

#endif // PLAYER_STRATEGY_H
