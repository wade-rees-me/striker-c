#ifndef REQUEST_H
#define REQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "constants.h"

// Define the Request struct
typedef struct {
    char responseString[MAX_MEMORY_SIZE];
    cJSON *jsonResponse;
} Request;

// Function prototypes
void requestFetchJson(Request *request, const char *url);

#endif // REQUEST_H
