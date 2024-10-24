#include <stdio.h>
#include "arguments.h"
#include "parameters.h"
#include "rules.h"
#include "strategy.h"
#include "simulator.h"
#include "constants.h"

//
int main(int argc, char* argv[]) {
	Arguments *arguments = newArguments(argc, argv);
	Parameters *parameters = newParameters(getDecks(arguments), getStrategy(arguments), getNumberOfDecks(arguments), arguments->number_of_hands);
	Rules *rules = newRules(getDecks(arguments));
    Strategy *strategy = newStrategy(getDecks(arguments), getStrategy(arguments), getNumberOfDecks(arguments) * 52);
	Simulator *simulator = newSimulator(parameters, rules, strategy);

	printf("Start: %s\n\n", STRIKER_WHO_AM_I);
	printf("  -- arguments -------------------------------------------------------------------\n");
	printParameters(parameters);
	printRules(rules);
	printf("  --------------------------------------------------------------------------------\n");

	simulatorRunOnce(simulator);
	printf("\nEnd: %s\n\n", STRIKER_WHO_AM_I);

	simulatorDelete(simulator);
	rulesDelete(rules);
	parametersDelete(parameters);
	argumentsDelete(arguments);

	return 0;
}

