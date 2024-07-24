#ifndef mmix_proj_parser
#define mmix_proj_parser

#include <stdbool.h>
#include "scanner.h"
#include "vm.h"
#include "trie.h"

typedef struct
{
	Token* current;
	Token* previous;
	bool hadError;
	bool panicMode;
	Table* table;
} Parser;

void parse(Parser* parser, Scanner* scanner, VM* vm);
void initParser(Parser* parser);
void freeParser(Parser* parser);

#endif
