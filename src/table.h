#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define TABLE_MIN_SIZE 8
#define TABLE_LOAD_FACTOR 0.75

typedef struct
{
	char* key;
	uint32_t hash;
	int key_length;
	int value;
} Entry;

typedef struct
{
	int size;
	int count;
	Entry* entries;
} Table;

void initTable(Table* table);
uint32_t hashString(char* s, int n);
Entry* findEntry(Entry* entries, int size, char* s, int n, uint32_t hash);
bool addToTable(Table* table, char* s, int n);
void freeTable(Table* table);
