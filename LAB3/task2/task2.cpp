#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <climits>
#include <cstring>
#include <locale>

#include "limits.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"

#include "vector.h"
#include "myerrors.h"

#define ll long long
#define ull unsigned long long
#define ld long double
#define uint unsigned int





void MultiplyMatrixVector(int dimensions, double* matrix, vector* v, vector* out) {
	for (int i = 0; i < dimensions; i++) {
		double res = 0;
		for (int j = 0; j < dimensions; j++) {
			res += matrix[i * dimensions + j] * v->coordinates[j];
		}
		out->coordinates[i] = res;
	}
}


kErrors norma1(vector* v, int dimensions, double* out) {
	double res = -1;

	for (int i = 0; i < v->dimensions; i++) {
		res = fabs(v->coordinates[i]) > res ? fabs(v->coordinates[i]) : res;
	}

	if (!isinf(res)) {
		return ERR_OVERFLOW;
	}
	else if (!isnormal(res)) {
		return INC_ARGS;
	}
	*out = res;
	return SUCCESS;
}

kErrors norma2(vector* v, int dimensions, double* out) {	
	double a = 0.0; //overflow?
	const int p = 3;

	for (int i = 0; i < v->dimensions; i++) {
		a += pow(v->coordinates[i], p);
	}

	if (!isinf(a)) {
		return ERR_OVERFLOW;
	}
	else if (!isnormal(a)) {
		return INC_ARGS;
	}

	a = pow(a, 1.0 / p);
	*out = a;
	return SUCCESS;
}

void GenerateMatrix(double* out, int dimensions) {
	for (int i = 0; i < dimensions; i++) {
		for (int j = 0; j < dimensions; j++) {
			out[i * dimensions + j] = i == j ? 1.0 : 0.0;
		}
	}
}

kErrors norma3(vector* v, int dimensions, double* out){
	double* a = (double*)malloc(dimensions * dimensions * sizeof(double));
	if (a == NULL) {
		return MEM_ALLOC_ERR;
	}
	GenerateMatrix(a, dimensions);


	vector temp;
	vector_create_zero(&temp, dimensions);
	MultiplyMatrixVector(dimensions, a, v, &temp);

	double res = 0;
	for (int i = 0; i < dimensions; i++) {
		res += temp.coordinates[i] * v->coordinates[i];
	}

	res = sqrt(res);
	*out = res;

	vector_destroy(&temp);
	free(a);
	return SUCCESS;
}


kErrors FinddMaxForN(int n, kErrors (*norma)(vector*, int, double*), int count, vector** out, vector** vectors) {
	vector* cur;

	double max = -1;
	double cur_norma;
	kErrors status;
	for (int i = 0; i < count; i++) {
		cur = vectors[i];
		status = norma(cur, n, &cur_norma);
		if (status != SUCCESS) {
			return status;
		}
		if (cur_norma > max) {
			max = cur_norma;
		}
	}
	int out_ind = 0;

	for (int i = 0; i < count; i++) {
		cur = vectors[i];
		status = norma(cur, n, &cur_norma);
		if (status != SUCCESS) {
			return status;
		}
		if (fabs(cur_norma - max) < 1e-6) {
			out[out_ind++] = cur;
		}
	}
	return SUCCESS;
}


kErrors FindMax(int n, kErrors (*n1)(vector*, int, double*), kErrors (*n2)(vector*, int, double*), kErrors (*n3)(vector*, int, double*),
				vector** n1_out, vector** n2_out, vector** n3_out, int count, ...) {
	if (n <= 1) {
		return INC_ARGS;
	}
	
	if (count <= 0) {
		return INC_NUM_OF_ARGS;
	}

	vector** vectors = (vector**)malloc(sizeof(vector) * count);
	if (vectors == NULL) {
		return MEM_ALLOC_ERR;
	}

	double max = -1.0;

	va_list arg;
	va_start(arg, count);

	for (int i = 0; i < count; i++) {
		vectors[i] = va_arg(arg, vector*);
	}

	va_end(arg);
	kErrors status = FinddMaxForN(n, n1, count, n1_out, vectors);
	if (status != SUCCESS) {
		free(vectors);
		return status;
	}


	status = FinddMaxForN(n, n2, count, n2_out, vectors);
	if (status != SUCCESS) {
		free(vectors);
		return status;
	}
	
	status = FinddMaxForN(n, n3, count, n3_out, vectors);
	if (status != SUCCESS) {
		free(vectors);
		return status;
	}
	free(vectors);
	return SUCCESS;
}


int main()
{
	int n_dimensions = 3;
	vector** out1 = (vector**)calloc(10, sizeof(vector*));
	if (out1 == NULL) {
		return MEM_ALLOC_ERR;
	}

	vector** out2 = (vector**)calloc(10, sizeof(vector*));
	if (out2 == NULL) {
		return MEM_ALLOC_ERR;
	}

	vector** out3 = (vector**)calloc(10, sizeof(vector*));
	if (out3 == NULL) {
		return MEM_ALLOC_ERR;
	}

	vector v1, v2, v3;
	vector_create(&v1, n_dimensions, 0.0, 2.0, 0.0);
	vector_create(&v2, n_dimensions, 3.0, sqrt(3.0), 0.0);
	vector_create(&v3, n_dimensions, 2.0, 2.0, 2.0);
	kErrors status = FindMax(3, norma1, norma2, norma3, out1, out2, out3, n_dimensions, &v1, &v2, &v3);
	if (status != SUCCESS) {
		free(out1);
		free(out2);
		free(out3);
		return ProccessError(status);
	}

	printf("Norma 1\n");
	for (int i = 0; i < n_dimensions; i++) {
		vector_print(out1[i]);
	}

	printf("Norma 2\n");
	for (int i = 0; i < n_dimensions; i++) {
		vector_print(out2[i]);
	}

	printf("Norma 3\n");
	for (int i = 0; i < n_dimensions; i++) {
		vector_print(out3[i]);
	}
	free(out1);
	free(out2);
	free(out3);
	vector_destroy(&v1);
	vector_destroy(&v2);
	vector_destroy(&v3);
	return 0;
}