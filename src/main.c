#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include "arguments.h"
#include "parameters.h"
#include "rules.h"
#include "strategy.h"
#include "simulator.h"
#include "constants.h"

//
int main(int argc, char *argv[]) {
	Arguments *arguments = newArguments(argc, argv);
	Parameters *parameters = newParameters(arguments);
	Rules *rules = newRules(getDecks(arguments));
	Strategy *strategy = newStrategy(getDecks(arguments), getStrategy(arguments), getNumberOfDecks(arguments));
	Simulator* simulators[NUMBER_OF_CORES_LOGICAL];
   	pthread_t threads[NUMBER_OF_CORES_LOGICAL];
	Report finalReport;

	printf("Start: %s\n", STRIKER_WHO_AM_I);
	printf("  -- arguments -------------------------------------------------------------------\n");
	printParameters(parameters);
	printRules(rules);
	printf("  --------------------------------------------------------------------------------\n");
	printf("  Start: simulation(%s) on %d logical cores\n", parameters->name, arguments->number_of_threads);

	initReport(&finalReport, arguments->number_of_threads);
	finalReport.start = time(NULL);

   	for (int i = 0; i < arguments->number_of_threads; i++) {
		simulators[i] = newSimulator(parameters, rules, strategy, NUMBER_OF_CORES_LOGICAL - 1 - i);
		pthread_create(&threads[i], NULL, simulatorRunOnce, (void*)simulators[i]);
   	}
	for (int i = 0; i < arguments->number_of_threads; i++) {
		void* ret;
		pthread_join(threads[i], &ret);
	}
	for (int i = 0; i < arguments->number_of_threads; i++) { // Merge all reports after threads finish
   		mergeReport(&finalReport, getReport(simulators[i]));
		simulatorDelete(simulators[i]);
	}
	printf("  End: simulation\n");
	printf("End: %s\n", STRIKER_WHO_AM_I);

	finalReport.end = time(NULL);
	finalReport.duration = finalReport.end - finalReport.start;
	printReport(&finalReport);

	if(finalReport.total_hands >= NUMBER_OF_HANDS_DATABASE) {
		insertReport(&finalReport, parameters, rules);
	}

	rulesDelete(rules);
	parametersDelete(parameters);
	argumentsDelete(arguments);

	return 0;
}

