#ifndef MMIX_TABLE
#define MMIX_TABLE
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

#define TABLE_MIN_SIZE 8
#define TABLE_LOAD_FACTOR 0.75

#define GENERIC_INSERT_FUNC(type)                                               \
    bool addToTable_##type(Table* table, char* s, type value);

typedef enum {
    TYPE_INT,
    TYPE_STR,
    TYPE_BOOL,
    TYPE_FLOAT,
    TYPE_DOUBLE,
} EntryType;

typedef union {
    uint64_t int_value;
    char* str_value;
    bool bool_value;
    float float_value;
    double double_value;
} EntryValue;

typedef struct
{
	char* key;
	uint64_t hash;
	uint64_t key_length;
    EntryType type;
    EntryValue value;
} Entry;

typedef struct
{
	uint64_t size;
	uint64_t count;
	Entry* entries;
} Table;

void initTable(Table* table);

bool findInTable(Table* table, char* s, EntryValue* value);

GENERIC_INSERT_FUNC(int64_t)
GENERIC_INSERT_FUNC(bool)
GENERIC_INSERT_FUNC(float)
GENERIC_INSERT_FUNC(double)

void freeTable(Table* table);
#endif
