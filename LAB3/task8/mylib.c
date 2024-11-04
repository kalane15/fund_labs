#include "string.h"
#include "limits.h"
#include "ctype.h"
#include "myerrors.h"
#include "stdbool.h"
#define ll long long

bool CheckNumber(char* number, int osn) {
    int start = 0;

    if (number[0] == '-') {
        if (strlen(number) == 1) {
            return false;
        }
        else {
            start = 1;
        }
    }
    for (int i = start; i < strlen(number); i++) {
        int res = 0;
        if ('0' <= number[i] && number[i] <= '9') {
            res = number[i] - '0';
        }
        else if (('A' <= number[i] && number[i] <= 'Z') || ('a' <= number[i] && number[i] <= 'z')) {
            res = 10 + tolower(number[i]) - 'a';
        }
        else {
            return false;
        }
        if (res >= osn) {
            return false;
        }
    }
    return true;
}

kErrors StringToInt(char* str, int* res) {
    int osn = 10;

    if (!CheckNumber(str, osn)) {
        return INC_ARGS;
    }
    int sign = 1;
    int start = 0;
    if (str[0] == '-') {
        sign = -1;
        start = 1;
    }
    long long int result = 0;
    for (int i = start; i < strlen(str); i++) {
        if ('0' <= str[i] && str[i] <= '9') {
            if (result > (INT_MAX / osn)) {
                return ERR_OVERFLOW;
            }
            result *= osn;
            result += str[i] - '0';
        }
        else {
            return INC_ARGS;
        }
    }
    *res = result * sign;
    return SUCCESS;
}
