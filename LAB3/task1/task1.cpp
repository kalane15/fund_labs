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


int Add(int a, int b);
int Subtract(int a, int b);

char* strrev(char* str)
{
	if (!str || !*str)
		return str;

	int i = strlen(str) - 1, j = 0;

	char ch;
	while (i > j)
	{
		ch = str[i];
		str[i] = str[j];
		str[j] = ch;
		i--;
		j++;
	}
	return str;
}

typedef enum kErrors {
	SUCCESS = 0,
	INC_ARGS,
	ERR_OVERFLOW,
	MEM_ALLOC_ERR
} kErrors;

char IntToChar(int a) {
	if (a < 10) {
		return Add('0', a);
	}
	else {
		return Subtract(Add('A', a), 10);
	}
}

int Increment(int a) {
	if ((a & 1) == 0) {
		return a | 1;
	}
	else {
		return Increment(a >> 1) << 1;
	}
}

int Add(int a, int b) {
	while (b != 0) {
		int in_memory = a & b;
		a = a ^ b;
		b = in_memory << 1;
	}
	return a;
}

int Subtract(int a, int b) {
	int neg_b = Add(~b, 1);
	return Add(a, neg_b);
}

bool CheckErrors(kErrors status) {
	if (status == SUCCESS) {
		return true;
	}

	switch (status) {
	case INC_ARGS:
		printf("Incorrect arguments\n");
		break;
	case MEM_ALLOC_ERR:
		printf("Error during memory allocating\n");
		break;
	case ERR_OVERFLOW:
		printf("overflow\n");
		break;
	default:
		printf("Uknown error\n");
	}
	return false;
}

kErrors ConvertTo2SS(ll number, int r, char* out) {
	if (r < 1 || r > 5) {
		return INC_ARGS;
	}
	strcpy(out, "");
	int sign = 1;
	int i = 0;
	if (number < 0) {
		number = abs(number);
		sign = -1;
	}

	int base = 1 << r;
	int ost;

	int o = 0;
	for (int i = 0; i < r; i = Increment(i)) {
		o = o | (1 << i);
	}

	while (number > 0) {
		ost = number & (o);
		out[i] = IntToChar(ost);
		i = Increment(i);
		number = number >> r;
	}
	if (sign == -1) {
		out[i] = '-';
		i = Increment(i);
	}
	out[i] = '\0';
	strrev(out);
	return SUCCESS;
}

int main() {
	char* res = (char*)malloc(1024);
	if (res == NULL) {
		return MEM_ALLOC_ERR;
	}

	kErrors status = ConvertTo2SS(52, 2, res);
	if (CheckErrors(status)) {
		printf("%s\n", res);
	}
	free(res);
}