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
    parser.panic_mode = true;
    while(!isAtEnd() && !check(TOKEN_ENDLINE) && !check(TOKEN_SEMICOLON)) // find next instruction, if available
    {
        advance();
    }

    if(!isAtEnd())
    {
        advance();
    }
}

static void errorAt(Token* token, const char* message)
{
	fprintf(stderr, "[line %d char %d] Error", token->line, token->offset);
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

static bool isEndOfInstr()
{
    return check(TOKEN_ENDLINE) || check(TOKEN_SEMICOLON);
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
	char* symbol = parser.current->start;
    stringToLowercase(symbol);

	EntryValue value;
	if(findInTable(&parser.table, symbol, strlen(symbol), &value))
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

        if(parser.panic_mode) // something went wrong in term()
            return -1;

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
                return -1;
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
		a = parseHexNumber(hexa);
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
		errorAtCurrent("Unknown term!\n");
		return -1;
	}
	return a;
}

static int32_t strongOperators()
{
	// strong binary operators: *,/,//,%,<<,>>,&
	int32_t a = term();

    if(parser.panic_mode) // something went wrong in term
        return -1;

    advance();
	int32_t b;
	while(!isAtEnd() && !isRightParen() && !isWeakOperator() && !check(TOKEN_COMMA) && !isEndOfInstr())
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
                return -1;
        }
	}

    if (parser.panic_mode) // something went wrong when calling term() for b 
        return -1;

	return a;
}

static int32_t expression()
{
	// weak binary operators: +,-,|,^
	int a = strongOperators();

    if (parser.panic_mode)
        return -1;

	int b;
	while(!isAtEnd() && !isRightParen() && !check(TOKEN_COMMA) && !isEndOfInstr())
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
                return -1;
        }
	}

    if (parser.panic_mode)
        return -1;

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

static uint32_t commaStatement(VM* vm)
{
	uint32_t operand1 = expression();

    if (parser.panic_mode)
        return 0;

	uint32_t operand2 = 0;
	uint32_t operand3 = 0;
    uint8_t arity = 1;

	if(check(TOKEN_COMMA)) // 2 arguments
	{
		advance();
		operand2 = expression();

        if (parser.panic_mode)
            return 0;

        arity++;
	}
    if(check(TOKEN_COMMA)) // 3 arguments
	{
		advance();
		operand3 = expression();
        
        if (parser.panic_mode)
            return 0;

        arity++;
    }
    
    if(check(TOKEN_COMMA)) // wrong number of arguments
    {
        errorAtCurrent("Wrong number of arguments! Maximum 3.");
        return 0;
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
    
    return operand1 + (operand2 << 8) + (operand3 << 16);
}

static void instructionStatement(VM* vm)
{
    char* instruction = getTokenString(parser.current);
    EntryValue emitValue;

    Byte bytes[4];
    if(findInTable(&instr_indices, instruction, strlen(instruction), &emitValue) != false)
    {
        bytes[0] = (uint8_t)emitValue.int_value;
    }
    else
    {
        errorAtCurrent("Unknown instruction.");
    }

    advance();
    uint32_t temp = commaStatement(vm);
    bytes[1] = temp & 0xFF;
    bytes[2] = temp >> 8 & 0xFF;
    bytes[3] = temp >> 16;
    if (parser.panic_mode)
    {
        free(instruction);
        return;
    }

    for (int i = 0; i < 4; i++)
    {
        emitByte(vm, bytes[i]);
    }

    advance();

    free(instruction);
}

static void labelStatement(VM* vm)
{
    char* label = getTokenString(parser.previous); // maybe add it to a vector with all the labels?

	// a label for a register,a value or something along these lines
	addToTable_uint64_t(&parser.table, label, strlen(label), parser.line); // add line position
	
    if(check(TOKEN_INSTRUCTION))
    {
	    instructionStatement(vm);

        if (parser.panic_mode)
            return;
    }
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
    parser.panic_mode = false;
	if(parser.current->type == TOKEN_LABEL)
	{
		labelStatement(vm);
	}
	else
	{
		instructionStatement(vm);
	}
	
	while(check(TOKEN_SEMICOLON))
	{
		advance(); // consume semicolon
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
    parser.panic_mode = false;
    initTable(&parser.table);
    initTable(&instr_indices);
     
    for(int i = 0; i < 256; i++)
    {
        addToTable_uint64_t(&instr_indices, instructions[i].name, strlen(instructions[i].name), i);
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
