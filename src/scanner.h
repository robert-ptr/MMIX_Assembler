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
	TOKEN_STRING, TOKEN_COMMA, TOKEN_CONSTANT ,TOKEN_ERR, TOKEN_EOF, TOKEN_PLUS, TOKEN_MINUS, TOKEN_SLASH,
	TOKEN_STAR, TOKEN_AROUND, TOKEN_DSLASH, TOKEN_LSHIFT, TOKEN_RSHIFT, TOKEN_AND, TOKEN_OR, 
	TOKEN_XOR, TOKEN_SEMICOLON, TOKEN_REGISTER, TOKEN_COMPLEMENT
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
void toLowercase(char** word);

#endif
