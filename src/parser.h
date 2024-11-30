#ifndef MMIX_PROJ_PARSER
#define MMIX_PROJ_PARSER

#include <stdbool.h>
#include "scanner.h"
#include "vm.h"
#include "trie.h"
#include "table.h"

typedef struct
{
	Token* current;
	Token* previous;
	bool hadError;
	bool panicMode;
	Table table;
    uint32_t line;
} Parser;

extern Parser parser;

extern Table instr_indices;

void parse(VM* vm);
void initParser();
void freeParser();

#endif
