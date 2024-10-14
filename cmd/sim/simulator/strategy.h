#ifndef PLAYER_STRATEGY_H
#define PLAYER_STRATEGY_H

#include <stdbool.h>
#include "player.h"
#include "shoe.h"

//
#define BET "bet"
#define INSURANCE "insurance"
#define SURRENDER "surrender"
#define DOUBLE "double"
#define SPLIT "split"
#define STAND "stand"
#define PLAY "play"

// Global functions
void initStrategy();
int playerGetBet(Player *p);
bool playerGetInsurance(Player *p);
bool playerGetSurrender(Player *p, const int *have, Card* up);
bool playerGetDouble(Player *p, const int *have, Card* up);
bool playerGetSplit(Player *p, Card* pair, Card* up);
bool playerGetStand(Player *p, const int *have, Card* up);
void playerGetPlay(Player *p, const int *have, Card* pair, Card* up);
void playerClear(Player *p);

#endif // PLAYER_STRATEGY_H
