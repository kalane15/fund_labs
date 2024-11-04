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

typedef struct Term {
	int coefficient;
	int exponent;
	struct Term* next;
} Term;

typedef struct Polynomial {
	Term* head;
} Polynomial;

Polynomial createPolynomial();
bool addTerm(Polynomial* poly, int coefficient, int exponent);
kErrors addPolynomials(Polynomial* p1, Polynomial* p2, Polynomial*);
kErrors subtractPolynomials(Polynomial* p1, Polynomial* p2, Polynomial*);
kErrors multiplyPolynomials(Polynomial* p1, Polynomial* p2, Polynomial*);
kErrors dividePolynomials(Polynomial* p1, Polynomial* p2, Polynomial* remainder, Polynomial*);
kErrors evaluatePolynomial(Polynomial* poly, int x, long long*);
kErrors differentiatePolynomial(Polynomial* poly, Polynomial*);
kErrors composePolynomials(Polynomial* p1, Polynomial* p2, Polynomial*);
void freePolynomial(Polynomial* poly);
void printPolynomial(Polynomial* poly);
kErrors parsePolynomial(const char* str, Polynomial* out);
kErrors parseFile(const char* filename);

int main(int argc, char** args) {
	const char* filename = "D:\\univer\\fund_labs\\fund_labs\\LAB3\\task8\\in.txt";
	
	//const char* filename = "in.txt";
	
	return ProccessError(parseFile(filename));
}

// Create a new polynomial
Polynomial createPolynomial() {
	Polynomial poly;
	poly.head = NULL;
	return poly;
}

// Add a term to the polynomial
bool addTerm(Polynomial* poly, int coefficient, int exponent) {
	if (coefficient == 0) return true;  // Ignore zero coefficients

	

	// Insert in sorted order (by exponent, descending)
	if (!poly->head || poly->head->exponent < exponent) {
		Term* newTerm = (Term*)malloc(sizeof(Term));
		if (newTerm == NULL) {
			return false;
		}
		newTerm->coefficient = coefficient;
		newTerm->exponent = exponent;
		newTerm->next = NULL;
		newTerm->next = poly->head;
		poly->head = newTerm;
	} else {
		Term* current = poly->head;
		Term* prev = current;
		while (current->next && current->next->exponent >= exponent) {
			prev = current;
			current = current->next;
		}
		if (current->exponent == exponent) {
			current->coefficient += coefficient;  // Combine like terms
			if (current->coefficient == 0) {
				Term* temp = current;
				prev->next = current->next;
				if (current == poly->head) {
					poly->head = current->next;
				}
				free(current);
			}
		} else {
			Term* newTerm = (Term*)malloc(sizeof(Term));
			if (newTerm == NULL) {
				return false;
			}
			newTerm->coefficient = coefficient;
			newTerm->exponent = exponent;
			newTerm->next = NULL;
			newTerm->next = current->next;
			current->next = newTerm;
		}
	}
	return true;
}

// Print the polynomial
void printPolynomial(Polynomial* poly) {
	Term* current = poly->head;
	if (current == NULL) {
		printf("0\n");
		return;
	}
	while (current) {
		if (current->coefficient != 0) {
			if (current != poly->head && current->coefficient > 0) {
				printf("+");
			}
			printf("%d", current->coefficient);
			if (current->exponent > 0) {
				printf("x^%d ", current->exponent);
			}
		}
		current = current->next;
	}
	printf("\n");
}

// Add two polynomials
kErrors addPolynomials(Polynomial* p1_inp, Polynomial* p2, Polynomial* out) {	
	Polynomial p = createPolynomial();
	Polynomial* p1 = &p;
	if (copyPolynoms(p1_inp, p1) != SUCCESS) {
		return MEM_ALLOC_ERR;
	}
	if (out->head != NULL) {
		freePolynomial(out);
	}
	*out = createPolynomial();
	Term* current = p1->head;
	while (current) {
		if (!addTerm(out, current->coefficient, current->exponent)) {
			return MEM_ALLOC_ERR;
		}
		current = current->next;
	}
	current = p2->head;
	while (current) {
		if (!addTerm(out, current->coefficient, current->exponent)) {
			return MEM_ALLOC_ERR;
		}
		current = current->next;
	}
	freePolynomial(&p);
	return SUCCESS;
}

