#include "chart.h"
#include "constants.h"

//
void initChart(Chart *chart, const char *name) {
	strcpy(chart->name, name);
	chart->nextRow = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
		ChartRow *row = &chart->rows[i];
		strcpy(row->key, "--");
    	for (int j = 0; j < 13; j++) {
			strcpy(row->value[j], "---");
    	}
    }
}

//
int chartGetRowCount(Chart *chart) {
	return chart->nextRow;
}

//
ChartRow *chartGetRow(Chart *chart, const char *key) {
    for (int i = 0; i < chart->nextRow; i++) {
		ChartRow *row = &chart->rows[i];
		char keyLower[50];
		strcpy(keyLower, key);
		toUpperString(keyLower);
		if(strcmp(row->key, keyLower) == 0) {
			return row;
		}
	}
	return NULL;
}

//
void chartInsert(Chart *chart, const char *key, int up, const char *value) {
	ChartRow *row = chartGetRow(chart, key);
	if (row == NULL) {
		row = &chart->rows[chart->nextRow++];
		strcpy(row->key, key);
		toUpperString(row->key);
	}
	strcpy(row->value[up], value);
	toUpperString(row->value[up]);
}

//
const char *chartGetValue(Chart *chart, const char *key, int up) {
	ChartRow *row = chartGetRow(chart, key);
	if (row != NULL) {
		return row->value[up];
	}
	printf("Cannot find value in %s for %s vs %d\n", chart->name, key, up);
	exit(-1);
}

//
const char *chartGetValueByTotal(Chart *chart, int total, int up) {
 	char key[KEY_SIZE];
    snprintf(key, sizeof(key), "%d", total);
	return chartGetValue(chart, key, up);
}

void chartPrint(Chart *chart) {
	printf("%s\n", chart->name);
	printf("--------2-----3-----4-----5-----6-----7-----8-----9-----T-----J-----Q-----K-----A---\n");
    for (int i = 0; i < chart->nextRow; i++) {
		ChartRow *row = &chart->rows[i];
		printf("%2s : ", row->key);
    	for (int j = 0; j < 13; j++) {
			printf("%4s, ", row->value[j]);
    	}
		printf("\n");
	}
	printf("------------------------------------------------------------------------------------\n\n");
}

