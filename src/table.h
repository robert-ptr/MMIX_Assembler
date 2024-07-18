#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define TABLE_MIN_SIZE 8
#define TABLE_LOAD_FACTOR 0.75

typedef struct
{
	char* key;
	uint64_t hash;
	uint64_t key_length;
	uint64_t value;
} Entry;

typedef struct
{
	uint64_t size;
	uint64_t count;
	Entry* entries;
} Table;

void initTable(Table* table);

bool findInTable(Table* table, char* s, uint64_t* value);
bool addToTable(Table* table, char* s, uint64_t value);

void freeTable(Table* table);
