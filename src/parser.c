#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "scanner.h"

static void errorAt(Token token, const char* message)
{
}

static void errorAtCurrent(const char* message)
{
	
}

static void error(const char* message)
{
}

static void advance(Parser* parser)
{
	parser->previous = parser->current;

	for (;;)
	{
		parser->current = scanToken();
		if(parser->current.type != TOKEN_ERR)
			break;

		errorAtCurrent();
	}
}

void parse(Scanner* scanner)
{
	if(token.type == TOKEN_LABEL)
	{

	}
}
