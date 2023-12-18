#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "scanner.h"
#include "vm.h"

static void errorAt(Token token, const char* message)
{
	fprintf(stderr, "[line %d] Error", token->line);
	if (token->type == TOKEN_EOF) 
	{
		fprintf(stderr, " at end");
	} 
	else if (token->type == TOKEN_ERROR) 
	{

	} 
	else 
	{
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	}
	fprintf(stderr, ": %s\n", message);
	parser.hadError = true;
}

static void errorAtCurrent(Parser* parser, const char* message)
{
	errorAt(parser->current, message);	
}

static void error(Parser* parser, const char* message)
{
	errorAt(parser->previous, message);
}

static void advance(Parser* parser, Scanner* scanner)
{
	parser->previous = parser->current;

	for (;;)
	{
		parser->current = scanToken(scanner);
		if(parser->current.type != TOKEN_ERR)
			break;

		errorAtCurrent(parser, );
	}
}

static void emitByte(VM* vm, Byte byte)
{
	addByte(vm->byte_set, byte);
}

static bool isMacro(Parser* parser)
{
	Token token = parser->current;
	// IS, GREG, BYTE, WYDE, TETRA, OCTA
	if(token.length == 2 && memcmp(token.start, "is", 2))
	{
		return true;	
	}
	else if (token.length == 4)
	{
		if(memcmp(token.start, "greg", 4))
			return true;
		else if(memcmp(token.start, "byte", 4))
			return true;
		else if(memcmp(token.start, "wyde", 4))
			return true;
		else if(memcmp(token.start, "octa", 4))
			return true;
	}
	else if (token.length == 5 && memcmp(token.start, "tetra", 5))
		return true;

	return false;
}

static void labelStatement(Parser* parser, Scanner* scanner, VM* vm)
{
	char* word = (char*)malloc((parser->previous.length + 1) * sizeof(char));
	word = parser->previous.start;
	word[length] = '\0';

	if(isMacro(parser))
	{
		advance();
		
		if(parser->current.type != TOKEN_GENERAL_REGISTER && parser->current.type != TOKEN_SPECIAL_REGISTER)
		{
			errorAtCurrent("Only registers and locations can be labeled.");
			// do something here
		}
		// convert parser->current.start to int
		addToTable(vm->table, word, parser->current.start); 
	}
}

static void instructionStatement(Parser* parser, Scanner* scanner, VM* vm)
{
}

void parse(Parser* parser, Scanner* scanner, VM* vm)
{
	advance(parser, scanner);

	if(parser.previous == TOKEN_LABEL)
	{
		labelStatement(parser, scanner);
	}
	else
	{
		instructionStatement(parser, scanner);
	}
}
