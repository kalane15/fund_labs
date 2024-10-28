#pragma once
typedef struct tnode tnode;
typedef struct tnode {
	char* word;
	int count;
	struct tnode* left;
	struct tnode* right;
} tnode;


//kErrors AddNode(tnode** p, const char* w, tnode** added_node);
void treeprint(struct tnode* p);
int StringComparator(char* a, char* b);