#ifndef MMIX_PROJ_COMMON
#define MMIX_PROJ_COMMON

#define GROW_LIST(x) ( (x) == 0 ? 8 : (2 * (x)))

char* getString(char* buffer, int32_t length, int32_t buf_index);
char* readFile(const char* path);
FILE* createFile(const char* path);
<<<<<<< HEAD
void charToLowercase(char* c);
void wordToLowercase(char** s);
char** importInstructions(const char* path, int* size);
=======
void toLowercaseC(char* c);
void toLowercase(char** s);
char** importInstructions(const char* path, int32_t* size);
>>>>>>> 291cd11b95e56b429e014cc16f61c10749dadc76


#endif
