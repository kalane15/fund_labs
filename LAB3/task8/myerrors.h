#pragma once
typedef enum kErrors {
	SUCCESS = 0,
	INC_ARGS,
	ERR_OVERFLOW,
	MEM_ALLOC_ERR,
	INC_NUM_OF_ARGS,
	INC_FLAG,
	INC_FILE,
	INC_INP_DATA,
	NESTED_COMMENTS,
	UNKNOWN_FUNC,
	NO_POINT_COMMA,
	EXTRA_CHARACTERS
} kErrors;
kErrors ProccessError(kErrors error);