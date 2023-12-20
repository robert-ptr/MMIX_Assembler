#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include "scanner.h"
#include "vm.h"

typedef enum
{
	PREC_NONE,
	PREC_TERM,
	PREC_FACTOR,
	PREC_UNARY
} Precedence;

typedef void (*ParseFn)(bool canAssign); 

typedef struct
{
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;
} ParseRule;

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


static void consume(Parser* parser, TokenType type, const char* message)
{
	if (parser->current.type == type)
	{
		advance();
		return;
	}

	errorAtCurrent(message);
}

static bool check(Parser* parser, TokenType type)
{
	return parser->current.type == type;
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

static bool match(char* instruction, int start, char* pattern, int length)
{
	if(memcmp(instruction + start, pattern, length) == 0)
		return true;
	
	return false;
}

static void term(Parser* parser, Scanner* scanner, VM* vm)
{
}

static void unary(Parser* parser, Scanner* scanner, VM* vm)
{
	TokenType operatorType = parser->previous.type;

	// add here

	switch (operatorType)
	{
		case TOKEN_MINUS: break;
		case TOKEN_COMPLEMENT: break;
		case TOKEN_REGISTER:	break;
		default: return;
	}
}

static void binary(Parser* parsser, VM* vm)
{
	TokenType operatorType = parser->previous.type;

	// add here
	
	switch (operatorType)
	{
		case TOKEN_SLASH:	break;
		case TOKEN_DSLASH:break;
		case TOKEN_PLUS:	break;
		case TOKEN_MINUS:	break;
		case TOKEN_STAR:	break;
		case TOKEN_AND:		break;
		case TOKEN_OR:		break;
		case TOKEN_XOR:		break;
		case TOKEN_LSHIFT:break;
		case TOKEN_RSHIFT:break;
		default: return;
	}
}

static void expressionStatement(Parser* parser, Scanner* scanner, VM* vm)
{
	// maybe i can resolve expressions at compile time o.o
}

static void instructionStatement(Parser* parser, Scanner* scanner, VM* vm)
{
	advance(parser, scanner);
	if(parser->current.type == TOKEN_INSTRUCTION)
	{
		char c = parser->current.start[0];
		Token token = parser->current;	
		switch(c)
		{
			case '2': 
			{
				if(token.length == 5 &&  match(token.start, 1, "addu", 4)
						emitByte(vm, OP_2ADDU);
				else
					errorAtCurrent(parser, "Unknown command, did you mean to write 2ADDU?");
				break;
			}
			case '4':
			{
				if(token.length == 5 && match(token.start, 1, "addu", 4)
					emitByte(vm, OP_4ADDU);
				else
					errorAtCurrent(parser, "Unknown command, did you mean to write 4ADDU?");
				break;
			}
			case '8':
			{
				if(token.length == 5 && match(token.start, 1, "addu", 8)
					emitByte(vm, OP_8ADDU);
				else
					errorAtCurrent(parser, "Unknown command, did you mean to write 8ADDU?");
				break;

			}
			case '1':
			{
				if(token.length == 6 && match(token.start, 1, "6addu", 5)
					emitByte(vm, OP_16ADDU);
				else
					errorAtCurrent(parser, "Unknown command, did you mean to write 16ADDU?");
				break;

			}
			case 'a':
			{
				if(token.length == 3 && match(token.start, 1, "dd", 2)
				{
					emitByte(vm, OP_ADD);
				}
				else if(token.length == 4 && match(token.start, 1, "ddu", 2)
				{
					emitByte(vm, OP_ADDU);
				}
				else if(match(token.start, 1, "nd", 2)
				{
					if(token.length == 3)
					{
						emitByte(vm, OP_AND);
					}
					else if(token.length == 4 && match(token.start, 1, "ndn", 3)
					{
						emitByte(vm, OP_ANDN);
					}
					else if(token.length == 5 && match(token.start, 1, "ndnh", 4)
					{
						emitByte(vm, OP_ANDNH);
					}
					else if(token.length == 5 && match(token.start, 1, "ndnl", 4)
					{
						emitByte(vm, OP_ANDNL);
					}
					else if(token.length == 6 && match(token.start, 1, "ndnmh", 5)
					{
						emitByte(vm, OP_ANDNMH);
					}
					else if(token.length == 6 && match(token.start, 1, "ndnml", 5)
					{
						emitByte(vm, OP_ANDNML);
					}
					else
					{
						errorAtCurrent(parser, "Unknown instruction.");
					}
				}
				else
				{
					errorAtCurrent(parser, "Unknown instruction.");
				}
			}
			case 'b':
			{
				if(token.length == 4 && match(token.start, 1, "dif", 3)
				{
					emitByte(OP_BDIF);
				}
				else if(token.length == 3 && match(token.start, 1, "ev", 2);
				{
					emitByte(OP_BEV);
				}
				else if(token.length == 2 && match(token.start, 1, "n", 1);
				{
					emitByte(OP_BN);
				}
				else if(token.length == 3 && match(token.start, 1, "nn", 2)
				{
					emitByte(OP_BNN);
				}
				else if(token.length == 3 && match(token.start, 1, "np", 2)
				{
					emitByte(OP_BNP);
				}
				else if(token.length == 3 && match(token.start, 1, "nz", 2)
				{
					emitByte(OP_BNZ);
				}
				else if(token.length == 3 && match(token.start, 1, "od", 2)
				{
					emitByte(OP_BOD);
				}
				else if(token.length == 2 && match(token.start, 1, "p", 1)
				{
					emitByte(OP_BP);
				}
				else if(token.length == 2 && match(token.start, 1, "z", 1)
				{
					emitByte(OP_BZ);
				}
				else
				{
					errorAtCurrent(parser, "Unknown instruction.");
				}
			}
			case 'c':
			{
				if(token.length == 3 && match(token.start, 1, "mp", 2)
				{
					emitByte(OP_CMP);
				}
				else if(token.length == 4 && match(token.start, 1, "mpu", 3);
				{
					emitByte(OP_CMPU);
				}
				else if(token.length == 4 && match(token.start, 1, "sev", 3);
				{
					emitByte(OP_CSEV);
				}
				else if(token.length == 3 && match(token.start, 1, "sn", 2)
				{
					emitByte(OP_CSN);
				}
				else if(token.length == 4 && match(token.start, 1, "snn", 3)
				{
					emitByte(OP_CSNN);
				}
				else if(token.length == 4 && match(token.start, 1, "snp", 3)
				{
					emitByte(OP_CSNP);
				}
				else if(token.length == 4 && match(token.start, 1, "snz", 3)
				{
					emitByte(OP_CSNZ);
				}
				else if(token.length == 4 && match(token.start, 1, "sod", 3)
				{
					emitByte(OP_CSOD);
				}
				else if(token.length == 3 && match(token.start, 1, "sp", 2)
				{
					emitByte(OP_CSP);
				}
				else if(token.length == 5 && match(token.start, 1, "swap", 4)
				{
					emitByte(OP_CSWAP);
				}
				else if(token.length == 3 && match(token.start, 1, "sz", 2)
				{
					emitByte(OP_CSZ);
				}
				else
				{
					errorAtCurrent(parser, "Unknown instruction.");
				}
			}
			case 'd':
			{
				if(token.length == 3 && match(token.start, 1, "iv", 2)
				{
					emitByte(OP_DIV);
				}
				else if(token.length == 4 && match(token.start, 1, "ivu", 3)
				{
					emitByte(OP_DIVU);
				}
				else
				{
					errorAtCurrent(parser, "Unknown instruction.");
				}
			}
			case 'f':
			{
			}
			case 'g':
			{
			}
			case 'i':
			{
			}
			case 'j':
			{
			}
			case 'l':
			{
			}
			case 'm':
			{
			}
			case 'n':
			{
			}
			case 'o':
			{
			}
			case 'p':
			{
			}
			case 'r':
			{
			}
			case 's':
			{
			}
			case 't':
			{
			}
			case 'u':
			{
			}
			case 'w':
			{
			}
			case 'x':
			{
			}
			case 'z':
			{
			}
		}
	}
	else
	{
		// Report an error
		errorAtCurrent(parser, "Unknown instruction.");
	}
	expressionStatement();
}

static void labelStatement(Parser* parser, Scanner* scanner, VM* vm)
{
	char* word = (char*)malloc((parser->previous.length + 1) * sizeof(char));
	word = parser->previous.start;
	word[length] = '\0';

	if(isMacro(parser))
	{
		advance(parser, scanner);
		
		// Somethings needs to be done here
		addToTable(vm->table, word, parser->current.start); 
	}
	insutrctionStatement(parser, scanner, vm);
}

ParseRule rules[] =
{
	[TOKEN_INSTRUCTION]					= {NULL,			NULL,		PREC_NONE},
//	[TOKEN_GENERAL_REGISTER],
	[TOKEN_SPECIAL_REGISTER]		= {NULL, 			NULL,		PREC_NONE},
	[TOKEN_IMMEDIATE] 					= {immediate,	NULL, 	PREC_NONE},
	[TOKEN_LABEL] 							= {label,  		NULL, 	PREC_NONE},
	[TOKEN_STRING]							= {string, 		NULL, 	PREC_NONE},
	[TOKEN_COMMA] 							= {NULL, 			NULL, 	PREC_NONE},
	[TOKEN_CONSTANT]						= {constant,	NULL,		PREC_NONE},
	[TOKEN_ERR] 								= {NULL,  		NULL, 	PREC_NONE},
	[TOKEN_EOF] 								= {NULL,  		NULL, 	PREC_NONE},
	[TOKEN_PLUS]								= {unary,			binary,	PREC_},
	[TOKEN_MINUS] 							= {unary,			binary, PREC_},
	[TOKEN_SLASH] 							= {NULL,			binary, PREC_},
	[TOKEN_STAR]								= {NULL,			binary, PREC_},
	[TOKEN_AROUND]							= {NULL,			NULL,		PREC_NONE},
	[TOKEN_DSLASH]							= {unary,			binary,	PREC_},
	[TOKEN_MOD] 								= {NULL,			binary, PREC_},
	[TOKEN_LSHIFT]							= {NULL,			binary,	PREC_},
	[TOKEN_RSHIFT]							= {NULL,			binary, PREC_},
	[TOKEN_AND] 							  = {NULL,			binary, PREC_},
	[TOKEN_OR] 									= {NULL,			binary, PREC_},
	[TOKEN_XOR] 								= {NULL,			binary, PREC_},
	[TOKEN_SEMICOLON] 					= {NULL,			NULL,	PREC_NONE},
	[TOKEN_REGISTER]						= {unary,			NULL, PREC_},
	[TOKEN_COMPLEMENT]	        = {unary,			NULL,	PREC_},
}

void parse(Parser* parser, Scanner* scanner, VM* vm)
{
	advance(parser, scanner);

	if(parser.previous == TOKEN_LABEL)
	{
		labelStatement(parser, scanner, vm);
	}
	else
	{
		instructionStatement(parser, scanner, vm);
	}
}
