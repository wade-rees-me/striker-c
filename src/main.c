#include "arguments.h"
#include "constants.h"
#include "parameters.h"
#include "rules.h"
#include "simulator.h"
#include "strategy.h"
#include "xlog/xlog.h"
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//
int main(int argc, char *argv[]) {
    xlog_init(SYSLOG_ADDRESS, SYSLOG_PORT);
    Arguments *arguments = newArguments(argc, argv);
    Parameters *parameters = newParameters(arguments);
    Rules *rules = newRules(getDecks(arguments));
    Strategy *strategy = newStrategy(getDecks(arguments), getStrategy(arguments), getNumberOfDecks(arguments));
    Simulator *simulators[NUMBER_OF_CORES_LOGICAL];
    pthread_t threads[NUMBER_OF_CORES_LOGICAL];
    Report finalReport;

    time_t start = xlog_start("Simulation started, strategy=%s, decks=%s, hands=%ld", getStrategy(arguments),
                              getDecks(arguments), arguments->number_of_hands);
    setvbuf(stdout, NULL, _IONBF, 0); // Unbuffered
    printf("Start: %s\n", STRIKER_WHO_AM_I);
    printf("  -- arguments -------------------------------------------------------------------\n");
    printParameters(parameters);
    printRules(rules);
    printf("  --------------------------------------------------------------------------------\n");

    initReportFinal(&finalReport, parameters);
    for (int i = 0; i < arguments->number_of_threads; i++) {
        simulators[i] = newSimulator(parameters, rules, strategy, NUMBER_OF_CORES_LOGICAL - 1 - i);
        pthread_create(&threads[i], NULL, simulatorRunOnce, (void *)simulators[i]);
    }
    for (int i = 0; i < arguments->number_of_threads; i++) {
        void *ret;
        pthread_join(threads[i], &ret);
    }
    for (int i = 0; i < arguments->number_of_threads; i++) { // Merge all reports after threads finish
        mergeReport(&finalReport, getReport(simulators[i]));
        simulatorDelete(simulators[i]);
    }

    finishReport(&finalReport);
    printf("  -- results ---------------------------------------------------------------------\n");
    printReport(&finalReport);
    printf("  --------------------------------------------------------------------------------\n");
    printf("  -- insert ----------------------------------------------------------------------\n");
    insertReport(&finalReport);
    printf("  --------------------------------------------------------------------------------\n");
    xlog_stop(start, "Simulation completed: %s", getStrategy(arguments));

    rulesDelete(rules);
    parametersDelete(parameters);
    argumentsDelete(arguments);

    xlog_close();
    return 0;
}

