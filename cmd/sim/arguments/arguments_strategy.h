#ifndef ARGUMENTS_STRATEGY_H
#define ARGUMENTS_STRATEGY_H

#include <stdbool.h>

// Define the strategy structure in C
typedef struct {
    bool mimic_flag;
    bool basic_flag;
    bool linear_flag;
    bool polynomial_flag;
    bool high_low_flag;
    bool wong_flag;
    bool striker_flag;
} StrategyArgs;

// Declare the global strategy argument structure
extern StrategyArgs CLStrategy;

// Function declarations
extern void strategy_parse_flags(int argc, char *argv[]);
extern const char* strategy_get();

#endif // ARGUMENTS_STRATEGY_H

