#define _CRT_SECURE_NO_WARNINGS
#define _XOPEN_SOURCE
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
#include "string.h"
#include <time.h>
#include "myerrors.h"

typedef struct Term {
    int coefficient;
    int exponent;
    struct Term* next;
} Term;

typedef struct Polynomial {
    Term* head;
} Polynomial;

Polynomial* createPolynomial();
void addTerm(Polynomial* poly, int coefficient, int exponent);
Polynomial* addPolynomials(Polynomial* p1, Polynomial* p2);
Polynomial* subtractPolynomials(Polynomial* p1, Polynomial* p2);
Polynomial* multiplyPolynomials(Polynomial* p1, Polynomial* p2);
Polynomial* dividePolynomials(Polynomial* p1, Polynomial* p2, Polynomial** remainder);
int evaluatePolynomial(Polynomial* poly, int x);
Polynomial* differentiatePolynomial(Polynomial* poly);
Polynomial* composePolynomials(Polynomial* p1, Polynomial* p2);
void freePolynomial(Polynomial* poly);
void printPolynomial(Polynomial* poly);
kErrors parsePolynomial(const char* str, Polynomial** out);
void processCommand(char* command, Polynomial** accumulator);
kErrors parseFile(const char* filename);

int main() {
    const char* filename = "in.txt";
    return ProccessError(parseFile(filename));
}

// Create a new polynomial
Polynomial* createPolynomial() {
    Polynomial* poly = (Polynomial*)malloc(sizeof(Polynomial));
    poly->head = NULL;
    return poly;
}

// Add a term to the polynomial
void addTerm(Polynomial* poly, int coefficient, int exponent) {
    if (coefficient == 0) return; // Ignore zero coefficients

    Term* newTerm = (Term*)malloc(sizeof(Term));
    newTerm->coefficient = coefficient;
    newTerm->exponent = exponent;
    newTerm->next = NULL;

    // Insert in sorted order (by exponent, descending)
    if (!poly->head || poly->head->exponent < exponent) {
        newTerm->next = poly->head;
        poly->head = newTerm;
    }
    else {
        Term* current = poly->head;
        while (current->next && current->next->exponent >= exponent) {
            current = current->next;
        }
        if (current->exponent == exponent) {
            current->coefficient += coefficient; // Combine like terms
            free(newTerm);
        }
        else {
            newTerm->next = current->next;
            current->next = newTerm;
        }
    }
}

// Print the polynomial
void printPolynomial(Polynomial* poly) {
    Term* current = poly->head;
    if (current == NULL) {
        printf("0\n");
        return;
    }
    while (current) {
        if (current != poly->head && current->coefficient > 0) {
            printf("+");
        }
        printf("%dx^%d ", current->coefficient, current->exponent);
        current = current->next;
    }
    printf("\n");
}

// Add two polynomials
Polynomial* addPolynomials(Polynomial* p1, Polynomial* p2) {
    Polynomial* result = createPolynomial();
    Term* current = p1->head;
    while (current) {
        addTerm(result, current->coefficient, current->exponent);
        current = current->next;
    }
    current = p2->head;
    while (current) {
        addTerm(result, current->coefficient, current->exponent);
        current = current->next;
    }
    return result;
}

// Subtract two polynomials
Polynomial* subtractPolynomials(Polynomial* p1, Polynomial* p2) {
    Polynomial* result = createPolynomial();
    Term* current = p1->head;
    while (current) {
        addTerm(result, current->coefficient, current->exponent);
        current = current->next;
    }
    current = p2->head;
    while (current) {
        addTerm(result, -current->coefficient, current->exponent);
        current = current->next;
    }
    return result;
}

// Multiply two polynomials
Polynomial* multiplyPolynomials(Polynomial* p1, Polynomial* p2) {
    Polynomial* result = createPolynomial();
    Term* t1 = p1->head;
    while (t1) {
        Term* t2 = p2->head;
        while (t2) {
            addTerm(result, t1->coefficient * t2->coefficient, t1->exponent + t2->exponent);
            t2 = t2->next;
        }
        t1 = t1->next;
    }
    return result;
}

// Evaluate a polynomial at a point x
int evaluatePolynomial(Polynomial* poly, int x) {
    int result = 0;
    Term* current = poly->head;
    while (current) {
        result += current->coefficient * (int)pow(x, current->exponent);
        current = current->next;
    }
    return result;
}

