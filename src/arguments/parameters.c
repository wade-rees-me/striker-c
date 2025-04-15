#include "parameters.h"
#include <stdio.h>
#include <stdlib.h>

// Local functions
void getCurrentTime(char *buffer, long bufferSize);
void generateName(char *buffer);

//
Parameters *newParameters(Arguments *arguments) {
    Parameters *parameters = (Parameters *)malloc(sizeof(Parameters));

    generateName(parameters->name);
    snprintf(parameters->decks, MAX_STRING_SIZE, "%s", getDecks(arguments));
    snprintf(parameters->strategy, MAX_STRING_SIZE, "%s", getStrategy(arguments));
    parameters->number_of_decks = getNumberOfDecks(arguments);
    parameters->number_of_hands = arguments->number_of_hands;
    parameters->number_of_threads = arguments->number_of_threads;
    parameters->share_of_hands = (parameters->number_of_hands / parameters->number_of_threads) + 1;
    parameters->verbose = parameters->number_of_threads == 1;
    snprintf(parameters->playbook, MAX_STRING_SIZE * 2, "%s-%s", parameters->decks, parameters->strategy);
    snprintf(parameters->processor, MAX_STRING_SIZE, "%s", STRIKER_WHO_AM_I);
    getCurrentTime(parameters->epoch, sizeof(parameters->epoch));

    return parameters;
}

//
void parametersDelete(Parameters *parameters) { free(parameters); }

// Print the Parameters
void printParameters(const Parameters *parameters) {
    char buffer[MAX_BUFFER_SIZE];

    printf("    %-26s: %s\n", "Processor", parameters->processor);
    printf("    %-26s: %d\n", "Threads", parameters->number_of_threads);
    printf("    %-26s: %s\n", "Name", parameters->name);
    printf("    %-26s: %s\n", "Version", STRIKER_VERSION);
    printf("    %-26s: %s\n", "Playbook", parameters->playbook);
    printf("    %-26s: %s\n", "Decks", parameters->decks);
    printf("    %-26s: %s\n", "Strategy", parameters->strategy);
    printf("    %-26s: %17s\n", "Number of hands",
           convertToStringWithCommas(parameters->number_of_hands, buffer, MAX_BUFFER_SIZE));
    printf("    %-26s: %17s\n", "Thread share of hands",
           convertToStringWithCommas(parameters->share_of_hands, buffer, MAX_BUFFER_SIZE));
    printf("    %-26s: %s\n", "Epoch", parameters->epoch);
}

//
void getCurrentTime(char *buffer, long bufferSize) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, bufferSize, TIME_LAYOUT, timeinfo);
}

//
void generateName(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    int year = tm_info->tm_year + 1900;
    int month = tm_info->tm_mon + 1;
    int day = tm_info->tm_mday;

    snprintf(buffer, MAX_STRING_SIZE, "%s_%4d_%02d_%02d_%012ld", STRIKER_WHO_AM_I, year, month, day, (long)t);
}

