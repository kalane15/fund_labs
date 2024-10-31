#pragma once
#include "myerrors.h"
#define ll long long

kErrors ParseDouble(char* str, double* out);
bool CheckIfDoubleCorrect(char* n);
bool CheckNumber(char* number, int osn);
kErrors StringToInt(char* str, ll* res);