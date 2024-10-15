#ifndef REPORT_H
#define REPORT_H

#include <time.h>

//
typedef struct {
	int64_t total_rounds;
	int64_t total_hands;
	int64_t total_bet;
	int64_t total_won;
	time_t start;
	time_t end;
	int64_t duration;
} Report;

// Global functions
void initReport(Report* report);

#endif // REPORT_H
