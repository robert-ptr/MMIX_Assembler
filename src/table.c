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
	free(table->entries);
    initTable(table);
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
		entries[i].value.int_value = 0;
        entries[i].value.str_value = NULL;
        entries[i].value.bool_value = false;
        entries[i].value.float_value = 0.0f;
        entries[i].value.double_value = 0.0;
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

static void setEntry_uint64_t(EntryValue* entry, int64_t value)
{
    entry->int_value = value;
    entry->type = TYPE_INT;
}

static void setEntry_bool(EntryValue* entry, bool value)
{
    entry->bool_value = value;
    entry->type = TYPE_BOOL;
}

static void setEntry_float(EntryValue* entry, float value)
{
    entry->float_value = value;
    entry->type = TYPE_FLOAT;
}

static void setEntry_double(EntryValue* entry, double value)
{
    entry->double_value = value;
    entry->type = TYPE_DOUBLE;
}

static void setEntry_string(EntryValue* entry, char* str)
{
    size_t len = strlen(str);
    entry->str_value = malloc(len + 1);
    strcpy(entry->str_value, str);
    entry->str_value[len] = '\0';
    entry->type = TYPE_STR;
}

#define GENERIC_INSERT_FUNC_DEF(type)                                           \
    bool addToTable_##type(Table* table, void* s, uint64_t n, type value)       \
    {                                                                           \
        if(table->count + 1 > table->size * TABLE_LOAD_FACTOR)                  \ 
        {                                                                       \
            uint64_t capacity = GROW_LIST(table->size);                         \
            adjustSize(table, capacity);                                        \
        }                                                                       \
                                                                                \
        uint64_t hash;                                                          \
        hash = hashFunc(s, n);                                                  \
                                                                                \
        Entry* entry = findEntry(table->entries, table->size, s, n, hash);      \
                                                                                \
        bool isNewEntry = (entry->key == NULL);                                 \
        if (isNewEntry) table->count++;                                         \
                                                                                \
        entry->key = s;                                                         \
        entry->hash = hash;                                                     \
        setEntry_##type(&entry->value, value);                                  \
        entry->key_length = n;                                                  \
                                                                                \
        return isNewEntry;                                                      \
	}

    // entry->key just points to the string, it doesn't own it so you can't just free it 
    // should i copy it?

GENERIC_INSERT_FUNC_DEF(string)
GENERIC_INSERT_FUNC_DEF(uint64_t)
GENERIC_INSERT_FUNC_DEF(bool)
GENERIC_INSERT_FUNC_DEF(float)
GENERIC_INSERT_FUNC_DEF(double)
