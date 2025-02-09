#ifndef MMIX_LIST 
#define MMIX_LIST 

#include <stdint.h>

typedef struct 
{
    uint64_t size;
    uint64_t count;
    void** values;
} List;

void initList(List* l);
void freeList(List* l);

int addToList(List* l, void* value);
void* getFromList(List* l, uint64_t index);

void expandList(List* l);

#endif 
