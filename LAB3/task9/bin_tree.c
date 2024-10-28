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
#include <time.h>
#include "bin_tree.h"


int StringComparator(char* a, char* b) {
	if (strlen(a) != strlen(b)) {
		return strlen(a) < strlen(b) ? -1 : 1;
	} 
	return strcmp(a, b);
}

kErrors AddNode(tnode** p, const char* w, tnode** added_node, bool* is_added)
{
	int cond;
	kErrors status = SUCCESS;
	if ((*p) == NULL) { 
		(*p) = (tnode*)malloc(sizeof(tnode));
		if ((*p) == NULL) {
			return MEM_ALLOC_ERR;
		}
		(*p)->word = _strdup(w);
		(*p)->count = 1;
		(*p)->left = (*p)->right = NULL;
		*added_node = *p;
		*is_added = true;
	}
	else if ((cond = StringComparator(w, (*p)->word)) == 0) {
		(*p)->count++;
		*is_added = false;
	}
	else if (cond < 0) 
		status = AddNode(&((*p)->left), w, added_node, is_added);
	else
		status = AddNode(&((*p)->right), w, added_node, is_added);
	return status;
}


void treeprint(struct tnode* p)
{
	if (p != NULL) {
		treeprint(p->left);
		printf("%4d %s\n", p->count, p->word);
		treeprint(p->right);
	}
}
