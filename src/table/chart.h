#ifndef CHART_H
#define CHART_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 21
#define KEY_SIZE 64

//
typedef struct ChartRow {
    char key[KEY_SIZE];
	char value[13][KEY_SIZE]; // 2, 3, ... A
} ChartRow;

//
typedef struct Chart {
    ChartRow rows[TABLE_SIZE];
    char name[64];
	int nextRow;
} Chart;

void initChart(Chart *chart, const char *name);
int chartGetRowCount(Chart *chart);
ChartRow *chartGetRow(Chart *chart, const char *key);
void chartInsert(Chart *chart, const char *key, int up, const char *value);
const char *chartGetValue(Chart *chart, const char *key, int up);
const char *chartGetValueByTotal(Chart *chart, int total, int up);
void chartPrint(Chart *chart);

#endif // CHART_H
