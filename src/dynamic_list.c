#include "dynamic_list.h"
#include <stdlib.h>

#define GROW_LIST(x) ((x) == 0 ? 0 : ((x) * 2))

void initList(List* l)
{
    l->size = 0;
    l->count = 0;
    l->values = NULL;
}

void freeList(List* l)
{
    free(l->values);
    initList(l);
}

void expandList(List* l)
{
    int size = GROW_LIST(l->size);

    void** new_list = (void**)malloc(size * sizeof(void*));

    for(int i = 0; i < l->size; i++)
        new_list[i] = l->values[i];

    l->values = new_list;
    l->size = size;
}

int addToList(List* l, void* value)
{
   if (l->size == l->count)
   {
       expandList(l);
   }

   l->values[l->count++] = value;
   return 0;
}

void removeFromList(List* l)
{
    if(l->count > 0)
        l->count--;
}

void* getFromList(List* l, uint64_t index)
{
    if(index >= l->count)
        return NULL;

    return l->values[index];
}
