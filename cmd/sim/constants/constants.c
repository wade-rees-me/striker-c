#include <stdlib.h>  // For getenv
#include "constants.h"

// Function to get environment variables

const char* get_strategy_url() {
	const char* url = getenv("STRIKER_URL_ACE");
	return url ? url : "";  // Return empty string if the variable is not set
}

const char* get_strategy_mlb_url() {
	const char* url = getenv("STRIKER_URL_MLB");
	return url ? url : "";
}

const char* get_rules_url() {
	const char* url = getenv("STRIKER_URL_RULES");
	return url ? url : "";
}

const char* get_simulation_url() {
	const char* url = getenv("STRIKER_URL_SIMULATION");
	return url ? url : "";
}

