#ifndef MMIX_PROJ_COMMON
#define MMIX_PROJ_COMMON

#include <stdio.h>
#include <stdint.h>

#define GROW_LIST(x) ( (x) == 0 ? 8 : (2 * (x)))

char* getString(char* buffer, int32_t length, int32_t buf_index);
char* readFile(const char* path);
FILE* createFile(const char* path);

void charToLowercase(char* c);
void stringToLowercase(char** s);
char** importInstructions(const char* path, int* size);
char* intToString(uint64_t n);

#endif
