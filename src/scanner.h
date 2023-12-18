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
	TOKEN_STRING, TOKEN_COMMA, TOKEN_CONSTANT ,TOKEN_ERR, TOKEN_EOF, TOKEN_ADD, TOKEN_SUB, TOKEN_DIV,
	TOKEN_MUL, TOKEN_AROUND,
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
Token scanToken(Scanner* scanner);

#endif
