#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"

Scanner scanner;

static bool isAtEnd()
{
	return *scanner.current == '\0';
}

static Token makeToken(TokenType type)
{
	Token token;
	token.start = scanner.start;
	token.line = scanner.line;
	token.length = (int)(scanner.current - scanner.start);
	token.type = type;

	return token;
}

static char advance()
{
	scanner.current++;
	return scanner.current[-1];
}

static char peek()
{
	return *scanner.current;
}

static char peekNext()
{
	if(*scanner.current == '\0') return '\0';

	return scanner.current[1];
}

static void skipWhitespace()
{
	for(;;)
	{
		char c = peek();

		switch(c)
		{
			case ' ':
			case '\t':
			case '\r':
				advance();
				valid = true;
				break;
			case '\n':
				scanner.line++;
				valid = true;
				advance();
				break;
			default:
				return;
		}
	}
}

static void match(int start, int length, char* pattern)
{

}

static TokenType identifierType()
{
	int length = (int)(scanner.current - scanner.start);
	// implement trie
}

static TokenType findTokenType()
{
	bool immediate = true;
	for(;;)
	{
		if(isAlphanumeric() && !isNumeric())
		{
			immediate = false;
		}
		while(isAlphanumeric())
			advance();
	}

	if(immediate)
		return TOKEN_IMMEDIATE;

	return identifierType();
}

static bool isNumeric()
{
	char c = peek();
	if((c >= '0' && c <= '9')
			return true;

	return false;
}

static bool isAlphanumeric()
{
		char c = peek();
		if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
			return true;

		return false;
}

void initScanner(const char* source)
{
	scanner.start = source;
	scanner.current = source;
	line = 1;
}

Token scanToken()
{
	skipWhitespace();
	scanner.start = scanner.current;

	if (isAtEnd())
		return makeToken(TOKEN_EOF);

	char c = advance();

	switch(c)
	{
		case ',': return makeToken(TOKEN_COMMA);
		case '$': return;
		case 'r': return;
		case '#': return;
		case '%': skipLine(); break;
		default:
		{
			// then we are either dealing with unknown characters or
			// either an instruction, a label or an immediate
			if(isAlphanumeric())
			{
				return makeToken(findTokenType());
			}

			return makeToken(TOKEN_ERR);
		}
	}

	return errorToken("Unexpected character.");
}
