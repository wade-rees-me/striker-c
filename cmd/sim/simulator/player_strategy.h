#ifndef PLAYER_STRATEGY_H
#define PLAYER_STRATEGY_H

#include <stdbool.h>
#include "player.h"

// Struct to hold aux data
struct AuxData {
    int bet;
    int do_insurance;
    int do_double;
    int do_split;
    int do_surrender;
    int do_stand;
};

extern int player_get_bet(Player *p);
extern bool player_get_insurance(Player *p);
extern bool player_get_surrender(Player *p, const int *have, int up);
extern bool player_get_double(Player *p, const int *have, int up);
extern bool player_get_split(Player *p, int pair, int up);
extern bool player_get_stand(Player *p, const int *have, int up);
extern char* build_url(const char *baseUrl, const int *seenData, const int *haveData, int pair, const char *playbook, int cards, int up);
extern char* build_params(const int *seenData, const int *haveData, int pair, const char *playbook, int cards, int up);
extern bool httpGet2(const char *url);
extern bool httpGet(const char* url, const char* params);
extern char *url_encode(const char *str);
extern void removeSpaces(char *str);
extern bool parse_aux_data(const char *response);

#endif // PLAYER_STRATEGY_H
