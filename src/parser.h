#ifndef mmix_proj_parser
#define mmix_proj_parser

#include <stdbool.h>
#include "scanner.h"

typedef struct
{
	Token current;
	Token previous;
	bool hadError;
	bool panicMode;
} Parser;

void parse(Parser* parser, Scanner* scanner, VM* vm);
void initParser();

#endif
