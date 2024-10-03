#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arguments.h"
#include "logger.h"
#include "rules.h"
#include "parameters.h"
#include "simulator.h"
#include "strategy.h"
#include "constants.h"

void generateName(char* buffer, size_t buffer_size);

//
int main(int argc, char* argv[]) {
    char name[256];
    char buffer[256];
    generateName(name, sizeof(name));
	Arguments arguments;
	parseArguments(&arguments, argc, argv);

	Logger logger;
    Logger_init(&logger, name, true);

	Rules rules;
	loadRules(&rules, getDecks(&arguments));

	Parameters params;
	initParameters(name, &params, &rules, &logger, getDecks(&arguments), getStrategy(&arguments), getNumberOfDecks(&arguments), arguments.hands);

	sprintf(buffer, "Starting: %s ...\n\n", name);
	Logger_simulation(&logger, buffer);
	Logger_simulation(&logger, "  -- arguments -------------------------------------------------------------------\n");
	printParameters(&params);
	printRules(&rules, &logger);
	Logger_simulation(&logger, "  --------------------------------------------------------------------------------\n");

	Simulation* sim = newSimulation(&params, &rules);
	initStrategy();
	simulatorRunOnce(sim);
	simulationDelete(sim);

	sprintf(buffer, "\nComplete: %s ...\n\n", name);
	Logger_simulation(&logger, buffer);
	return 0;
}

//
void generateName(char* buffer, size_t buffer_size) {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);

    int year = tm_info->tm_year + 1900;
    int month = tm_info->tm_mon + 1;
    int day = tm_info->tm_mday;

    snprintf(buffer, buffer_size, "%s_%4d_%02d_%02d_%012ld", STRIKER_WHO_AM_I, year, month, day, (long)t);
}

