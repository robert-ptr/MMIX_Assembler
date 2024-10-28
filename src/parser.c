#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"

static void errorAt(Token* token, const char* message)
{
	fprintf(stderr, "[line %d] Error", token->line);
	if (token->type == TOKEN_EOF) 
	{
		fprintf(stderr, " at end");
	} 
	else if (token->type == TOKEN_ERR) 
	{
		
	} 
	else 
	{
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	}
	fprintf(stderr, ": %s\n", message);
	parser.hadError = true;
}

static void errorAtCurrent(const char* message)
{
	errorAt(parser.current, message);	
}

static void error(const char* message)
{
	errorAt(parser.previous, message);
}
static bool check(TokenType type)
{
	return parser.current->type == type;
}

static void advance()
{
	parser.previous = parser.current;

	for (;;)
	{
		*parser.current = scanToken();
		if(parser.current->type != TOKEN_ERR)
			break;

		errorAtCurrent("Unknown expression");
	}
}

static void consume(TokenType type, const char* message)
{
	if (parser.current->type == type)
	{
		advance();
		return;
	}

	errorAtCurrent(message);
}

static bool isStrongOperator()
{
	return check(TOKEN_STAR) || check(TOKEN_MOD) || check(TOKEN_AND) || check(TOKEN_SLASH) || check(TOKEN_LSHIFT) || check(TOKEN_RSHIFT);
}

static bool isWeakOperator()
{
	return check(TOKEN_PLUS) || check(TOKEN_MINUS) || check(TOKEN_OR) || check(TOKEN_XOR);
}

static bool isAtEnd()
{
	return check(TOKEN_EOF);
}

static bool isUnary()
{
	return check(TOKEN_PLUS) || check(TOKEN_MINUS) || check(TOKEN_COMPLEMENT) || check(TOKEN_REGISTER);
}

static bool isRightParen()
{
	return check(TOKEN_RPAREN);
}

static void emitByte(VM* vm, Byte byte)
{
	addByte(vm->byte_set, byte);
}

static bool match(char* instruction, int32_t start, char* pattern, int32_t length)
{
	if(memcmp(instruction + start, pattern, length) == 0)
		return true;
	
	return false;
}

static bool isMacro()
{
	Token* token = parser.current;
	// IS, GREG, BYTE, WYDE, TETRA, OCTA
	if(token->length == 2 && memcmp(token->start, "is", 2))
	{
		return true;	
	}
	else if (token->length == 4)
	{
		if(match(token->start, 0, "greg", 4))
			return true;
		else if(match(token->start, 0, "byte", 4))
			return true;
		else if(match(token->start, 0, "wyde", 4))
			return true;
		else if(match(token->start, 0, "octa", 4))
			return true;
	}
	else if (token->length == 5 && memcmp(token->start, "tetra", 5))
		return true;

	return false;
}

static char* getTokenString(Token* token)
{
    int word_length = token->length + 1; // +1 for '\0' 
    char* word = (char*)malloc(word_length * sizeof(char));
    strncpy(word, token->start, token->length);
    word[token->length] = '\0';
}

static int32_t symbol()
{
	// repair this function
	stringToLowercase(&symbol);

	uint64_t value;
	if(findInTable(parser.table, symbol, &value))
		return value;
	else
	{
		printf("Unknown symbol.\n");
		return -1;
	}
}

static int32_t constant()
{
	int32_t n = 0;
	// repair this function
	
	return n;
}

static Byte reg()
{
	return constant();
}
static int32_t location()
{
}

static int32_t expression();

static int32_t term()
{
	// possible terms: primaries(a symbol, constant, @, and strongOperators enclosed in parentheses or a unary operator followed by a primary
	// unary operators: +, -, ~, $
	int a;
	if(isUnary())
	{
		TokenType token_type = parser.current->type;
		advance();
		a = term();
		switch(token_type)
		{
			case TOKEN_PLUS:
				break;
			case TOKEN_MINUS:
				a = -a;
				break;
			case TOKEN_COMPLEMENT:
				a = ~a;
				break;
			case TOKEN_REGISTER:
				a = reg();
				break;
		}
	}
	else if(parser.current->type == TOKEN_LPAREN)
	{
		advance(); // skip the '('
		a = expression();
		advance(); // skip the ')'
	}
	else if(parser.current->type == TOKEN_AROUND)
	{
		a = location();
	}
	else if(parser.current->type == TOKEN_CONSTANT)
	{
		advance();
        char* hexa = getTokenString(parser.current);
		a = fromHexadecimal(hexa);
        free(hexa);
	}
	else if(parser.current->type == TOKEN_LABEL)
	{
		a = symbol();
	}
	else if(parser.current->type == TOKEN_IMMEDIATE)
	{
		a = constant();
	}
	else
	{
		// Report an error
		printf("Unknown term.\n");
		return -1;
	}
	return a;
}

