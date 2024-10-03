#include "logger.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <uuid/uuid.h>
#include "constants.h"

// Utility function to create directories
int create_directory(const char* dir) {
	struct stat st = {0};
	if (stat(dir, &st) == -1) {
		if (mkdir(dir, 0700) != 0) {
			perror("mkdir failed");
			return 0;
		}
	}
	return 1;
}

// Initialize the logger
void Logger_init(Logger* logger, const char* name, bool debugFlag) {
	strcpy(logger->name, name);
	logger->debugFlag = debugFlag;

	strcpy(logger->directory, getSimulationDirectory());
	Logger_getSubdirectory(logger);

	// Create the directory if it does not exist
	snprintf(logger->fullPath, sizeof(logger->fullPath), "%s/%s", logger->directory, logger->subdirectory);

	if (!create_directory(logger->fullPath)) {
		printf("Failed to create directory: %s\n", logger->fullPath);
		return;
	}

	// Set up simulation file name and open it
	snprintf(logger->simulationFileName, sizeof(logger->simulationFileName), "%s/%s_simulation.txt", logger->fullPath, logger->name);
	logger->simulationFile = fopen(logger->simulationFileName, "w");
	if (!logger->simulationFile) {
		printf("Failed to open simulation file: %s\n", logger->simulationFileName);
		return;
	}

	// Set up debug file if required
	if (logger->debugFlag) {
		snprintf(logger->debugFileName, sizeof(logger->debugFileName), "%s/%s_debug.txt", logger->fullPath, logger->name);
		logger->debugFile = fopen(logger->debugFileName, "w");
		if (!logger->debugFile) {
			perror("Failed to open debug file");
		}
	}
}

// Clean up logger (close files)
void Logger_cleanup(Logger* logger) {
	if (logger->simulationFile) {
		fclose(logger->simulationFile);
		logger->simulationFile = NULL;
	}

	if (logger->debugFile) {
		fclose(logger->debugFile);
		logger->debugFile = NULL;
	}
}

// Log simulation messages
void Logger_simulation(Logger* logger, const char* message) {
	if (logger->simulationFile) {
		fprintf(logger->simulationFile, "%s", message);
		printf("%s", message);  // Print to console as well
		fflush(stdout);
	}

	if (logger->debugFile) {
		fprintf(logger->debugFile, "%s", message);
	}
}

// Log debug messages
void Logger_debug(Logger* logger, const char* message) {
	if (logger->debugFile) {
		fprintf(logger->debugFile, "%s", message);
	}
}

// Log insert messages
void Logger_insert(Logger* logger, const char* message) {
	char insertFileName[512];
	snprintf(insertFileName, sizeof(logger->debugFileName), "%s/%s_insert.txt", logger->fullPath, logger->name);

	FILE* insertFile = fopen(insertFileName, "w");
	if (insertFile) {
		fprintf(insertFile, "%s", message);
		fclose(insertFile);
	} else {
		perror("Failed to open insert file");
	}
}

// Get subdirectory based on the current date and generate a GUID
void Logger_getSubdirectory(Logger* logger) {
	time_t now = time(NULL);
	struct tm* localTime = localtime(&now);
	snprintf(logger->subdirectory, sizeof(logger->subdirectory), "%04d_%02d_%02d", localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday);
}

