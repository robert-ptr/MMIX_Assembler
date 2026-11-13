#ifndef MMIX_LIST 
#define MMIX_LIST 

#include <stdint.h>

typedef struct 
{
    uint64_t size;
    uint64_t count;
    void** values;
} List;

void init_list(List* l);
void free_list(List* l);

int add_to_list(List* l, void* value);
void* get_from_list(List* l, uint64_t index);

void expand_list(List* l);

#endif 
