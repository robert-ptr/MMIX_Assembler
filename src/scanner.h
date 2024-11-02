#ifndef mmix_proj_scanner
#define mmix_proj_scanner

#include "trie.h"
#include "common.h"

struct
{
	char* start;
	char* current;
	int32_t line;

	TrieNode* instruction_trie;
} scanner;

typedef enum
{
	TOKEN_INSTRUCTION, TOKEN_GENERAL_REGISTER, TOKEN_SPECIAL_REGISTER, TOKEN_IMMEDIATE, TOKEN_LABEL, 
	TOKEN_STRING, TOKEN_COMMA, TOKEN_CONSTANT ,TOKEN_ERR, TOKEN_EOF, TOKEN_PLUS, TOKEN_MINUS, TOKEN_SLASH,
	TOKEN_STAR, TOKEN_AROUND, TOKEN_DSLASH, TOKEN_LSHIFT, TOKEN_RSHIFT, TOKEN_AND, TOKEN_OR, TOKEN_XOR,
	TOKEN_MOD ,TOKEN_SEMICOLON, TOKEN_REGISTER, TOKEN_COMPLEMENT, TOKEN_LPAREN, TOKEN_RPAREN,
	TOKEN_SCONST, 
    TOKEN_IS, TOKEN_GREG, TOKEN_LOC, TOKEN_BYTE, TOKEN_WYDE, TOKEN_TETRA, TOKEN_OCTA, // preprocessor stuff
} TokenType;

typedef struct
{
	TokenType type;
	char* start;
	int32_t length;
	int32_t line;
} Token;

void initScanner(char* source);
void freeScanner();
Token scanToken();

#endif
