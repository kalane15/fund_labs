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

void processCommand(char* command, Polynomial** accumulator);
void parseFile(const char* filename, Polynomial** accumulator);

int main() {
    Polynomial* p1 = createPolynomial();
    addTerm(p1, 2, 2);
    addTerm(p1, 1, 1);
    Polynomial* p2 = createPolynomial();
    addTerm(p2, 3, 3);
    addTerm(p2, -2, 1);
    Polynomial* p = addPolynomials(p1, p2);
    printPolynomial(p);

    return 0;
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

// Parse the command file
void parseFile(const char* filename, Polynomial** accumulator) {


}
