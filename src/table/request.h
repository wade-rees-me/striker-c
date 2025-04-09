#ifndef REQUEST_H
#define REQUEST_H

#include "constants.h"
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the Request
typedef struct {
    char responseString[MAX_MEMORY_SIZE];
    cJSON *jsonResponse;
} Request;

// Function prototypes
void requestFetchJson(Request *request, const char *url);

#endif // REQUEST_H
