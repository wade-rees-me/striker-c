#ifndef REPORT_H
#define REPORT_H

#include "constants.h"
#include "parameters.h"
#include "rules.h"
#include <stdint.h>
#include <time.h>

//
typedef struct {
    char name[MAX_STRING_SIZE];
    char version[MAX_STRING_SIZE];
    char playbook[MAX_STRING_SIZE];
    char simulator[MAX_STRING_SIZE];
    char strategy[MAX_STRING_SIZE];
    char decks[MAX_STRING_SIZE];
    char epoch[MAX_STRING_SIZE];
    int64_t total_rounds;
    int64_t total_hands;
    int64_t total_bet;
    int64_t total_won;
    int64_t total_blackjacks;
    int64_t total_doubles;
    int64_t total_splits;
    int64_t total_wins;
    int64_t total_loses;
    int64_t total_pushes;
    int64_t total_threads;
    int64_t start;
    int64_t end;
    int64_t duration;
    float advantage;
    float per_billion;
} Report;

// Global functions
void initReportFinal(Report *report, Parameters *parameters, int64_t start);
void initReport(Report *report);
void mergeReport(Report *a, Report *b);
void finishReport(Report *report, int64_t end);
void printReport(Report *report);
void insertReport(Report *report);

#endif // REPORT_H
