#ifndef REPORT_H
#define REPORT_H

#include <stdint.h>  // Include this for int64_t
#include <time.h>
#include "parameters.h"
#include "rules.h"
#include "constants.h"

//
typedef struct {
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
	int64_t duration;
	float advantage;
	time_t start;
	time_t end;
} Report;

// Global functions
void initReport(Report *report, int number_of_threads);
void mergeReport(Report *a, Report *b);
void printReport(Report *report);
void insertReport(Report *report, Parameters *parameters, Rules *rules);

#endif // REPORT_H
