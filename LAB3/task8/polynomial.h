#pragma once
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
kErrors evaluatePolynomial(Polynomial* poly, long double x, long double*);
kErrors differentiatePolynomial(Polynomial* poly, Polynomial*);
kErrors composePolynomials(Polynomial* p1, Polynomial* p2, Polynomial*);
void freePolynomial(Polynomial* poly);
void printPolynomial(Polynomial* poly);