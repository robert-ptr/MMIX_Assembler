#ifndef MMIX_PROJ_COMMON
#define MMIX_PROJ_COMMON

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	char* name;
	uint8_t arity[3]; // maximum 3 possible values
	bool has_alt_ver;
	uint8_t is_constant[3][3];
} Instruction;

extern Instruction instructions[256]; 
char* getString(char* buffer, int32_t length, int32_t buf_index);
char* readFile(const char* path);
FILE* createFile(const char* path);

void charToLowercase(char* c);
void stringToLowercase(char** s);
char* intToHexString(uint64_t n);
char* intToBinaryString(uint64_t n);
char* intToDecimalString(uint64_t n);
uint64_t fromHexadecimal(const char* str);

#endif