// Differentiate a polynomial
Polynomial* differentiatePolynomial(Polynomial* poly) {
    Polynomial* result = createPolynomial();
    Term* current = poly->head;
    while (current) {
        if (current->exponent > 0) {
            addTerm(result, current->coefficient * current->exponent, current->exponent - 1);
        }
        current = current->next;
    }
    return result;
}

// Compose two polynomials
Polynomial* composePolynomials(Polynomial* p1, Polynomial* p2) {
    Polynomial* result = createPolynomial();
    Term* current = p1->head;
    while (current) {
        int coeff = current->coefficient;
        int exp = current->exponent;
        Polynomial* termPoly = createPolynomial();
        addTerm(termPoly, coeff, exp);
        Polynomial* evaluatedPoly = multiplyPolynomials(termPoly, p2);
        result = addPolynomials(result, evaluatedPoly);
        freePolynomial(termPoly);
        freePolynomial(evaluatedPoly);
        current = current->next;
    }
    return result;
}

// Free memory allocated for a polynomial
void freePolynomial(Polynomial* poly) {
    Term* current = poly->head;
    while (current) {
        Term* temp = current;
        current = current->next;
        free(temp);
    }
    free(poly);
}

// Process a command from the file
void processCommand(char* command, Polynomial** accumulator) {
    char* action = strtok(command, "(");
    char* args = strtok(NULL, ")");

    if (strcmp(action, "Add") == 0) {
        // Handle Add
    }
    else if (strcmp(action, "Sub") == 0) {
        // Handle Sub
    }
    else if (strcmp(action, "Mult") == 0) {
        // Handle Mult
    }
    else if (strcmp(action, "Div") == 0) {
        // Handle Div
    }
    else if (strcmp(action, "Eval") == 0) {
        // Handle Eval
    }
    else if (strcmp(action, "Diff") == 0) {
        // Handle Diff
    }
    else if (strcmp(action, "Cmps") == 0) {
        // Handle Cmps
    }
}
void swap(void** a, void** b) {
    void* temp = *a;
    *a = *b;
    *b = temp;
}

kErrors CheckString(char* str) {
    if (str[0] != ';') {
        return NO_POINT_COMMA;
    }
    int len = strlen(str);
    for (int i = 1; i < len; i++) {
        if (str[i] != ' ') {
            if (str[i] == '%' || str[i] == '[') {
                return SUCCESS;
            }
            else {
                return EXTRA_CHARACTERS;
            }
        }
    }
}

