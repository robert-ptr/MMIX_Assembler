#include <asm-generic/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"

Parser parser;
Table instr_indices;

static void advance();
static bool isAtEnd();
static bool check(TokenType type);

static void triggerPanicMode() // something wrong happened, trigger panic mode, and try to get to the next line
{
    while(!isAtEnd() && !check(TOKEN_ENDLINE))
    {
        advance();
    }

    if(check(TOKEN_ENDLINE))
        advance();
}

static void errorAt(Token* token, const char* message)
{
	fprintf(stderr, "[line %d] Error", token->line);
	if (token->type == TOKEN_EOF) 
	{
		fprintf(stderr, " at end");
	} 
	else if (token->type == TOKEN_ERR) 
	{
	    fprintf(stderr, "Unknown token.");	
	} 
	else 
	{
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	}
	fprintf(stderr, ": %s\n", message);
    triggerPanicMode();
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
	return check(TOKEN_PLUS) || check(TOKEN_MINUS) || check(TOKEN_COMPLEMENT) || check(TOKEN_GENERAL_REGISTER);
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

static char* getTokenString(Token* token)
{
    int word_length = token->length + 1; // +1 for '\0' 
    char* word = (char*)malloc(word_length * sizeof(char));
    strncpy(word, token->start, token->length);
    word[token->length] = '\0';

    return word;
}

static int32_t symbol()
{
	// repair this function
	char* symbol = parser.current->start;
    stringToLowercase(&symbol);

	EntryValue value;
	if(findInTable(&parser.table, symbol, &value))
    {
        if(value.type == TYPE_INT)
            return value.int_value;
    }

    printf("Unknown symbol.\n");
    return -1;
}

static int32_t number()
{
	uint8_t n = parseNumber(getTokenString(parser.current));

	return n;
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
			case TOKEN_GENERAL_REGISTER:
				a = number();
				break;
            default:
                errorAtCurrent("Unknown term!");
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
        char* hexa = getTokenString(parser.current);
        hexa[0] = '0';
		a = fromHexadecimal(hexa);
        free(hexa);
	}
	else if(parser.current->type == TOKEN_LABEL)
	{
		a = symbol();
	}
	else if(parser.current->type == TOKEN_IMMEDIATE)
	{
		a = number();
	}
	else
	{
		// Report an error
		printf("Unknown term!\n");
		return -1;
	}
	return a;
}

static int32_t strongOperators()
{
	// strong binary operators: *,/,//,%,<<,>>,&
	int32_t a = term();
    advance();
	int32_t b;
	while(!isAtEnd() && !isRightParen() && !isWeakOperator() && !check(TOKEN_COMMA) && !check(TOKEN_ENDLINE))
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
            default:
                errorAtCurrent("Unknown strong operator!");
        }
	}

	return a;
}

static int32_t expression()
{
	// weak binary operators: +,-,|,^
	int a = strongOperators();
	int b;
	while(!isAtEnd() && !isRightParen() && !check(TOKEN_COMMA) && !check(TOKEN_ENDLINE))
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
            default:
                errorAtCurrent("Unknown operator!");
        }
	}

	return a;
}

static bool checkOperandSizes(uint32_t op1, uint32_t op2, uint32_t op3)
{
    if((op1 >> 8) + (op2 >> 8) + (op3 >> 8) != 0 )
    {
        error("Arguments of wrong size!");
        return false;
    }

    return true;
}

static void commaStatement(VM* vm)
{
	uint32_t operand1 = expression();
	uint32_t operand2 = 0;
	uint32_t operand3 = 0;
    uint8_t arity = 1;

	if(check(TOKEN_COMMA)) // 2 arguments
	{
		advance();
		operand2 = expression();
        arity++;
	}
    if(check(TOKEN_COMMA)) // 3 arguments
	{
		advance();
		operand3 = expression();
        arity++;
    }
    if(check(TOKEN_COMMA)) // wrong number of arguments
    {
        error("Wrong number of arguments! Maximum 3.");
    }

    if(arity == 1)
    {
        operand2 = operand1 >> 8 & 0xFF;
        operand3 = operand1 & 0xFF;
        operand1 >>= 16;
    }
    else if(arity == 2)
    {
        operand3 = operand2 & 0xFF;
        operand2 >>= 8;
    }

    checkOperandSizes(operand1, operand2, operand3);
    emitByte(vm, operand1);
    emitByte(vm, operand2); 
    emitByte(vm, operand3);
}

static void instructionStatement(VM* vm)
{
    char* instruction = getTokenString(parser.current);
    EntryValue emitValue;
    if(findInTable(&instr_indices, instruction, &emitValue) != false)
    {
        Byte byte = (uint8_t)emitValue.int_value;
        emitByte(vm, byte);	// check the operands in order to determine if you need to add 1
    }
    else
    {
        errorAtCurrent("Unknown instruction.");
    }

    advance();
    commaStatement(vm);
    advance();

    free(instruction);
}

static void labelStatement(VM* vm)
{
    char* label = getTokenString(parser.previous); // maybe add it to a vector with all the labels?

	// a label for a register,a value or something along these lines
	addToTable_uint64_t(&parser.table, label, parser.line); // add line position
	
    if(check(TOKEN_INSTRUCTION))
	    instructionStatement(vm);
    else
    {
        switch(parser.current->type) // check for MMIXAL stuff
        {
            case TOKEN_IS:
                break;
            case TOKEN_GREG:
                break;
            case TOKEN_BYTE:
                break;
            case TOKEN_WYDE:
                break;
            case TOKEN_TETRA:
                break;
            case TOKEN_OCTA:
                break;
            case TOKEN_PREFIX:
                break;
            case TOKEN_IMMEDIATE:
                break;
            case TOKEN_CONSTANT:
                break;
            case TOKEN_GENERAL_REGISTER:
                break;
            case TOKEN_SPECIAL_REGISTER:
                break;
            case TOKEN_AROUND:
                break;
            case TOKEN_LABEL:   // label for another label?
                break;
            default:
                errorAtCurrent("Unknown literal after label!");
        }
    }

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
	parser.previous = (Token*)malloc(sizeof(Token));
	parser.current = (Token*)malloc(sizeof(Token));
	parser.line = 1;
    initTable(&parser.table);
    initTable(&instr_indices);
     
    for(int i = 0; i < 256; i++)
    {
        addToTable_uint64_t(&instr_indices, instructions[i].name, i);
    }
}

void freeParser()
{
    freeTable(&parser.table);
    freeTable(&instr_indices);

    free(parser.current);
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
