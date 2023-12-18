#define GROW_LIST(x) ( (x) == 0 ? 8 : (2 * (x)))

char* readFile(const char* path);