// Parse the command file
kErrors parseFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return INC_FILE;
    }

    char line[256];
    Polynomial* summator = createPolynomial(); // Начальное значение сумматора
    Polynomial* current = createPolynomial(); // Начальное значение сумматора

    while (fgets(line, sizeof(line), file)) {
        // Удаление символа новой строки
        line[strcspn(line, "\n")] = 0;

        // Игнорирование однострочных комментариев
        if (line[0] == '%') continue;

        // Игнорирование многострочных комментариев
        if (line[0] == '[') {
            while (fgets(line, sizeof(line), file) && line[0] != ']');
            continue;
        }
        kErrors status;
        bool use_summator = false;
        // Парсинг команды и аргументов
        char* command = strtok(line, "(");
        char* args = strtok(NULL, ");");
        char* other = strtok(NULL, "");
        status = CheckString(other);
        if (status != SUCCESS) {
            return status;
        }
        if (args || strcmp(command, "Diff") == 0) {
            // Убираем лишние пробелы
            char* arg1 = strtok(args, ",");
            char* arg2 = strtok(NULL, ",");
            if (arg1 && arg2) {
                while (isspace(*arg1)) arg1++;
                while (isspace(*arg2)) arg2++;
            }

            // Обработка команд
            if (strcmp(command, "Add") == 0) {
                if (arg2 == NULL) {
                    use_summator = true;
                }

                Polynomial* p1 = NULL;
                status = parsePolynomial(arg1, &p1);
                Polynomial* p2 = NULL;
                if (use_summator) {
                    p2 = summator;
                }
                else {
                    status = (status == SUCCESS) ? parsePolynomial(arg2, &p2) : status;
                }
                if (status != SUCCESS) {
                    freePolynomial(summator);
                    freePolynomial(current);
                    fclose(file);
                    return status;
                }
                printPolynomial(p1);
                printPolynomial(p2);

                if (use_summator) {
                    summator = addPolynomials(p1, p2);
                    printPolynomial(summator);
                }
                else {
                    current = addPolynomials(p1, p2);
                    printPolynomial(current);
                }                
            }
            else if (strcmp(command, "Sub") == 0) {
                if (arg2 == NULL) {
                    use_summator = true;
                }

                Polynomial* p1 = NULL;
                status = parsePolynomial(arg1, &p1);
                Polynomial* p2 = NULL;
                if (use_summator) {
                    p2 = summator;
                }
                else {
                    status = status == SUCCESS ? parsePolynomial(arg2, &p2) : status;
                }
                if (status != SUCCESS) {
                    freePolynomial(summator);
                    freePolynomial(current);
                    fclose(file);
                    return status;
                }

               
                /*printPolynomial(p1);
                printPolynomial(p2);*/
                if (use_summator) {
                    swap((void**)&p1, (void**)&p2);
                    summator = subtractPolynomials(p1, p2);
                    printPolynomial(summator);
                }
                else {
                    current = subtractPolynomials(p1, p2);
                    printPolynomial(current);
                }
            }
            else if (strcmp(command, "Mult") == 0) {
                if (arg2 == NULL) {
                    use_summator = true;
                }

                Polynomial* p1 = NULL;
                status = parsePolynomial(arg1, &p1);
                Polynomial* p2 = NULL;
                if (use_summator) {
                    p2 = summator;
                }
                else {
                    status = status == SUCCESS ? parsePolynomial(arg2, &p2) : status;
                }
                if (status != SUCCESS) {
                    freePolynomial(summator);
                    freePolynomial(current);
                    fclose(file);
                    return status;
                }

                if (use_summator) {
                    summator = multiplyPolynomials(p1, p2);
                    printPolynomial(summator);
                }
                else {
                    current = multiplyPolynomials(p1, p2);
                    printPolynomial(current);
                }
            }
            else if (strcmp(command, "Div") == 0) {
                if (arg2 == NULL) {
                    use_summator = true;
                }
                Polynomial* remainder;

                Polynomial* p1 = NULL;
                status = parsePolynomial(arg1, &p1);
                Polynomial* p2 = NULL;
                if (use_summator) {
                    p2 = summator;
                }
                else {
                    status = status == SUCCESS ? parsePolynomial(arg2, &p2) : status;
                }
                if (status != SUCCESS) {
                    freePolynomial(summator);
                    freePolynomial(current);
                    fclose(file);
                    return status;
                }

                if (use_summator) {
                    swap((void**)&p1, (void**)&p2);
                    summator = dividePolynomials(p1, p2, &remainder);
                    printPolynomial(summator);
                }
                else {
                    current = dividePolynomials(p1, p2, &remainder);
                    printPolynomial(current);
                }
                freePolynomial(remainder);
            }
            else if (strcmp(command, "Mod") == 0) {
                if (arg2 == NULL) {
                    use_summator = true;
                }
                Polynomial* remainder;

                Polynomial* p1 = NULL;
                status = parsePolynomial(arg1, &p1);
                Polynomial* p2 = NULL;
                if (use_summator) {
                    p2 = summator;
                }
                else {
                    status = status == SUCCESS ? parsePolynomial(arg2, &p2) : status;
                }
                if (status != SUCCESS) {
                    freePolynomial(summator);
                    freePolynomial(current);
                    fclose(file);
                    return status;
                }

                if (use_summator) {
                    swap((void**)&p1, (void**)&p2);
                    dividePolynomials(p1, p2, &remainder);
                    summator = remainder;
                    printPolynomial(summator);
                }
                else {
                    dividePolynomials(p1, p2, &remainder);
                    printPolynomial(remainder);
                }
                freePolynomial(remainder);
            }
            else if (strcmp(command, "Eval") == 0) {
                use_summator = true;
                int x = atoi(arg1);
                
                Polynomial* p1 = NULL; 
                status = parsePolynomial(arg1, &p1);               
                Polynomial* p2 = NULL;
                if (use_summator) {
                    p2 = summator;
                }
                else {
                    status = status == SUCCESS ? parsePolynomial(arg2, &p2) : status;
                }
                if (status != SUCCESS) {
                    freePolynomial(summator);
                    freePolynomial(current);
                    fclose(file);
                    return status;
                }

                int result = evaluatePolynomial(summator, x);
                printf("Result of evaluation at %d: %d\n", x, result);   
               
            } else if (strcmp(command, "Diff") == 0) {
				summator = differentiatePolynomial(summator);
				printPolynomial(summator);
			} else if (strcmp(command, "Cmps") == 0) {
				/*Polynomial* p1 = NULL = (strcmp(arg1, "sum") == 0) ? summator : parsePolynomial(arg1);
				Polynomial* p2 = parsePolynomial(arg2);
				summator = composePolynomials(p1, p2);
				printPolynomial(summator);*/
            }
            else {
                return UNKNOWN_FUNC;
            }
		} else {
			return INC_NUM_OF_ARGS;
		}
	}

	freePolynomial(summator);
	freePolynomial(current);
	fclose(file);
}

