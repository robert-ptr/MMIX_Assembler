#ifndef MMIX_PROJ_COMMON
#define MMIX_PROJ_COMMON

#define GROW_LIST(x) ( (x) == 0 ? 8 : (2 * (x)))

char* readFile(const char* path);
void toLowercaseC(char* c);
void toLowercase(char** s);
char** importInstructions(const char* path, int* size);


#endif
