#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

// Callback function for libcurl to write data
size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	char *ptr = realloc(mem->memory, mem->size + realsize + 1);
	if(ptr == NULL) {
		printf("Not enough memory (realloc returned NULL)\n");
		exit(1);
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

