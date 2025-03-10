#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "table.h"

Table t;

#define GROW_LIST(x) ((x) == 0 ? 8 : ((x) * 2))

void initTable(Table* table)
{
    table->size = 0;
    table->count = 0;
    table->entries = NULL; 
}

void freeTable(Table* table)
{
    for(int i = 0; i < table->count; i++)
    {
    }

    free(table->entries);
}

static uint64_t hashFunc(void* value, size_t n)
{
    uint64_t hash = 2166136261u;
    uint8_t* bytes = (uint8_t*)value;
    for (uint64_t i = 0; i < n; i++)
    {
        hash ^= bytes[i];
        hash *= 16777619;
    }

    return hash;
}

Entry* findEntry(Entry* entries, uint64_t size, uint8_t* s, uint64_t n, uint64_t hash)
{
    uint64_t index = hash % size;
    for (;;)
    {
        Entry* entry = &entries[index];
        if (entry->key == NULL)
        {
            return entry;
        }
        else if(entry->key_length == n && entry->hash == hash && memcmp(entry->key, s, n) == 0)
            return entry;

        index = (index + 1) % size;
    }
}

bool findInTable(Table* table, void* s, uint64_t n, EntryValue* val)
{
    uint64_t hash;
    Entry* entry;

    hash = hashFunc(s, n);
    entry = findEntry(table->entries, table->size, s, n, hash);

    if(entry->key == NULL)
        return false;

    *val = entry->value;
    return true;
}

static void adjustSize(Table* table, uint64_t capacity)
{
    Entry* entries = (Entry*)malloc(capacity * sizeof(Entry));
    for(uint64_t i = 0; i < capacity; i++)
    {
        entries[i].key = NULL;
        entries[i].value.as_int = 0;
        entries[i].value.as_str = NULL;
        entries[i].value.as_bool = false;
        entries[i].value.as_float = 0.0f;
        entries[i].value.as_double = 0.0;
    }

    for(uint64_t i = 0; i < table->size; i++)
    {
        Entry* entry = &table->entries[i];
        if (entry->key == NULL) continue;

        Entry* dest = findEntry(entries, capacity, entry->key, entry->key_length, entry->hash);
        dest->key = entry->key;
        dest->key_length = entry->key_length;
        dest->hash = entry->hash;
        dest->value = entry->value;
    }

    free(table->entries);
    table->size = capacity;
    table->entries = entries;
}

bool addToTable(Table* table, void* s, uint64_t n, void* value, EntryType type)
{
    if(table->count + 1 > table->size * TABLE_LOAD_FACTOR)
    {
        uint64_t capacity = GROW_LIST(table->size);
        adjustSize(table, capacity);
    }

    uint64_t hash;
    hash = hashFunc(s, n);

    Entry* entry = findEntry(table->entries, table->size, s, n, hash);

    bool isNewEntry = (entry->key == NULL);
    if (isNewEntry) table->count++;

    entry->key = s;
    entry->hash = hash;
    entry->value.type = type;

    switch(type)
    {
        case TYPE_STR:
            entry->value.as_str = value;
            break;
        case TYPE_INT:
            entry->value.as_int = *(int*)value;
            break;
        case TYPE_FLOAT:
            entry->value.as_float = *(float*)value;
            break;
        case TYPE_DOUBLE:
            entry->value.as_double = *(double*)value;
            break;
        case TYPE_BOOL:
            entry->value.as_bool = *(bool*)value;
            break;
        default:
           fprintf(stderr, "Invalid entry type!"); 
    }

    entry->key_length = n;

    return isNewEntry;
}
