#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "constants.h"

//
const char *getStrategyUrl() {
	return getenv("STRIKER_URL_STRATEGY");
}

//
const char *getRulesUrl() {
	return getenv("STRIKER_URL_RULES");
}

//
const char *getSimulationUrl() {
	return getenv("STRIKER_URL_SIMULATION");
}

// Function to convert bool to string
const char* boolToString(bool b) {
    return b ? "true" : "false";
}

//
void toUpperString(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

//
bool parseAuxBool(cJSON *json, char *tag, bool value) {
	cJSON *item = cJSON_GetObjectItem(json, tag);
	if (item != NULL) {
		return item->valueint == 1;
	}
	return value;
}

//
int parseAuxInt(cJSON *json, char *tag, int value) {
	cJSON *item = cJSON_GetObjectItem(json, tag);
	if (item != NULL) {
		return item->valueint;
	}
	return value;
}

//
double parseAuxDouble(cJSON *json, char *tag, double value) {
	cJSON *item = cJSON_GetObjectItem(json, tag);
	if (item != NULL) {
		return item->valuedouble;
	}
	return value;
}

//
char *convertToStringWithCommas(long long number, char *buffer, size_t bufferSize) {
    // Temporary buffer to store the number in reverse order
    char temp[100];  
    int len = 0;

    // Handle negative numbers
    int isNegative = 0;
    if (number < 0) {
        isNegative = 1;
        number = -number;  // Make the number positive for processing
    }

    // Convert the number into the temp array in reverse order
    do {
        temp[len++] = (number % 10) + '0';
        number /= 10;
    } while (number > 0);

    // Calculate the size needed for the result (including commas and possible negative sign)
    int resultSize = len + (len - 1) / 3 + (isNegative ? 1 : 0) + 1;

    // Ensure the provided buffer is large enough to hold the result
    if (resultSize > bufferSize) {
        printf("Error: Buffer is too small to hold the formatted number.\n");
        exit(-1);
    }

    int resultIndex = 0;
    if (isNegative) {
        buffer[resultIndex++] = '-';
    }

    // Add commas and copy digits into the buffer
    for (int i = 0; i < len; i++) {
        if (i > 0 && i % 3 == 0) {
            buffer[resultIndex++] = ',';  // Insert comma after every 3 digits
        }
        buffer[resultIndex++] = temp[i];
    }

    buffer[resultIndex] = '\0';  // Null-terminate the string

    // Reverse the string in place to correct the order
    int start = isNegative ? 1 : 0;  // Skip the negative sign if it's there
    int end = resultIndex - 1;
    while (start < end) {
        char tempChar = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = tempChar;
        start++;
        end--;
    }
	return buffer;
}