// Subtract two polynomials
kErrors subtractPolynomials(Polynomial* p1, Polynomial* p2, Polynomial* out) {
	if (out->head != NULL) {
		freePolynomial(out);
	}
	*out = createPolynomial();
	Term* current = p1->head;
	while (current) {
		if (!addTerm(out, current->coefficient, current->exponent)) {
			return MEM_ALLOC_ERR;
		}
		current = current->next;
	}
	current = p2->head;
	while (current) {
		if (!addTerm(out, -current->coefficient, current->exponent)) {
			return MEM_ALLOC_ERR;
		}
		current = current->next;
	}

	return SUCCESS;
}

// Multiply two polynomials
kErrors multiplyPolynomials(Polynomial* p1, Polynomial* p2, Polynomial* out) {
	Polynomial temp = createPolynomial();
	if (copyPolynoms(p1, &temp) != SUCCESS) {
		return MEM_ALLOC_ERR;
	}
	freePolynomial(out);
	*out = createPolynomial();
	Term* t1 = temp.head;
	while (t1) {
		Term* t2 = p2->head;
		while (t2) {
			if (!addTerm(out, t1->coefficient * t2->coefficient, t1->exponent + t2->exponent)) {
				return MEM_ALLOC_ERR;
			}
			t2 = t2->next;
		}
		t1 = t1->next;
	}
	freePolynomial(&temp);
	return SUCCESS;
}

// Evaluate a polynomial at a point x
kErrors evaluatePolynomial(Polynomial* poly, int x, long long* out) {
	long long result = 0;
	Term* current = poly->head;
	while (current) {
		ll t = llabs(current->coefficient * (int)pow(x, current->exponent));
		ll t2 = LLONG_MAX - t;
		if (llabs(result) > llabs(t2)) {
			return ERR_OVERFLOW;
		}
		result += current->coefficient * (int)pow(x, current->exponent);
		current = current->next;
	}
	*out = result;
	return SUCCESS;
}

// Differentiate a polynomial
kErrors differentiatePolynomial(Polynomial* poly, Polynomial* out) {
	Polynomial temp = createPolynomial();
	Term* current = poly->head;
	while (current) {
		if (current->exponent > 0) {
			if (!addTerm(&temp, current->coefficient * current->exponent, current->exponent - 1)) {
				return MEM_ALLOC_ERR;
			}
		}
		current = current->next;
	}
	*out = temp;
	return SUCCESS;
}

kErrors powPolynom(Polynomial* p, int pow, Polynomial* out) {
	freePolynomial(out);
	*out = createPolynomial();
	if (pow == 0) {
		if (!addTerm(out, 1, 0)) {
			return MEM_ALLOC_ERR;
		}
		return SUCCESS;;
	}

	if (!addTerm(out, 1, 0)) {
		return MEM_ALLOC_ERR;
	}
	for (int i = 0; i < pow; i++) {
		if (multiplyPolynomials(out, p, out) != SUCCESS) {
			return MEM_ALLOC_ERR;
		}
	}
	return SUCCESS;
}

Polynomial multipleByConst(int cnst, Polynomial p) {
	Term* cur = p.head;
	while (cur != NULL) {
		cur->coefficient *= cnst;
		cur = cur->next;
	}
	return p;
}

