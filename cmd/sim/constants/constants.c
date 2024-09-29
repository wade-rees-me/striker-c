#include <stdlib.h>

//
const char* getStrategyUrl() {
	return getenv("STRIKER_URL_ACE");
}

//
const char* getStrategyMlbUrl() {
	return getenv("STRIKER_URL_MLB");
}

//
const char* getRulesUrl() {
	return getenv("STRIKER_URL_RULES");
}

//
const char* getSimulationUrl() {
	return getenv("STRIKER_URL_SIMULATION");
}

const char* getSimulationDirectory() {
	return getenv("STRIKER_SIMULATIONS");
}

int64_t getDatabaseMinimumRounds() {
	char* str = getenv("STRIKER_MINIMUM_ROUNDS");
	char* end;
	return strtol(str, &end, 10);
}

