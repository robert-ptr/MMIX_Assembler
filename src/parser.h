#ifndef mmix_proj_parser
#define mmix_proj_parser

#include <stdbool.h>
#include "scanner.h"
#include "vm.h"
#include "trie.h"

struct
{
	Token* current;
	Token* previous;
	bool hadError;
	bool panicMode;
	Table* table;
} parser;

void parse(VM* vm);
void initParser();
void freeParser();

#endif
