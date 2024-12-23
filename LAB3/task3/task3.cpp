﻿#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include <climits>
#include <cstring>
#include <locale>

#include "limits.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"
#include "myerrors.h"
#include "string.h"
#include "windows.h"
#define uint unsigned int;
 

typedef enum kOpts {
	ASCENDING_ORDER,
	DESCENDING_ORDER
} kOpts;

typedef struct Employee {
	unsigned int id;
	char* name;
	char* surname;
	double salary;
} Employee;

kErrors ParseInput(int argc, char** args, kOpts* opt, char** file_name1, char** file_name2) {
	if (argc != 4) {
		return INC_NUM_OF_ARGS;
	}

	if (strlen(args[2]) != 2) {
		return INC_FLAG;
	}

	if (args[2][0] == '/' || args[2][0] == '-') {
		char option = args[2][1];

		switch (option) {
		case 'a':
			*opt = ASCENDING_ORDER;
			break;
		case 'd':
			*opt = DESCENDING_ORDER;
			break;
		default:
			return INC_FLAG;
		}
	}
	else {
		return INC_FLAG;
	}

	char path1[1024];
	char path2[1024];
	*file_name1 = args[1];
	*file_name2 = args[3];

	int status = GetFullPathNameA(*file_name1, 1024, path1, NULL);
	if (status == 1024 || status == 0) {
		return INC_PATH;
	}
	status = GetFullPathNameA(*file_name2, 1024, path2, NULL);
	if (status == 1024 || status == 0) {
		return INC_PATH;
	}

	if (strcmp(path1, path2) == 0) {
		return INC_PATH;
	}
	return SUCCESS;
}

void ClearEmloyees(Employee* employees, int size) {
	for (int i = 0; i < size; i++) {
		free(employees[i].surname);
		free(employees[i].name);
	}
	free(employees);
}

bool CheckSurname(char* surname) {
	int n = strlen(surname);
	if (n == 0){
		return false;
	}

	for (int i = 0; i < n; i++) {
		if (!('a' <= tolower(surname[i]) && tolower(surname[i]) <= 'z')) {
			return false;
		}
	}
	return true;
}

kErrors GetInfo(Employee** employees, char* filename, int* size, int start_size) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		return INC_FILE;
	}

	int buffer_size = start_size;
	int i = 0;
	Employee cur;
	int count;
	while (!feof(file)) {
		count = 0;
		cur.surname = (char*)malloc(1024);
		cur.name = (char*)malloc(1024);
		count = fscanf(file, "%u %s %s %lf\n", &(cur.id), cur.name, cur.surname, &(cur.salary));
		if (count != 4) {
			free(cur.surname);
			free(cur.name);
			ClearEmloyees(*employees, i);			
			fclose(file);
			return INC_INP_DATA;
		}

		if (!CheckSurname(cur.surname)) {
			free(cur.surname);
			free(cur.name);
			ClearEmloyees(*employees, i);
			fclose(file);
			return INC_INP_DATA;
		}

		if (buffer_size == i) {
			Employee* temp = *employees;
			*employees = (Employee*)realloc(*employees, sizeof(Employee) * buffer_size * 2);
			buffer_size *= 2;
			if (employees == NULL) {
				*employees = temp;
				free(cur.surname);
				free(cur.name);
				ClearEmloyees(*employees, i);
				fclose(file);
				return MEM_ALLOC_ERR;
			}
		}

		(*employees)[i] = cur;
		i++;
	}

	*size = i;
	fclose(file);
	return SUCCESS;
}

int comparator(const void* a, const void* b) {
	Employee* e1 = (Employee*)a;
	Employee* e2 = (Employee*)b;
	if (e1->salary != e2->salary) {
		return e1->salary < e2->salary ? -1 : 1;
	} 
	if (strcmp(e1->surname, e2->surname) != 0) {
		return strcmp(e1->surname, e2->surname);
	} 
	if (strcmp(e1->name, e2->name) != 0) {
		return strcmp(e1->name, e2->name);
	}
	if (e1->id != e2->id) {
		return e1->id < e2->id ? -1 : 1;
	}
	return 0;
}

void PrintEmployees(FILE* out, Employee* employees, int size) {
	for (int i = 0; i < size; i++) {
		fprintf(out, "%u %s %s %lf\n", employees[i].id, employees[i].name, employees[i].surname, employees[i].salary);
	}
}

void ReverseArray(Employee* arr, int n) {
	for (int i = 0; i < n / 2; ++i)
	{
		Employee temp = arr[i];
		arr[i] = arr[n - i - 1];
		arr[n - i - 1] = temp;
	}
}

int main(int argc, char** args)
{
	int start_size = 1;
	kOpts opt;
	char* inp_file;
	char* out_file;
	kErrors status = ParseInput(argc, args, &opt, &inp_file, &out_file);
	if (status != 0) {
		return ProccessError(status);
	}

	Employee* employees = (Employee*)malloc(sizeof(Employee) * start_size);
	if (employees == NULL) {
		return ProccessError(MEM_ALLOC_ERR);
	}

	int size;
	status = GetInfo(&employees, inp_file, &size, start_size);
	if (status != SUCCESS) {
		return ProccessError(status);
	}	
	qsort(employees, size, sizeof(Employee), comparator);

	if (opt == DESCENDING_ORDER) {
		ReverseArray(employees, size);
	}

	FILE* out = fopen(out_file, "w");

	if (out == NULL) {
		return ProccessError(INC_FILE);
	}

	PrintEmployees(out, employees, size);
	ClearEmloyees(employees, size);
	fclose(out);
}

