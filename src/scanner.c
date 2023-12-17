#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"
#include "trie.h"

char* getString(char* buffer, int length, int buf_index);
{
	if(length <= 1)
		return NULL;
	
	int i = 0;
	char* new_string = (char*)malloc(length * sizeof(char));
	new_string[length - 1] = '\0';
	for(; (i + 1 - length) != 0; i++)
	{
		new_string[i]	= buffer[buf_index - length + 1 + i];	
	}

	return new_string;
}

void toLowercase(char** string)
{
	for(int i = 0; i < (*string)[i] != '\0'; i++)
	{
		if((*string)[i] >= 'A' && (*string)[i] <= 'Z')
			(*string)[i] += 32;
	}
}

char** importInstructions(const char* path, int* size)
{
	char** strings = (char**)malloc(256 * sizeof(char*));

	char* buffer = readFile(path);
	int length = 0;
	int index = 0;
	int buf_index = 0;
	while(buffer[buf_index] != '\0')
	{
		if(buffer[buf_index] == ' ' || buffer[buf_index] == ';' || buffer[buf_index] == '\t' || buffer[buf_index] == '\n')
		{
			if(index == 0)
				length++;
			char* new_string = getString(buffer, length, buf_index);
			if(new_string != NULL)
			{
				strings[index++] = new_string;
			}
			length = 0;
		}
		buf_index++;
		length++;
	}
	*size = index;
	index--;
	while(index >= 0)
	{
		printf("%s ", strings[index]);
		index--;
	}

	return strings;
}

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

static Token errorToken(char* message)
{
	Token token;
	token.start = message;
	token.line = scanner.line;
	token.length = strlen(message);
	token.type = TOKEN_ERR;

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
	int length = scanner.current - scanner.start;
	char* word = (char*)malloc(length * sizeof(char));
	for(int i = 0; i < length; i++)
	{
		word[i] = scanner.start + i;
	}
	if(findWord(word))
		return TOKEN_INSTRUCTION;

	return TOKEN_LABEL;
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

	int size;
	char** words = importInstructions("instructions.txt", &size);
	scanner.root = getNode();
	createTrie(scanner.root, words, size);
}

static TokenType makeString()
{
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
		case '$': return makeToken(TOKEN_GENERAL_REGISTER);
		case 'r': return makeToken(TOKEN_SPECIAL_REGISTER);
		case '#': return makeToken(TOKEN_LOCATION);
		case '%': skipLine(); break;
		case '"': return makeString();
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
