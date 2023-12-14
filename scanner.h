typedef struct
{
	char* start;
	char* current;
	int line;
} Scanner;

typedef enum
{
	TOKEN_INSTRUCTION, TOKEN_GENERAL_REGISTER, TOKEN_SPECIAL_REGISTER, TOKEN_IMMEDIATE, TOKEN_LABEL, TOKEN_COMMA,
} TokenType;

typedef struct
{
	TokenType type;
	char* source;
	int length;
	int line;
} Token;

void initScanner(char* source);
