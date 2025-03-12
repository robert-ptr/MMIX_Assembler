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
} DataType;

typedef struct {
    DataType type;
    union {
        uint64_t as_int;
        struct {
            char* lexeme;
            uint64_t n;
        } as_str;
        bool as_bool;
        float as_float;
        double as_double;
    };
} TableData;

typedef struct
{
    TableData key;
    uint64_t hash;
    TableData value;
} Entry;

typedef struct
{
    uint64_t size;
    uint64_t count;
    Entry* entries;
} Table;

void initTable(Table* table);
bool findInTable(Table* table, TableData* s, TableData* value);
bool addToTable(Table* table, TableData* s, TableData* value);
void freeTable(Table* table);
#endif
