#ifndef ARGUMENTS_TABLE_H
#define ARGUMENTS_TABLE_H

#include <stdbool.h>

// Define the table structure in C
typedef struct {
    bool single_deck_flag;
    bool double_deck_flag;
    bool six_shoe_flag;
} TableArgs;

// Declare the global table argument structure
extern TableArgs CLTable;

// Function declarations
extern void table_parse_flags(int argc, char *argv[]);
extern const char* table_get(int *numDecks);

#endif // ARGUMENTS_TABLE_H

