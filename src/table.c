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
    for(int i = 0; i < table->size; i++)
    {
        if(table->entries[i].key.type == TYPE_STR)
            free(table->entries[i].key.as_str.lexeme);
        if(table->entries[i].value.type == TYPE_STR)
            free(table->entries[i].value.as_str.lexeme);

    }

    free(table->entries);
}

static uint64_t hashHelper(void* value, size_t n)
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

static uint64_t hashFunc(TableData* s)
{
    uint64_t hash;

    switch(s->type)
    {
        case TYPE_STR:
            hash = hashHelper(&s->as_str.lexeme, s->as_str.n);
            break;
        case TYPE_INT:
            hash = hashHelper(&s->as_int, 8);
            break;
        case TYPE_FLOAT:
            hash = hashHelper(&s->as_float, 4);
            break;
        case TYPE_DOUBLE:
            hash = hashHelper(&s->as_double, 8);
            break;
        case TYPE_BOOL:
            hash = hashHelper(&s->as_bool, 1);
            break;
        default:
            fprintf(stderr, "Unknown key type!");
    }

    return hash;
}

static Entry* findEntry(Entry* entries, uint64_t size, TableData* key, uint64_t hash)
{
    uint64_t index = hash % size;
    for (;;)
    {
        Entry* entry = &entries[index];
        if (entry->key.type == TYPE_UNASSIGNED)
        {
            return entry;
        }
        else if(entry->hash == hash && entry->key.type == key->type)
        {
            switch(key->type)
            {
                case TYPE_STR:
                    printf("Comparing strings:\n");
                    printf("  key.as_str.lexeme = %p\n", key->as_str.lexeme);
                    printf("  entry->key.as_str.lexeme = %p\n", entry->key.as_str.lexeme);
                    printf("  key.as_str.n = %lu\n", entry->key.as_str.n);

                    if (key->as_str.lexeme == NULL || entry->key.as_str.lexeme == NULL) {
                        fprintf(stderr, "Error: Null pointer in string key\n");
                        return NULL;
                    }
                    fflush(stdout);
                    
                    if (key->as_str.n == entry->key.as_str.n && memcmp(key->as_str.lexeme, entry->key.as_str.lexeme, key->as_str.n) == 0)
                        return entry;
                    break;
                case TYPE_INT:
                    if (key->as_int == entry->key.as_int)
                        return entry;
                    break;
                case TYPE_FLOAT:
                    if (key->as_float == entry->key.as_float)
                        return entry;
                    break;
                case TYPE_DOUBLE:
                    if (key->as_double == entry->key.as_double)
                        return entry;
                    break;
                case TYPE_BOOL:
                    if (key->as_bool == entry->key.as_bool)
                        return entry;
                    break;
                default:
                    fprintf(stderr, "Unknown key type!");
            }
        }

        index = (index + 1) % size;
    }
}

bool findInTable(Table* table, TableData* s, TableData* val)
{
    uint64_t hash = hashFunc(s);
    Entry* entry;
 
    entry = findEntry(table->entries, table->size, s, hash);

    if(entry->key.type == TYPE_UNASSIGNED)
        return false;

    if(val != NULL)
        *val = entry->value;
    
    return true;
}

static void adjustSize(Table* table, uint64_t capacity)
{
    Entry* entries = (Entry*)malloc(capacity * sizeof(Entry));
    for(uint64_t i = 0; i < capacity; i++)
    {
        entries[i].value.as_str.lexeme = NULL;
        entries[i].value.as_str.n = 0;
        entries[i].key.type = TYPE_UNASSIGNED;
    }

    for(uint64_t i = 0; i < table->size; i++)
    {
        Entry* entry = &table->entries[i];
        if (entry->key.type == TYPE_UNASSIGNED) continue;

        Entry* dest = findEntry(entries, capacity, &entry->key, entry->hash);
        dest->key = entry->key;
        dest->hash = entry->hash;
        dest->value = entry->value;
    }

    free(table->entries);
    table->size = capacity;
    table->entries = entries;
}

bool addToTable(Table* table, TableData* s, TableData* value)
{
    if (s->type == TYPE_UNASSIGNED || value->type == TYPE_UNASSIGNED) {
        fprintf(stderr, "Error: Uninitialized TableData passed to addToTable\n");
        return false;
    }

    if(table->count + 1 > table->size * TABLE_LOAD_FACTOR)
    {
        uint64_t capacity = GROW_LIST(table->size);
        adjustSize(table, capacity);
    }

    uint64_t hash;
    hash = hashFunc(s);

    Entry* entry = findEntry(table->entries, table->size, s, hash);

    bool isNewEntry = (entry->key.type == TYPE_UNASSIGNED);
    if (isNewEntry) table->count++;

    if (s->type == TYPE_STR)
    {
        if (entry->key.type == TYPE_STR && entry->key.as_str.lexeme != NULL) {
            free(entry->key.as_str.lexeme);
        }
        entry->key.type = s->type;
        entry->key.as_str.n = s->as_str.n;
        entry->key.as_str.lexeme = (char*)malloc(s->as_str.n * sizeof(char));
        strncpy(entry->key.as_str.lexeme, s->as_str.lexeme, s->as_str.n);
    }
    else 
        entry->key = *s;
    entry->hash = hash;

    if (value->type == TYPE_STR)
    {
        if (entry->value.type == TYPE_STR && entry->value.as_str.lexeme != NULL) {
            free(entry->value.as_str.lexeme);
        }
        entry->value.type = value->type;
        entry->value.as_str.n = value->as_str.n;
        entry->value.as_str.lexeme = (char*)malloc(value->as_str.n * sizeof(char));
        strncpy(entry->value.as_str.lexeme, value->as_str.lexeme, value->as_str.n);
    }
    else 
        entry->value = *value;

    return isNewEntry;
}
