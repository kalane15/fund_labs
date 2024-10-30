#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>


#include "limits.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"
#include "myerrors.h"
#include "string.h"
#include "mystring.h"

bool string_create(const char* s, string* out) {
	out->symbols = (char*)malloc(strlen(s) + 1);
	if (out->symbols == NULL) {
		return false;
	}
	out->size= strlen(s);
	strcpy(out->symbols, s);
	return true;;
}

void string_clear(string* s) {
	s->size = 0;
	strcpy(s->symbols, "");
}

void string_destroy(string* s) {
	string_clear(s);
	free(s->symbols);
}

int string_comp(string* s1, string* s2) {
	if (s1->size != s2->size) {
		return s1->size < s2->size ? -1 : 1;
	}
	return strcmp(s1->symbols, s2->symbols);
}

bool string_equal(string* s1, string* s2) {
	return strcmp(s1->symbols, s2->symbols);
}

void string_copy(string* dest, string* src) {
	dest->size = src->size;
	strcpy(dest->symbols, src->symbols);
}

void string_cat(string* dest, string* src) {
	dest->size = dest->size + src->size;
	strcat(dest->symbols, src->symbols);
}

char* string_pr(string s) {
	return s.symbols;
}