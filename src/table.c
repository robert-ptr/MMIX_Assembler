#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "table.h"

#define GROW_TABLE(x) ( (x) == 0 ? 8 : (2 * x))

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

uint32_t hashString(char* s, int n)
{
	uint32_t hash = 2166136261u;
	for (int i = 0; i < n; i++)
	{
		hash ^= (uint8_t)s[i];
		hash *= 16777619;
	}

	return hash;
}

Entry* findEntry(Entry* entries, int size, char* s, int n, uint32_t hash)
{
	uint32_t index = hash % size;
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

static void adjustSize(Table* table, int capacity)
{
	Entry* entries = (Entry*)malloc(capacity * sizeof(Entry));
	for(int i = 0; i < capacity; i++)
	{
		entries[i].key = NULL;
		entries[i].value = 0;
	}

	for(int i = 0; i < table->size; i++)
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

bool addToTable(Table* table, char* s, int n)
{
	if(table->count + 1 > table->size * TABLE_LOAD_FACTOR)
	{
		int capacity = GROW_TABLE(table->size);
		adjustSize(table, capacity);
	}
	
	uint32_t hash = hashString(s, n);
	Entry* entry = findEntry(table->entries, table->size, s, n, hash);
	bool isNewEntry = (entry->key == NULL); 
	if (isNewEntry) table->count++;

	entry->key = s;
	entry->hash = hash;
	entry->value++;
	entry->key_length = n;
	return isNewEntry;
}

/* for testing
int main()
{
	initTable(&t);
	printf("%d", addToTable(&t, "add", 3, -211));
	addToTable(&t, "add", 3, 200);
	Entry* entry = findEntry(&t, "add", 3, hashString("add",3));
	printf("%d", (entry->value));
	freeTable(&t);
}
*/