kErrors parsePolynomial(const char* str, Polynomial** out) {
    Polynomial* poly = createPolynomial();
    const char* p = str;
    int coefficient = 1;
    int exponent = 0;
    bool readingCoefficient = true; // Считываем коэффициент
    int len = strlen(str);
    int sign = 1;
    for (int p = 0; p < len;) {
        if (str[p] == '+' || str[p] == '-') {
            if (p > 0 && str[p - 1] == '-' || p > 0 && str[p - 1] == '+' || p == len - 1) {
                return INC_ARGS;
            }

            // Если встретили знак, сохраняем коэффициент и начинаем новый терм
            if (!readingCoefficient) {
                coefficient *= sign;
                addTerm(poly, coefficient, exponent);
            }
            sign = (str[p] == '-') ? -1 : 1; // Устанавливаем знак коэффициента
            coefficient = 1;
            readingCoefficient = true; // Считываем новый коэффициент
            p++;
        }
        else if (isdigit(str[p]) || str[p] == 'x') {
            if (str[p] == 'x') {
                if (p > 0 && !isdigit(str[p - 1]) && str[p-1] != '-' && str[p-1] != '+') {
                    return INC_ARGS;
                }
                // Если встретили 'x', проверяем, есть ли степень
                if (str[p + 1] == '^') {
                    p += 2; // Пропускаем 'x^'
                    exponent = atoi(&str[p]);
                    while (isdigit(str[p])) p++; // Пропускаем цифры степени
                }
                else {
                    exponent = 1; // Степень по умолчанию                    
                    p++; // Пропускаем 'x'
                }
                readingCoefficient = false; // Переходим к следующему терму
                if (coefficient == 0 && p == 0) {
                    coefficient = 1;
                }
                else if (str[p - 1] == '-' || str[p] == '+') {
                    coefficient = str[p - 1] == '-' ? -1 : 1;
                }
            }
            else {
                // Считываем коэффициент
                coefficient = atoi(&str[p]);
                while (isdigit(str[p])) p++; // Пропускаем цифры коэффициента
            }
        }
        else {
            return INC_ARGS;
        }        
    }

    // Добавляем последний терм, если он есть
    if (!readingCoefficient) {
        coefficient *= sign;
        addTerm(poly, coefficient, exponent);
    }
    else {
        coefficient *= sign;
        addTerm(poly, coefficient, 0);
    }
    *out = poly;
    return SUCCESS;
}

Polynomial* dividePolynomials(Polynomial* dividend, Polynomial* divisor, Polynomial** remainder) {
    // Проверяем, не равен ли делитель нулевому многочлену
    if (!divisor || !divisor->head) {
        fprintf(stderr, "Error: Division by zero polynomial.\n");
        return NULL;
    }

    Polynomial* quotient = createPolynomial(); // Многочлен-частное
    Polynomial* currentRemainder = createPolynomial(); // Временный остаток

    // Копируем делимое в остаток
    Term* currTerm = dividend->head;
    while (currTerm) {
        addTerm(currentRemainder, currTerm->coefficient, currTerm->exponent);
        currTerm = currTerm->next;
    }

    // Основной цикл деления
    while (currentRemainder->head && currentRemainder->head->exponent >= divisor->head->exponent) {
        // Находим коэффициент и степень для текущего терма частного
        int coeff = currentRemainder->head->coefficient / divisor->head->coefficient;
        int exp = currentRemainder->head->exponent - divisor->head->exponent;

        // Создаем новый терм для частного и добавляем его
        addTerm(quotient, coeff, exp);

        // Создаем многочлен для вычитания
        Polynomial* temp = createPolynomial();
        addTerm(temp, coeff, exp); // Добавляем терм частного
        Polynomial* toSubtract = multiplyPolynomials(temp, divisor); // Умножаем на делитель

        // Вычитаем результат из текущего остатка
        currentRemainder = subtractPolynomials(currentRemainder, toSubtract);

        // Освобождаем временные многочлены
        freePolynomial(temp);
        freePolynomial(toSubtract);
    }

    // Устанавливаем остаток
    *remainder = currentRemainder;

    return quotient; // Возвращаем частное
}