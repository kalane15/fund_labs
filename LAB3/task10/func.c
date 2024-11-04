#include "func.h"
#include <stdlib.h>
#include <string.h>
#include "Windows.h"
#include "myerrors.h"

//kOpt CheckExpression(const char *expr){
//	int count = 0;
//	while (*expr){
//		switch (*expr){
//			case '(':
//				count++;
//				break;
//			case ')':
//				count--;
//				break;
//		}
//		expr++;
//	}
//	if (!count){
//		return OPT_SUCCESS;
//	}
//	return OPT_ERROR_INPUT;
//}

Node* CreateNode(char value) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	if (!newNode){
		fprintf(stderr, "Error: Failed to allocate memory for new node\n");
		return NULL;
	}

	newNode->value = value;
	newNode->child_count = 0;
	newNode->child_capacity = 2;
	newNode->children = (Node**)malloc(newNode->child_capacity * sizeof(Node*));
	if (!newNode->children){
		fprintf(stderr, "Error: Failed to allocate memory for children array\n");
		free(newNode);
		return NULL;
	}
	return newNode;
}

kErrors AddChild(Node* parent, Node* child) {
	if (parent->child_count == parent->child_capacity) {
		parent->child_capacity *= 2;
		Node** newChildren = (Node**)realloc(parent->children, parent->child_capacity * sizeof(Node*));
		if (newChildren == NULL) {
			fprintf(stderr, "Error: Failed to reallocate memory for children array\n");
			return MEM_ALLOC_ERR;
		}
		parent->children = newChildren;
	}
	parent->children[parent->child_count++] = child;
	return SUCCESS;
}

void FreeTree(Node* root) {
	if (root == NULL) return;
	for (int i = 0; i < root->child_count; i++) {
		FreeTree(root->children[i]);
	}
	free(root->children);
	free(root);
}

 kErrors ParseExpression(const char** expr, Node** out) {
	if (**expr == '\0' || **expr == ')') return SUCCESS;
	kErrors status = SUCCESS;
	Node* root = CreateNode(**expr);
	if (!root) return MEM_ALLOC_ERR;

	(*expr)++;

	if (**expr == '(') {
		(*expr)++;
		while (**expr != '\0' && **expr != ')') {
			Node* child;
			status = ParseExpression(expr, &child);
			if (status == SUCCESS) {
				status = AddChild(root, child);
				if (status != SUCCESS){
					FreeTree(root);
					return status;
				}
			}else{
				FreeTree(root);
				return status;
			}
			if (**expr == ',') (*expr)++;
		}
		(*expr)++;
	}
	*out = root;
	return status;
}


void PrintTree(Node* root, FILE* file, int depth) {
	if (root == NULL) return;

	for (int i = 0; i < depth; i++) {
		fprintf(file, "  ");
	}

	fprintf(file, "%c\n", root->value);

	for (int i = 0; i < root->child_count; i++) {
		PrintTree(root->children[i], file, depth + 1);
	}
}

kErrors ValidateInput(int argc, char **argv){
	char full_in[BUFSIZ], full_out[BUFSIZ];
	if (argc != 3) return INC_NUM_OF_ARGS;
	if (!GetFullPathNameA(argv[1], BUFSIZ, full_in, NULL) || !GetFullPathNameA(argv[2], BUFSIZ, full_out, NULL)) return INC_FILE;
	if (!strcmp(full_in, full_out)) return INC_FILE;

	return SUCCESS;
}