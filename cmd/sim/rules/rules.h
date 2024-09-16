#ifndef RULES_H
#define RULES_H

#include <stdbool.h>

// Struct to hold table rules
typedef struct {
    char playbook[50];
    bool hit_soft_17;
    bool surrender;
    bool double_any_two_cards;
    bool double_after_split;
    bool resplit_aces;
    bool hit_split_aces;
} RulesTableStruct;

// Declare the global simulation argument structure
extern RulesTableStruct TableRules;

// Function declarations
extern void rules_load_table(const char *decks);

#endif // RULES_H
