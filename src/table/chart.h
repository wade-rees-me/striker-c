#ifndef CHART_H
#define CHART_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 21

//
typedef struct ChartRow {
    char key[6];
	char value[13][6]; // 2, 3, ... A
} ChartRow;

//
typedef struct Chart {
    ChartRow rows[TABLE_SIZE];
    char name[64];
	int nextRow;
} Chart;

void initChart(Chart *chart, const char *name) ;
ChartRow *chartGetRow(Chart *chart, const char *key);
void chartInsert(Chart *chart, const char *key, int up, const char *value);
const char *chartGetValue(Chart *chart, const char *key, int up);
void chartPrint(Chart *chart);

#endif // CHART_H
