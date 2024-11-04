#include "myerrors.h"
#include "polynomial.h"

kErrors parsePolynomial(const char* str, Polynomial* out) {
	Polynomial poly = createPolynomial();
	const char* p = str;
	int coefficient = 1;
	int exponent = 0;
	bool readingCoefficient = true;  
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

		
			if (!readingCoefficient) {
				coefficient *= sign;
				addTerm(&poly, coefficient, exponent);
			}
			sign = (str[p] == '-') ? -1 : 1;  
			coefficient = 1;
			readingCoefficient = true;  
			p++;
		}
		else if (isdigit(str[p]) || str[p] == 'x') {
			if (str[p] == 'x') {
				if (p > 0 && (!isdigit(str[p - 1]) && str[p - 1] != '-' && str[p - 1] != '+' && str[p - 1] != ' ' && str[p - 1] != '\t')) {
					return INC_ARGS;
				}
				if (str[p + 1] == '^') {
					p += 2;
					exponent = atoi(&str[p]);
					while (isdigit(str[p])) p++;  
				}
				else {
					exponent = 1;  
					p++;          
				}
				readingCoefficient = false;  
				if (coefficient == 0 && p == 0) {
					coefficient = 1;
				}
				else if (str[p - 1] == '-' || str[p - 1] == '+') {
					coefficient = str[p - 1] == '-' ? -1 : 1;
				}
			}
			else {
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
		}
		else if (str[p] == ' ' || str[p] == '\t') {
			p++;
		}
		else {
			return INC_ARGS;
		}
	}

	if (!readingCoefficient) {
		coefficient *= sign;
		addTerm(&poly, coefficient, exponent);
	}
	else {
		coefficient *= sign;
		addTerm(&poly, coefficient, 0);
	}
	*out = poly;
	return SUCCESS;
}

kErrors dividePolynomials(Polynomial* dividend, Polynomial* divisor, Polynomial* remainder, Polynomial* out) {
	if (!divisor || !divisor->head) {
		fprintf(stderr, "Error: Division by zero polynomial.\n");
		Polynomial res;
		res.head = NULL;
		return INC_ARGS;
	}
	kErrors status = SUCCESS;
	*out = createPolynomial();         
	Polynomial currentRemainder = createPolynomial(); 

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
		int coeff = currentRemainder.head->coefficient / divisor->head->coefficient;
		int exp = currentRemainder.head->exponent - divisor->head->exponent;
		int exp_prev = currentRemainder.head->exponent;
		if (!addTerm(out, coeff, exp)) {
			freePolynomial(&currentRemainder);
			return MEM_ALLOC_ERR;
		}

		// Создаем многочлен для вычитания
		Polynomial temp = createPolynomial();
		if (!addTerm(&temp, coeff, exp)) {
			freePolynomial(&currentRemainder);
			return MEM_ALLOC_ERR;
		}

		Polynomial toSubtract = createPolynomial();
		status = multiplyPolynomials(&temp, divisor, &toSubtract);  
		if (status != SUCCESS) {
			freePolynomial(&toSubtract);
			freePolynomial(&temp);
			freePolynomial(&currentRemainder);
			return MEM_ALLOC_ERR;
		}

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
		freePolynomial(&temp);
		freePolynomial(&toSubtract);
	}

	*remainder = currentRemainder;
	return SUCCESS;  
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

void freePolynomial(Polynomial* poly) {
	Term* current = poly->head;
	while (current) {
		Term* temp = current;
		current = current->next;
		free(temp);
	}
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
	}
	else {
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
		}
		else {
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
kErrors evaluatePolynomial(Polynomial* poly, long double x, long double* out) {
	long double result = 0;
	Term* current = poly->head;
	while (current) {
		long double t = llabs((long double)current->coefficient * powl(x, (long double)current->exponent));
		long double t2 = LDBL_MAX - t;
		if (fabsl(result) > fabsl(t2)) {
			return ERR_OVERFLOW;
		}
		result += current->coefficient * powl(x, current->exponent);
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