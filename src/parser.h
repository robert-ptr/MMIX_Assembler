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
    Table* symbols;
    FILE* fp;
    uint8_t general_reg;
} Parser;

extern Parser parser;

extern Table instr_indices;

void parse();
void initParser(char* output_file);
void freeParser();

#endif
