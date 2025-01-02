#ifndef MMIX_TABLE
#define MMIX_TABLE
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

#define TABLE_MIN_SIZE 8
#define TABLE_LOAD_FACTOR 0.75

#define GENERIC_INSERT_FUNC(type)                                               \
    bool addToTable_##type(Table* table, void* s, uint64_t n, type value);

typedef char* string;

typedef enum {
    TYPE_INT,
    TYPE_STR,
    TYPE_BOOL,
    TYPE_FLOAT,
    TYPE_DOUBLE,
} EntryType;

typedef struct {
    EntryType type;
    union {
        uint64_t int_value;
        char* str_value;
        bool bool_value;
        float float_value;
        double double_value;
    };
} EntryValue;

typedef struct
{
	uint8_t* key;
	uint64_t hash;
	uint64_t key_length;
    EntryValue value;
} Entry;

typedef struct
{
	uint64_t size;
	uint64_t count;
	Entry* entries;
} Table;

void initTable(Table* table);

bool findInTable(Table* table, void* s, uint64_t n, EntryValue* value);

GENERIC_INSERT_FUNC(string)
GENERIC_INSERT_FUNC(uint64_t)
GENERIC_INSERT_FUNC(bool)
GENERIC_INSERT_FUNC(float)
GENERIC_INSERT_FUNC(double)

void freeTable(Table* table);
#endif
