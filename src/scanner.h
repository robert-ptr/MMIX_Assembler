#ifndef mmix_proj_scanner
#define mmix_proj_scanner

#include "trie.h"

typedef struct
{
	char* start;
	char* current;
	int line;

	TrieNode* trie;
} Scanner;

typedef enum
{
	TOKEN_INSTRUCTION, TOKEN_GENERAL_REGISTER, TOKEN_SPECIAL_REGISTER, TOKEN_IMMEDIATE, TOKEN_LABEL, 
	TOKEN_STRING, TOKEN_COMMA, TOKEN_LOCATION ,TOKEN_ERR, TOKEN_EOF,
} TokenType;

typedef struct
{
	TokenType type;
	char* start;
	int length;
	int line;
} Token;

Scanner* initScanner(char* source);
void freeScanner(Scanner* scanner);
Token scanToken();

#endif
