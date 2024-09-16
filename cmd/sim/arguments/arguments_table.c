#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "arguments_table.h"

// Global table configuration structure
TableArgs CLTable = {false, false, false};

// Function to parse command-line flags
void table_parse_flags(int argc, char *argv[]) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-table-single-deck") == 0) {
			CLTable.single_deck_flag = true;
		} else if (strcmp(argv[i], "-table-double-deck") == 0) {
			CLTable.double_deck_flag = true;
		} else if (strcmp(argv[i], "-table-six-shoe") == 0) {
			CLTable.six_shoe_flag = true;
		}
	}
}

// Function to get the table configuration (deck type and number of decks)
const char* table_get(int *numDecks) {
	if (CLTable.double_deck_flag) {
		*numDecks = 2;
		return "double-deck";
	}
	if (CLTable.six_shoe_flag) {
		*numDecks = 6;
		return "six-shoe";
	}
	*numDecks = 1;
	return "single-deck";
}

