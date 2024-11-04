#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "limits.h"
#include "locale.h"
#include "myerrors.h"
#include "mylib.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "float.h"
#include "polynomial.h"

kErrors parsePolynomial(const char* str, Polynomial* out);
kErrors parseFile(const char* filename);

int main(int argc, char** args) {
	const char* filename = "D:\\univer\\fund_labs\\fund_labs\\LAB3\\task8\\in.txt";
	
	//const char* filename = "in.txt";
	
	return ProccessError(parseFile(filename));
}

void swap(void** a, void** b) {
	void* temp = *a;
	*a = *b;
	*b = temp;
}

kErrors CheckString(char* str) {
	if (str == NULL) {
		return SUCCESS;
	}
	if (str[0] != ';') {
		return NO_POINT_COMMA;
	}
	int len = strlen(str);
	for (int i = 1; i < len; i++) {
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n') {
			if (str[i] == '%' || str[i] == '[') {
				return SUCCESS;
			} else {
				return EXTRA_CHARACTERS;
			}
		}
	}
	return SUCCESS;
}

void strip(char* string) {
	if (string == NULL) {
		return;
	}
	char* cur = string;
	while (*cur == ' ' || *cur == '\t') {
		cur++;
	}
	strcpy(string, cur);
}



// Parse the command file
kErrors parseFile(const char* filename) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		return INC_FILE;
	}
	kErrors status = SUCCESS;
	char* line = (char*)malloc(1024);
	char* start_line = line;
	if (line == NULL) {
		return MEM_ALLOC_ERR;
	}
	char* buffer = (char*)malloc(1024);
	if (buffer == NULL) {
		free(line);
		return MEM_ALLOC_ERR;
	}
	Polynomial summator = createPolynomial();  
	Polynomial current = createPolynomial();   
	bool is_in_comm = false;
	while (fgets(buffer, 1024, file)) {
		line = start_line;
		strcpy(line, buffer);
		// Удаление символа новой строки
		line[strcspn(line, "\n")] = 0;
		
		// Игнорирование однострочных комментариев
		if (line[0] == '%') continue;

		// Игнорирование многострочных комментариев
		
		bool foundl= false;
		bool foundr = false;
		bool open_com = false;
		for (int i = 0; i < strlen(buffer); i++) {
			if (buffer[i] == '[') {
				line[i] = '\0';
				if (is_in_comm) {
					status = INC_COMMENTS;
					break;
				}
				else {
					is_in_comm = true;
					open_com = true;
					for (int j = i + 1; j < strlen(buffer); j++) {
						if (buffer[j] == ']') {
							is_in_comm = false;
							open_com = false;
							for (int k = i + j + 1; k < strlen(buffer); k++) {
								if (buffer[k] == '[') {
									is_in_comm = true;
									open_com = true;
									i = k + 1;
									break;
								}								
								strncat(line, &buffer[k], 1);
							}							
							break;
						}
						if (buffer[j] == '[') {
							status = INC_COMMENTS;
							break;
						}
					}
				}
			}

			if (buffer[i] == ']') {
				if (is_in_comm) {
					is_in_comm = false;
					line += i + 1;
				}
			}
		}
		if (status != SUCCESS) {
			break;
		}
		strip(line);
		if (is_in_comm && !open_com || strlen(line) == 0 || *line == '\n') {
			continue;
		}



		for (int i = 0; i < strlen(line) - 1; i++) {
			if (line[i] == '(') {
				if (!foundl) {
					foundl = true;
				}
				else {
					foundl = false;
					break;
				}
			}
			else if (line[i] == ')') {
				if (line[i + 1] == ';') {
					if (!foundr) {
						foundr = true;
					}
					else {
						foundr = false;
						break;
					}
				}
			}
		}

		if ((!foundl || !foundr) && !is_in_comm) {
			status = INC_SYNTAX;
			break;
		}

		bool use_summator = false;
		// Парсинг команды и аргументов
		char* command = strtok(line, "(");
		char* args = strtok(NULL, ");");
		char* other = strtok(NULL, "");
		strip(command);
		strip(args);
		status = CheckString(other);
		if (status != SUCCESS) {
			return status;
		}
		current = createPolynomial();
		if (args || strcmp(command, "Diff") == 0) {
			// Убираем лишние пробелы
			char* arg1 = strtok(args, ",");
			char* arg2 = strtok(NULL, ",");
			if (arg1 && arg2) {
				while (isspace(*arg1)) arg1++;
				while (isspace(*arg2)) arg2++;
			}

			// Обработка команд

			if (strcmp(command, "Eval") == 0) {
				use_summator = true;
				if (arg2 != NULL) {
					status = INC_NUM_OF_ARGS;
				}
				if (status != SUCCESS) {
					freePolynomial(&summator);
					freePolynomial(&current);
					fclose(file);
					free(line);
					free(buffer);
					return status;
				}
				long double x;
				if (!CheckIfDoubleCorrect(arg1)) {
					status = INC_ARGS;
					freePolynomial(&summator);
					freePolynomial(&current);
					fclose(file);
					free(line);
					free(buffer);
					return status;
				}
				x = ParseDouble(arg1);

				long double result;
				status = evaluatePolynomial(&summator, x, &result);
				if (status != SUCCESS) {
					freePolynomial(&summator);
					freePolynomial(&current);
					fclose(file);
					free(line);
					free(buffer);
					return status;
				}
				printf("Result of evaluation at %Lf: %Lf\n", x, result);

			} else if (strcmp(command, "Diff") == 0) {
				status = differentiatePolynomial(&summator, &summator);
				printPolynomial(&summator);
			} else {
				if (arg2 == NULL) {
					use_summator = true;
				}

				Polynomial p1 = createPolynomial();
				status = parsePolynomial(arg1, &p1);
				Polynomial p2 = createPolynomial();
				if (use_summator) {
					status = copyPolynoms(&summator, &p2);
				} else {
					status = (status == SUCCESS) ? parsePolynomial(arg2, &p2) : status;
				}
				if (status != SUCCESS) {
					freePolynomial(&summator);
					freePolynomial(&current);
					fclose(file);
					return status;
				}
				Polynomial remainder = createPolynomial();

				if (strcmp(command, "Add") == 0) {
					// printPolynomial(p1);
					// printPolynomial(p2);
					if (use_summator) {
						status = addPolynomials(&p1, &p2, &current);
						status = copyPolynoms(&current, &summator);
						printPolynomial(&summator);
					} else {
						status = addPolynomials(&p1, &p2, &current);
						printPolynomial(&current);
					}
				} else if (strcmp(command, "Sub") == 0) {
					if (use_summator) {
						swap((void**)&p1, (void**)&p2);
						status = subtractPolynomials(&p1, &p2, &current);
						status = copyPolynoms(&current, &summator);
						printPolynomial(&summator);
					} else {
						status = subtractPolynomials(&p1, &p2, &current);
						printPolynomial(&current);
					}
				} else if (strcmp(command, "Mult") == 0) {
					if (use_summator) {
						status = multiplyPolynomials(&p1, &p2, &summator);
						printPolynomial(&summator);
					} else {
						status = multiplyPolynomials(&p1, &p2, &current);
						printPolynomial(&current);
					}
				} else if (strcmp(command, "Div") == 0) {
					if (use_summator) {
						swap((void**)&p1, (void**)&p2);
						status = dividePolynomials(&p1, &p2, &remainder, &summator);
						printPolynomial(&summator);
					} else {
						status = dividePolynomials(&p1, &p2, &remainder, &current);
						printPolynomial(&current);
					}
					// freePolynomial(remainder);
				} else if (strcmp(command, "Mod") == 0) {
					if (use_summator) {
						swap((void**)&p1, (void**)&p2);
						status = dividePolynomials(&p1, &p2, &remainder, &current);
						summator = remainder;
						printPolynomial(&summator);
					} else {
						status = dividePolynomials(&p1, &p2, &remainder, &current);
						printPolynomial(&remainder);
					}

					// freePolynomial(remainder);
				}

				// printPolynomial(summator);

				else if (strcmp(command, "Cmps") == 0) {
					if (use_summator) {
						swap((void**)&p1, (void**)&p2);
						status = composePolynomials(&p1, &p2, &summator);
						 printPolynomial(&summator);
					} else {
						status = composePolynomials(&p1, &p2, &current);
						printPolynomial(&current);
					}
				} else {
					status = UNKNOWN_FUNC;
				}

				freePolynomial(&p1);
				freePolynomial(&p2);
				freePolynomial(&remainder);
				if (status != SUCCESS) {
					break;
				}
			}			
		} else {
			status = INC_SYNTAX;
		}
		freePolynomial(&current);
		if (status != SUCCESS) {
			break;
		}
	}
	if (is_in_comm) {
		status = INC_COMMENTS;
	}
	free(buffer);
	free(start_line);
	freePolynomial(&summator);
	fclose(file);
	return status;
}

