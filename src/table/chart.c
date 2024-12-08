#include "chart.h"

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
ChartRow *chartGetRow(Chart *chart, const char *key) {
    for (int i = 0; i < chart->nextRow; i++) {
		ChartRow *row = &chart->rows[i];
		if(strcmp(row->key, key) == 0) {
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
	}
	strcpy(row->value[up], value);
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

void chartPrint(Chart *chart) {
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

