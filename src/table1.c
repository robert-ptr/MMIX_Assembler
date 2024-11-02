#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "table.h"
#include "common.h"

Table t;

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

static uint64_t hashString(char* s, uint64_t n)
{
	uint64_t hash = 2166136261u;
	for (uint64_t i = 0; i < n; i++)
	{
		hash ^= (uint8_t)s[i];
		hash *= 16777619;
	}

	return hash;
}

Entry* findEntry(Entry* entries, uint64_t size, char* s, uint64_t n, uint64_t hash)
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

bool findInTable(Table* table, char* s, uint64_t* val)
{
	uint64_t n;
	uint64_t hash;
	Entry* entry;

	n = strlen(s);
	hash = hashString(s, n);
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
		entries[i].value = 0;
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

bool addToTable(Table* table, char* s, uint64_t value)
{
	if(table->count + 1 > table->size * TABLE_LOAD_FACTOR)
	{
		uint64_t capacity = GROW_LIST(table->size);
		adjustSize(table, capacity);
	}
	
	uint64_t hash, n;
	n = strlen(s);
	hash = hashString(s, n);
	
	Entry* entry = findEntry(table->entries, table->size, s, n, hash);
	
	bool isNewEntry = (entry->key == NULL); 
	if (isNewEntry) table->count++;

	entry->key = s;
	entry->hash = hash;
	entry->value = value;
	entry->key_length = n;
	
	return isNewEntry;
}
