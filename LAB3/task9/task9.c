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
#include "tree_funcs.h"
#include "vector.h"

kErrors AddNode(tnode** p, const char* w, tnode** added_node, bool* is_added);
void translate(char* string)
{
	size_t len = strlen(string);

	if (len != 2) {
		return;
	}
	
	while (string[0] == '\\')
	{
		switch (string[1])
		{
		case 'n':
			string[0] = '\n';
			string[1] = '\0';
			break;

		case 't':
			string[0] = '\t';
			string[1] = '\0';
			break;
		default:
			return;
		}
	}
	return;
}

kErrors CheckInput(int argsc, char** args) {
	if (argsc < 3) {
		return INC_NUM_OF_ARGS;
	}
	for (int i = 2; i < argsc; i++) {
		translate(args[i]);
		if (strlen(args[i]) != 1) {
			return INC_SEPS;
		}
	}
	return SUCCESS;
}

void FreeStringArray(char** array, int count) {
	for (int i = 0; i < count; i++) {
		free(array[i]);
	}
	free(array);
}

kErrors CopyArgs(char** to, char** from, int argsc) {
	if (to == NULL) {
		return MEM_ALLOC_ERR;
	}

	for (int i = 0; i < argsc; i++) {
		to[i] = (char*)malloc(strlen(from[i]) + 1);
		if (to[i] == NULL) {
			FreeStringArray(to, i - 1);
			return MEM_ALLOC_ERR;
		}
		strcpy(to[i], from[i]);
	}
	return SUCCESS;
}

void PrintHelp() {
	printf("s - поиск слова\n");
	printf("h - помощь\n");
	printf("m - самое короткое и самое длинное слова\n");
	printf("l - загрузить из файла\n");
	printf("u - сохранить в файл\n");
	printf("d - определить глубину дерева\n");
	printf("n - вывод n самых часто встречающихся слов\n");
	printf("p - печать дерева\n");
}

kErrors ProccessFlag(char flag, tnode** tree, vector* v) {
	char word[1024];
	tnode* res;
	int a;
	int count = 0;
	FILE* file;
	kErrors status;
	switch (flag) {
	case 's':		
		printf("Введите слово:\n");
		count = scanf("%s", word);
		if (count != 1) {
			printf("Некорректный ввод\n");
			return INC_INP_DATA;
		}
		res = Search(word, *tree);
		if (res == NULL) {
			printf("Такого слова нет в файле\n");
		}
		else {
			printf("Число вхождений: %d\n", res->count);
		}
		break;
	case 'h':
			PrintHelp();
			break;
	case 'm':
		res = FindLongest(*tree);
		printf("Самое длинное слово: %s\n", res->word);
		res = FindShortest(*tree);
		printf("Самое короткое слово: %s\n", res->word);
		break;
	case 'd':
		a = FindDepth(*tree);
		printf("Глубина дерева равна: %d\n", a);
		break;
	case 'p':
		PrintTree(*tree, 0);
		break;
	case 'n':
		printf("Введите n\n");
		count = scanf("%d", &a);
		if (count != 1) {
			printf("Некорректный ввод\n");
			return INC_INP_DATA;
		}
		status = PrintFirstN(v, a);
		if (status != SUCCESS) {
			printf("Неккоректное n\n");
			return INC_INP_DATA;
		}
		break;
	case 'u':
		printf("Введите имя файла для сохранения\n");
		count = scanf("%s", word);
		if (count != 1) {
			printf("Некорректный ввод\n");
			return INC_INP_DATA;
		}
		file = fopen(word, "w");
		if (file == NULL) {
			printf("Некорректный файл\n");
			return INC_INP_DATA;
		}
		SaveTreeToFile(*tree, file);
		printf("Дерево успешно сохранено в: %s\n", word);
		
		fclose(file);
		break;
	case 'l':
		printf("Введите имя файла для загрузки\n");
		count = scanf("%s", word);
		if (count != 1) {
			printf("Некорректный ввод\n");
			return INC_INP_DATA;
		}
		file = fopen(word, "r");
		if (file == NULL) {
			printf("Некорректный файл\n");
			return INC_INP_DATA;
		}
		ClearTree(*tree);
		*tree = NULL;
		vector_destroy(v);
		vector_create(v, 10);
		status = LoadTreeFromFile(tree, file, v);
		if (status == SUCCESS) {
			printf("Дерево успешно загружено из %s\n", word);
		}
		else {
			printf("Ошибка загрузки файла\n");
			return INC_INP_DATA;
		}
		break;
	default:
		return INC_FLAG;
	}
	return SUCCESS;
}

kErrors StartInteraction(tnode* tree, vector* nodes) {
	char c = 'a', cn = 'a';
    char word[1024];
    int n;

	while (!(c == 'e' && cn == '\n')) {
		int count = fscanf(stdin, "%c", &c);
		count += fscanf(stdin, "%c", &cn);
		if (cn != '\n') {
			printf("Некорректный формат ввода\n");
			ClearBuffer();
			continue;
		}
		if (count != 2) {
			printf("Неккоректный флаг\n");
			ClearBuffer();
		}

		kErrors status = ProccessFlag(c, tree, nodes);
		if (status != INC_INP_DATA && status != SUCCESS && status != INC_FLAG) {
			//DestroyMails(post.mails, post.mail_amount);
			return ProccessError(status);
		}
		else {
			switch (status) {
			case INC_FLAG:
				printf("Incorrect flag\n");
				break;
			}
		}
		fseek(stdin, 0, SEEK_END);
	}
	return SUCCESS;
}

int main(int argsc, char** args)
{
	setlocale(LC_ALL, "RU");
	char** copied_args = (char**)malloc(sizeof(char*) * argsc);
	if (copied_args == NULL) {
		return MEM_ALLOC_ERR;
	}
	kErrors status = CopyArgs(copied_args, args, argsc);
	if (status != SUCCESS) {
		return status;
	}
	CheckInput(argsc, copied_args);
	if (status != SUCCESS) {
		return ProccessError(status);
	}
	FILE* file = fopen(args[1], "r");
	if (file == NULL) {
		return ProccessError(INC_FILE);
	}

	tnode* tree = NULL;
	tnode** nodes = (tnode**)malloc(sizeof(tnode*) * 10);

	vector v;
	bool a = vector_create(&v, 10);

	if (a == false) {
		FreeStringArray(copied_args, argsc);
		return MEM_ALLOC_ERR;
	}


	ReadTreeFromFile(file, &tree, copied_args + 2, argsc - 2, &v);
	qsort(v.buffer, v.size, sizeof(tnode*), TnodeCompCount);
	fclose(file);

	status = StartInteraction(&tree, &v);

	FreeStringArray(copied_args, argsc);
	return ProccessError(status);
}

