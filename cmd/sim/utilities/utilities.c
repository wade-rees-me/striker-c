#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* addCommas(long long number) {
	char buffer[128];
	char* result = malloc(128);
	int i, j = 0, len;

	sprintf(buffer, "%lld", number);
	len = strlen(buffer);
	for (i = 0; i < len; i++) {
		result[j++] = buffer[i];

		if ((len - i - 1) % 3 == 0 && i != len - 1 && buffer[i] != '-') {
			result[j++] = ',';
		}
	}
	result[j] = '\0';

	return(result);
}

void printWithCommas(long long number) {
	char buffer[50];
	char result[50] = "";
	int i, j = 0, len;

	// Convert the number to a string
	sprintf(buffer, "%lld", number);

	len = strlen(buffer);

	for (i = 0; i < len; i++) {
		result[j++] = buffer[i];

		if ((len - i - 1) % 3 == 0 && i != len - 1 && buffer[i] != '-') {
			result[j++] = ',';
		}
	}

	result[j] = '\0';

	printf("%s", result);
}
