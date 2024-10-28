#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "limits.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"
#include "myerrors.h"


kErrors ProccessError(kErrors error) {
	switch (error) {
		case INC_ARGS:
			printf("Incorrect arguments\n");
			break;
		case INC_FLAG:
			printf("Incorrect flag\n");
			break;
		case INC_NUM_OF_ARGS:
			printf("Incorrect number of arguments\n");
			break;
		case MEM_ALLOC_ERR:
			printf("Error during memory allocating\n");
			break;
		case ERR_OVERFLOW:
			printf("Overflow\n");
			break;
		case INC_FILE:
			printf("Incorrect file\n");
			break;
		case INC_INP_DATA:
			printf("Incorrect input data\n");
			break;
		case INC_SEPS:
			printf("Incorrect separators\n");
			break;
		case SUCCESS:
			//dont do anything
			break;
	}
	return error;
}