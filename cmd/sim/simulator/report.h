#ifndef REPORT_H
#define REPORT_H

#include <time.h>

typedef struct {
    int64_t total_rounds;
    int64_t total_hands;
    int64_t total_bet;
    int64_t total_won;
    time_t start;
    time_t end;
    int64_t duration;
} Report;

typedef struct {
    char guid[37];
    char* processor;
    char timestamp[50];
    const char* decks;
    const char* strategy;
    char playbook[124];
    char* blackjack_pays;
    int64_t tables;
    int64_t rounds;
    int number_of_decks;
    float penetration;
} Parameters;

void report_init(Report* report);

#endif // REPORT_H

