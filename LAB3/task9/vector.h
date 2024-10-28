#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "bin_tree.h"
#define TYPE tnode*

typedef struct MatchResult {
	unsigned int string_ind;
	unsigned int symbol_ind;
} MatchResult;

typedef struct vector {
	TYPE* buffer;
	int buffer_size;
	int size;
} vector;


bool vector_create(vector* v, int init_buff_size);
bool vector_is_empty(vector* v);
bool vector_push(vector* v, TYPE d);
void v_set(vector* v, int pos, TYPE val);
TYPE v_get(vector* v, int pos);
bool vector_grow(vector* v);
void vector_destroy(vector* v);