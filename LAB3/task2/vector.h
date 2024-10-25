#pragma once
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
#define ll long long
#define ull unsigned long long
#define ld long double
#define uint unsigned int

typedef struct vector {
	double* coordinates;
	uint dimensions;
} vector;

bool vector_create(vector* v, int count, ...);
void vector_destroy(vector* v);
void vector_print(vector* v);
bool vector_create_zero(vector* v, int count);

