#pragma once
typedef struct string {
	int size;
	char* symbols;
} string;

bool string_create(const char* s, string* out);

void string_clear(string* s);

void string_destroy(string* s);

int string_comp(string* s1, string* s2);

bool string_equal(string* s1, string* s2);

void string_copy(string* dest, string* src);

void string_cat(string* dest, string* src);

char* string_pr(string s);