static int32_t strongOperators()
{
	// strong binary operators: *,/,//,%,<<,>>,&
	int32_t a = term();
	int32_t b;
	while(!isAtEnd() && !isRightParen() && !isWeakOperator())
	{
        switch(parser.current->type)
        {
            case TOKEN_STAR:
                b = term();
                a *= b;
                break;
            case TOKEN_AND:
                b = term();
                a &= b;
                break;
            case TOKEN_MOD:
                a %= b;
                break;
            case TOKEN_DSLASH:
                advance();
                b = term();
                break;
            case TOKEN_SLASH:
                b = term();
                a /= b;
                break;
            case TOKEN_LSHIFT:
                advance();
                b = term();
                a <<= b;
                break;
            case TOKEN_RSHIFT:
                advance();
                b = term();
                a >>= b;
                break;
        }
	}

	return a;
}

static int32_t expression()
{
	// weak binary operators: +,-,|,^
	int a = strongOperators();
	int b;
	while(!isAtEnd() && !isRightParen())
	{
        switch(parser.current->type)
        {
            case TOKEN_PLUS:
                b = strongOperators();
                a += b;
                break;
            case TOKEN_MINUS:
                b = strongOperators();
                a -= b;
                break;
            case TOKEN_OR:
                b = strongOperators();
                a |= b;
                break;
            case TOKEN_XOR:
                b = strongOperators();
                a ^= b;
                break;
        }
	}

	return a;
}

static void commaStatement(VM* vm)
{
	uint32_t operand1 = expression();
	uint32_t operand2;
	uint32_t operand3;

	if(check(TOKEN_COMMA))
	{
		advance();
		operand2 = expression();
	}
	else
	{
		emitByte(vm, operand1 >> 16);
		emitByte(vm, operand1 >> 8 & 0xFF);
		emitByte(vm, operand1 & 0xFF);
		return;
	}
	if(check(TOKEN_COMMA))
	{
		advance();
		operand3 = expression();
		if(operand1 > 255)
		{
            error("First argument is of the wrong size! Can be at most 8 bits.");
		}
		else
		{
			emitByte(vm, operand1);
		}

		if(operand2 > 255)
		{
            error("Second argument is of the wrong size! Can be at most 8 bits.");
		}
		else
		{
			emitByte(vm, operand2);
		}

		if(operand3 > 255)
		{
            error("Third argument is of the wrong size! Can be at most 8 bits.");
		}
		else
		{
			emitByte(vm, operand3);
		}
	}
	else
	{
		if(operand1 > 255)
		{
            error("First argument is of the wrong size! Can be at most 8 bits.");
		}
		else
		{
			emitByte(vm, operand1);
		}

		if(operand2 > 65535)
		{
            error("Second argument is of the wrong size! Can be at most 16 bits.");
		}
		else
		{
			emitByte(vm, operand2);
		}
	}

	if(check(TOKEN_COMMA))
	{
	}
}

static void instructionStatement(VM* vm)
{
	advance();
	TrieNode* root = getNode();
	createInstructionTrie(root);

	if(parser.current->type == TOKEN_INSTRUCTION)
	{
		char* instruction = getTokenString(parser.current);
        int32_t emitValue;
		if((emitValue = findWord(root, instruction)) != -1)
		{
			Byte byte = (uint8_t)emitValue;
			emitByte(vm, byte);	// check the operands in order to determine if you need to add 1
		}
		else
		{
			errorAtCurrent("Unknown instruction.");
		}

		free(instruction);
	}
	else
	{
		// Report an error
		errorAtCurrent("Unknown instruction.");
	}
	commaStatement(vm);
}

static void labelStatement(VM* vm)
{
    char* label = getTokenString(parser.previous);

	if(isMacro())
	{
		advance();

		// a label for a register,a value or something along these lines
		addToTable(parser.table, label, parser.line); // add line position
	}
	else
	{
		// that was a label for the current line
		
		instructionStatement(vm);
	}

    free(label);
}

static void semicolonStatement(VM* vm) // if you write macros/instructions on the same line
{
	if(parser.previous->type == TOKEN_LABEL)
	{
		labelStatement(vm);
	}
	else
	{
		instructionStatement(vm);
	}
	
	while(check(TOKEN_SEMICOLON))
	{
		advance();
		if(parser.previous->type == TOKEN_LABEL)
		{
			labelStatement(vm);
		}
		else
		{
			instructionStatement(vm);
		}
	}
}

void initParser()
{
	parser.hadError = false;
	parser.panicMode = false;
	parser.previous = NULL;
	parser.current = NULL;
	parser.line = 1;
    initTable(parser.table);
}

void freeParser()
{
	initParser(); // not much to be done here, so we just reset the parser
	freeTable(parser.table); // then free the table
}

void parse(VM* vm)
{
	advance();

	while(!check(TOKEN_EOF))
	{
		semicolonStatement(vm);
        parser.line++;
	}
}
