#ifndef MMIX_TABLE
#define MMIX_TABLE
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

#define TABLE_MIN_SIZE 8
#define TABLE_LOAD_FACTOR 0.75

typedef enum {
    TYPE_INT,
    TYPE_STR,
    TYPE_BOOL,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_UNASSIGNED,
} EntryType;

typedef enum {
    TYPE_REGISTER,
    TYPE_IMMEDIATE,
    TYPE_LOCATION,
} MMIXEntryType;

typedef struct {
    EntryType type;
    union {
        uint64_t as_int;
        char* as_str;
        bool as_bool;
        float as_float;
        double as_double;
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
bool addToTable(Table* table, void* s, uint64_t n, void* value, EntryType type);
void freeTable(Table* table);
#endif
