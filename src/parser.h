#ifndef MMIX_PARSER
#define MMIX_PARSER

#include <stdbool.h>
#include "scanner.h"
#include "trie.h"
#include "table.h"

typedef struct 
{
    uint64_t* values;
    uint64_t size;
    uint64_t top;
} Stack;

typedef struct
{
    Token current;
    Token previous;
    bool panic_mode;
    Table* locations; // you can jump to these
    Table* aliases; // but not to these
    FILE* fp;
    char current_prefix[1024];
    uint8_t prefix_length;
    uint8_t general_reg;
    uint64_t register_values[16];
    uint64_t current_location; // number of bytes
    
    // all of these are for labels of the type: *B, *H, *F, where * is a digit from 0 to 9
    Stack* local_labels[10];
} Parser;


extern Parser parser;

extern Table instr_indices;

void parse();
void initParser(char* output_file);
void freeParser();

#endif
