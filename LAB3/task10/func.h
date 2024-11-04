#pragma once
#include <stdio.h>
#include "myerrors.h"

typedef struct Node {
	char value;
	struct Node** children;
	int child_count;
	int child_capacity;
} Node;

//kOpt CheckExpression(const char *expr);
void PrintTree(Node *root, FILE *output, int x);
void FreeTree(Node *root);
Node* CreateNode(char value);
kErrors ParseExpression(const char** expr, Node**);
kErrors ValidateInput(int argc, char **argv);