// Compose two polynomials
kErrors composePolynomials(Polynomial* p1, Polynomial* p2, Polynomial* out) {  // p1(p2)
	Polynomial result = createPolynomial();
	Term* current = p1->head;
	Polynomial temp = createPolynomial();
	kErrors status = SUCCESS;
	while (current) {
		status = powPolynom(p2, current->exponent, &temp);
		if (status != SUCCESS) {
			freePolynomial(&temp);
			return status;
		}
		temp = multipleByConst(current->coefficient, temp);
		addPolynomials(&result, &temp, &result);
		current = current->next;
	}
	status = copyPolynoms(&result, out);
	freePolynomial(&result);
	freePolynomial(&temp);
	return status;
}

// Free memory allocated for a polynomial
void freePolynomial(Polynomial* poly) {
	Term* current = poly->head;
	while (current) {
		Term* temp = current;
		current = current->next;
		free(temp);
	}
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

kErrors copyPolynoms(Polynomial* from, Polynomial* to) {
	if (to->head != NULL) {
		freePolynomial(to);
	}
	*to = createPolynomial();
	Term* current = from->head;
	while (current) {
		if (!addTerm(to, current->coefficient, current->exponent)) {
			return MEM_ALLOC_ERR;
		}
		current = current->next;
	}
	return SUCCESS;
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
				int x;
				status = StringToInt(arg1, &x);
				if (status != SUCCESS) {
					freePolynomial(&summator);
					freePolynomial(&current);
					fclose(file);
					free(line);
					free(buffer);
					return status;
				}
				
				if (status != SUCCESS) {
					 freePolynomial(&summator);
					 freePolynomial(&current);
					fclose(file);
					free(line);
					free(buffer);
					return status;
				}

				long long int result;
				status = evaluatePolynomial(&summator, x, &result);
				if (status != SUCCESS) {
					freePolynomial(&summator);
					freePolynomial(&current);
					fclose(file);
					free(line);
					free(buffer);
					return status;
				}
				printf("Result of evaluation at %d: %lld\n", x, result);

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
						// printPolynomial(summator);
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
			status = INC_NUM_OF_ARGS;
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

kErrors parsePolynomial(const char* str, Polynomial* out) {
	Polynomial poly = createPolynomial();
	const char* p = str;
	int coefficient = 1;
	int exponent = 0;
	bool readingCoefficient = true;  // Считываем коэффициент
	int len = strlen(str);
	if (len == 0) {
		return INC_ARGS;
	}
	int sign = 1;
	kErrors status = SUCCESS;
	for (int p = 0; p < len;) {
		if (str[p] == '+' || str[p] == '-') {
			if (p > 0 && str[p - 1] == '-' || p > 0 && str[p - 1] == '+' || p == len - 1) {
				return INC_ARGS;
			}

			// Если встретили знак, сохраняем коэффициент и начинаем новый терм
			if (!readingCoefficient) {
				coefficient *= sign;
				addTerm(&poly, coefficient, exponent);
			}
			sign = (str[p] == '-') ? -1 : 1;  // Устанавливаем знак коэффициента
			coefficient = 1;
			readingCoefficient = true;  // Считываем новый коэффициент
			p++;
		} else if (isdigit(str[p]) || str[p] == 'x') {
			if (str[p] == 'x') {
				if (p > 0 && (!isdigit(str[p - 1]) && str[p - 1] != '-' && str[p - 1] != '+' && str[p-1] != ' ' && str[p-1] != '\t')) {
					return INC_ARGS;
				}
				// Если встретили 'x', проверяем, есть ли степень
				if (str[p + 1] == '^') {
					p += 2;  // Пропускаем 'x^'
					exponent = atoi(&str[p]);
					while (isdigit(str[p])) p++;  // Пропускаем цифры степени
				} else {
					exponent = 1;  // Степень по умолчанию
					p++;           // Пропускаем 'x'
				}
				readingCoefficient = false;  // Переходим к следующему терму
				if (coefficient == 0 && p == 0) {
					coefficient = 1;
				} else if (str[p - 1] == '-' || str[p - 1] == '+') {
					coefficient = str[p - 1] == '-' ? -1 : 1;
				}
			} else {
				// Считываем коэффициент
				char* temp = (char*)malloc(BUFSIZ);
				if (temp == NULL) {
					return MEM_ALLOC_ERR;
				}
				strcpy(temp, "");
				int i = p;
				while (isdigit(str[i])) {
					strncat(temp, &str[i], 1);
					i++;
				}

				status = StringToInt(temp, &coefficient);
				if (status != SUCCESS) {
					return status;
				}
				free(temp);
				while (isdigit(str[p])) p++;
			}
		} else if (str[p] == ' ' || str[p] == '\t') {
			p++;
		}
		else {
			return INC_ARGS;
		}
	}

	// Добавляем последний терм, если он есть
	if (!readingCoefficient) {
		coefficient *= sign;
		addTerm(&poly, coefficient, exponent);
	} else {
		coefficient *= sign;
		addTerm(&poly, coefficient, 0);
	}
	*out = poly;
	return SUCCESS;
}

kErrors dividePolynomials(Polynomial* dividend, Polynomial* divisor, Polynomial* remainder, Polynomial* out) {
	// Проверяем, не равен ли делитель нулевому многочлену
	if (!divisor || !divisor->head) {
		fprintf(stderr, "Error: Division by zero polynomial.\n");
		Polynomial res;
		res.head = NULL;
		return INC_ARGS;
	}
	kErrors status = SUCCESS;
	*out = createPolynomial();          // Многочлен-частное
	Polynomial currentRemainder = createPolynomial();  // Временный остаток

	// Копируем делимое в остаток
	Term* currTerm = dividend->head;
	while (currTerm) {
		if (!addTerm(&currentRemainder, currTerm->coefficient, currTerm->exponent)) {
			freePolynomial(&currentRemainder);
			return MEM_ALLOC_ERR;
		}
		currTerm = currTerm->next;
	}

	// Основной цикл деления
	while (currentRemainder.head && currentRemainder.head->exponent >= divisor->head->exponent) {
		// Находим коэффициент и степень для текущего терма частного
		int coeff = currentRemainder.head->coefficient / divisor->head->coefficient;
		int exp = currentRemainder.head->exponent - divisor->head->exponent;
		int exp_prev = currentRemainder.head->exponent;
		// Создаем новый терм для частного и добавляем его
		if (!addTerm(out, coeff, exp)) {
			freePolynomial(&currentRemainder);
			return MEM_ALLOC_ERR;
		}		

		// Создаем многочлен для вычитания
		Polynomial temp = createPolynomial();
		if (!addTerm(&temp, coeff, exp)) {
			freePolynomial(&currentRemainder); // Добавляем терм частного
			return MEM_ALLOC_ERR;
		}

		Polynomial toSubtract = createPolynomial();
		status = multiplyPolynomials(&temp, divisor, &toSubtract);  // Умножаем на делитель
		if (status != SUCCESS) {
			freePolynomial(&toSubtract);
			freePolynomial(&temp);
			freePolynomial(&currentRemainder);
			return MEM_ALLOC_ERR;
		}

		// Вычитаем результат из текущего остатка
		status = copyPolynoms(&currentRemainder, &temp);
		if (status != SUCCESS) {
			freePolynomial(&toSubtract);
			freePolynomial(&temp);
			freePolynomial(&currentRemainder);
			return status;
		}

		status = subtractPolynomials(&temp, &toSubtract, &currentRemainder);
		if (status != SUCCESS) {
			freePolynomial(&toSubtract);
			freePolynomial(&temp);
			freePolynomial(&currentRemainder);
			return MEM_ALLOC_ERR;
		}
		if (currentRemainder.head->exponent == exp_prev && currentRemainder.head->coefficient != 0) {
			return INC_DIV;
		}
		// Освобождаем временные многочлены
		 freePolynomial(&temp);
		 freePolynomial(&toSubtract);
	}

	// Устанавливаем остаток
	*remainder = currentRemainder;
	return SUCCESS;  // Возвращаем частное
}