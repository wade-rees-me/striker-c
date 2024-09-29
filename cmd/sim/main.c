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

//
int main(int argc, char* argv[]) {
	Arguments arguments;
	parseArguments(&arguments, argc, argv);

	Logger logger;
    Logger_init(&logger, STRIKER_WHO_AM_I, true);

	Rules rules;
	loadRules(&rules, getDecks(&arguments));

	Parameters params;
	initParameters(&params, &rules, &logger, getDecks(&arguments), getStrategy(&arguments), getNumberOfDecks(&arguments), arguments.rounds);

	Logger_simulation(&logger, "Starting: striker-c ...\n\n");
	Logger_simulation(&logger, "  -- arguments -------------------------------------------------------------------\n");
	printParameters(&params);
	printRules(&rules, &logger);
	Logger_simulation(&logger, "  --------------------------------------------------------------------------------\n");

	Simulation* sim = newSimulation(&params, &rules);
	initStrategy();
	simulatorRunOnce(sim);
	simulationDelete(sim);

	Logger_simulation(&logger, "\n\nComplete: striker-c ...\n");
	return 0;
}

