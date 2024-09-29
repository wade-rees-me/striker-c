#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// Logger structure to hold logger-related data
typedef struct {
	char simulator[128];
	char directory[256];
	char subdirectory[256];
	char guid[128];
	char fullPath[512];

	char simulationFileName[512];
	FILE* simulationFile;

	char debugFileName[512];
	FILE* debugFile;

	bool debugFlag;
} Logger;

// Function prototypes
void Logger_init(Logger* logger, const char* simulator, bool debugFlag);
void Logger_cleanup(Logger* logger);
void Logger_simulation(Logger* logger, const char* message);
void Logger_debug(Logger* logger, const char* message);
void Logger_insert(Logger* logger, const char* message);
void Logger_getSubdirectory(Logger* logger);

#endif // LOGGER_H
