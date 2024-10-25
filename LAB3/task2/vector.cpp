#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include <climits>
#include <cstring>
#include <locale>

#include "limits.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"
#include "vector.h"

#define ll long long
#define ull unsigned long long
#define ld long double
#define uint unsigned int


bool vector_create(vector* v, int count, ...) {
	v->coordinates = (double*)malloc(sizeof(double) * count);
	if (v->coordinates == NULL) {
		return false;
	}
	va_list arg;
	va_start(arg, count);

	for (int i = 0; i < count; i++) {
		v->coordinates[i] = va_arg(arg, double);
	}
	v->dimensions = count;
	va_end(arg);
}

bool vector_create_zero(vector* v, int count) {
	v->coordinates = (double*)malloc(sizeof(double) * count);
	if (v->coordinates == NULL) {
		return false;
	}
	

	for (int i = 0; i < count; i++) {
		v->coordinates[i] = 0.0;
	}
	v->dimensions = count;
}

void vector_print(vector* v) {
	if (v == NULL) {
		return;
	}

	for (int i = 0; i < v->dimensions; i++) {
		printf("%f ", v->coordinates[i]);
	}
	printf("\n");
}

void vector_destroy(vector* v) {
	free(v->coordinates);
	v->dimensions = 0;
}