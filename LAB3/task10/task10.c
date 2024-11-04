#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "func.h"
#include "myerrors.h"

int main(int argc, char* argv[]) {
	kErrors status = ValidateInput(argc, argv);
	if (status != SUCCESS){
		return ProccessError(status);
	}

	FILE* input = fopen(argv[1], "r");
	if (!input) {
		return ProccessError(INC_FILE);
	}

	FILE* output = fopen(argv[2], "w");
	if (!output) {
		fclose(input);
		return ProccessError(INC_FILE);
	}

	char *line = (char*)malloc(1024);
	size_t line_size = 0;
	int count = 1;
	while (fgets(line, 1024, input) >  0) {
		const char* expr = line;
		Node* root;
		status = ParseExpression(&expr, &root);
		if (status != SUCCESS){
			break;
		}
		fprintf(output,"Tree number %d:\n", count++);
		PrintTree(root, output, 0);
		fprintf(output, "\n");
		FreeTree(root);
	}

	free(line);
	fclose(input);
	fclose(output);
	return ProccessError(status);
}