#include "report.h"

//
void initReport(Report* report) {
	report->total_rounds = 0;
	report->total_hands = 0;
	report->total_bet = 0;
	report->total_won = 0;
	report->start = 0;
	report->end = 0;
	report->duration = 0;
}

