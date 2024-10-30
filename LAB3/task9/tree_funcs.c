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
#include "bin_tree.h"
#include "vector.h"
kErrors AddNode(tnode** p, const char* w, tnode** added_node, bool* is_added);
void ClearBuffer(void)
{
    fseek(stdin, 0, SEEK_END);
}

bool InArray(char c, char** array, int array_size) {
    for (int i = 0; i < array_size; i++) {
        if (c == array[i][0]) {
            return true;
        }
    }
    return false;
}

void ClearTree(tnode* tree) {
    if (tree->left != NULL) {
        ClearTree(tree->left);
    }
    if (tree->right != NULL) {
        ClearTree(tree->right);
    }
    free(tree->word);
}

kErrors ReadTreeFromFile(FILE* in, tnode** out, char** seps, int seps_count, vector* v) {
    char* cur = (char*)malloc(256);
    if (cur == NULL) {
        return MEM_ALLOC_ERR;
    }
    strcpy(cur, "");
    char c;
    kErrors status = SUCCESS;


    if (cur == NULL) {
        return MEM_ALLOC_ERR;
    }
    tnode* cur_node = NULL;
    bool st;
    bool is_added = false;
    while (fscanf(in, "%c", &c) >  0) {
        if (InArray(c, seps, seps_count) && strlen(cur) > 0) {
            status = AddNode(out, cur, &cur_node, &is_added);
            if (status != SUCCESS) {
                ClearTree(*out);
                free(cur);
                return status;
            }
            if (is_added) {
                st = vector_push(v, cur_node);
                if (st == false) {
                    ClearTree(*out);
                    free(cur);
                    return MEM_ALLOC_ERR;
                }
            }           

            cur = (char*)malloc(256);
            if (cur == NULL) {
                ClearTree(*out);
                free(cur);
                return MEM_ALLOC_ERR;
            }
            strcpy(cur, "");
        }
        else if (!InArray(c, seps, seps_count)){
            strncat(cur, &c, 1);
        }
    }

    if (strlen(cur) != 0) {
        status = AddNode(out, cur, &cur_node, &is_added);
        if (status != SUCCESS) {
            ClearTree(*out);
            free(cur);
            return status;
        }
        if (is_added) {
            st = vector_push(v, cur_node);
            if (st == false) {
                ClearTree(*out);
                free(cur);
                return MEM_ALLOC_ERR;
            }
        }
    }
    free(cur);
    return SUCCESS;
}

void PrintTnode(tnode* node)
{
    printf("%s (%d)\n", node->word, node->count);
}

void PrintTree(tnode* root, int tab_count)
{
    if (root != NULL)
    {
        PrintTree(root->right, tab_count + 1);
        for (int i = 0; i < tab_count; ++i) { printf("\t"); }
        PrintTnode(root);
        PrintTree(root->left, tab_count + 1);
    }
}

tnode* FindLongest(tnode* tree) {
    tnode* cur = tree;
    while (cur->right != NULL) {
        cur = cur->right;
    }
    return cur;
}

tnode* FindShortest(tnode* tree) {
    tnode* cur = tree;
    while (cur->left != NULL) {
        cur = cur->left;
    }
    return cur;
}

int TnodeCompCount(void* a, void* b) {
    tnode* t1 = *(tnode**)a;
    tnode* t2 = *(tnode**)b;

    if (t1->count == t2->count) {
        return 0;
    }
    else {
        return t1->count > t2->count ? -1 : 1;
    }
}

kErrors PrintFirstN(vector* v, int n) {
    if (n > v->size || n <= 0) {
        return INC_ARGS;
    }
    for (int i = 0; i < n; i++) {
        printf("Слово: %s; число вхождений: %d\n", v_get(v, i)->word, v_get(v, i)->count);
    }
    return SUCCESS;
}

int FindDepth(tnode* tree)
{
	if (tree == NULL) {
		return 0;
	}

    int left_depth = FindDepth(tree->left);
    int right_depth = FindDepth(tree->right);

    return 1 + (left_depth > right_depth ? left_depth : right_depth);
}

kErrors SaveTreeToFile(tnode* tree, FILE* out) {
    fprintf(out, "%d %s\n", tree->count, tree->word);
    if (tree->left != NULL) {
        SaveTreeToFile(tree->left, out);
    }
    if (tree->right != NULL) {
        SaveTreeToFile(tree->right, out);
    }   
    return SUCCESS;
}

kErrors LoadTreeFromFile(tnode** tree, FILE* out, vector* v) {
    char* s = (char*)malloc(1024);
    int a;
    tnode* node = NULL;
    bool b;
    int count = 0;
    count = fscanf(out, "%d %s\n", &a, s);
    while (!feof(out) || count > 0) {        
        if (count != 2) {
            free(s);
            return INC_INP_DATA;
        }
        AddNode(tree, s, &node, &b);
        if (b) {
            if (!vector_push(v, node)) {
                free(s);
                return MEM_ALLOC_ERR;
            }
        }
        node->count = a;
        count = fscanf(out, "%d %s\n", &a, s);
    }
    free(s);
    return SUCCESS;
}

tnode* Search(char* word, tnode* tree) {
    if (tree == NULL) {
        return NULL;
    }
    if (StringComparator(word, tree->word) < 0) {
        return Search(word, tree->left);
    }
    else if (StringComparator(word, tree->word) > 0) {
        return Search(word, tree->right);
    }
    else {
        return tree;
    }
}