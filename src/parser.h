#ifndef MMIX_PARSER
#define MMIX_PARSER

#include <stdbool.h>
#include "scanner.h"
#include "trie.h"
#include "table.h"

typedef struct
{
    Token current;
    Token previous;
    bool panic_mode;
    Table* locations; // you can jump to these
    Table* aliases; // but not to these
    uint64_t register_values[16];
    FILE* fp;
    uint8_t general_reg;
    char current_prefix[256];
    uint8_t prefix_length;
    uint64_t current_location; // number of bytes
} Parser;

extern Parser parser;

extern Table instr_indices;

void parse();
void initParser(char* output_file);
void freeParser();

#endif
