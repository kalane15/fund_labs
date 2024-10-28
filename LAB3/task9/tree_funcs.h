#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "locale.h"
#include "limits.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"
#include "myerrors.h"
#include "string.h"
#include "vector.h"

kErrors ReadTreeFromFile(FILE* in, tnode** out, char** seps, int seps_count, vector* v);
void ClearTree(tnode* tree);
void PrintTree(tnode* root, int tab_count);
void PrintTnode(tnode* node);
tnode* FindShortest(tnode* tree);
tnode* FindLongest(tnode* tree);
int TnodeCompCount(void* a, void* b);
kErrors PrintFirstN(vector* v, int n);
int FindDepth(tnode* tree);
void SaveTreeToFile(tnode* tree, FILE* out);
kErrors LoadTreeFromFile(tnode** tree, FILE* out, vector* v);
int StringComparator(char* a, char* b);
tnode* Search(char* word, tnode* tree);