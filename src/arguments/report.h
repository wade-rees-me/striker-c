#ifndef REPORT_H
#define REPORT_H

#include <time.h>

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
	time_t start;
	time_t end;
	int64_t duration;
} Report;

// Global functions
void initReport(Report *report);

#endif // REPORT_H
