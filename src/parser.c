#include <asm-generic/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"

Parser parser;
Table instr_indices;

static void advance();
static int32_t expression();
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
    // Debug: Print token details
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
    errorAt(&parser.current, message);	
}

static void error(const char* message)
{
    errorAt(&parser.previous, message);
}

static bool check(TokenType type)
{
    return parser.current.type == type;
}

static void advance()
{
    parser.previous = parser.current;

    for (;;)
    {
        Token new_token = scanToken();
        parser.current = new_token;
        //printToken(parser.current);
        if(parser.current.type != TOKEN_ERR)
            break;

        errorAtCurrent("Unknown expression");
    }
}

static void consume(TokenType type, const char* message)
{
    if (parser.current.type == type)
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

static char* getTokenString(Token* token)
{
    int word_length = token->length + 1; // +1 for '\0' 
    char* word = (char*)malloc(word_length * sizeof(char));
    strncpy(word, token->start, token->length);
    word[token->length] = '\0';

    return word;
}

static void emitByte(uint8_t byte)
{
    fwrite(&byte, sizeof(uint8_t), 1, parser.fp);
}

static bool match(char* instruction, int32_t start, char* pattern, int32_t length)
{
    if(memcmp(instruction + start, pattern, length) == 0)
        return true;

    return false;
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

static int32_t symbol()
{
    char* symbol = getTokenString(&parser.current);
    stringToLowercase(symbol);

    EntryValue value;
    if(parser.symbols->size != 0 && findInTable(parser.symbols, symbol, parser.current.length, &value))
    {
        if(value.type == TYPE_INT)
            return value.int_value;
    }

    printf("Unknown symbol.\n");
    return -1;
}

static int32_t number()
{
    return parseNumber(getTokenString(&parser.current));
}

static int32_t location()
{
    return parseNumber(getTokenString(&parser.current));
}

static int32_t term(bool* isImmediate)
{
    // possible terms: primaries(a symbol, constant, @, and strongOperators enclosed in parentheses or a unary operator followed by a primary
    // unary operators: +, -, ~, $
    int a;
    if(isUnary())
    {
        TokenType token_type = parser.current.type;
        advance();
        a = term(isImmediate);

        if(parser.panic_mode) // something went wrong in term()
            return -1;

        switch(token_type)
        {
            case TOKEN_PLUS:
                if (!isImmediate)
                {
                    errorAtCurrent("Can only apply '+' unary operator to immediate value.");
                    return -1;
                }
                break;
            case TOKEN_MINUS:
                if (!isImmediate)
                {
                    errorAtCurrent("Can only apply '-' unary operator to immediate value.");
                    return -1;
                }
                a = -a;
                break;
            case TOKEN_COMPLEMENT:
                if (!isImmediate)
                {
                    errorAtCurrent("Can only apply '~' unary operator to immediate value.");
                    return -1;
                }
                a = ~a;
                break;
            case TOKEN_GENERAL_REGISTER:
                *isImmediate = false;
                a = number();
                break;
            default:
                errorAtCurrent("Unknown term!");
                return -1;
        }
    }
    else if(parser.current.type == TOKEN_LPAREN)
    {
        advance(); // skip the '('
        a = expression();
        advance(); // skip the ')'
    }
    else if(parser.current.type == TOKEN_AROUND)
    {
        *isImmediate = false;
        a = location();
    }
    else if(parser.current.type == TOKEN_CONSTANT)
    {
        char* hexa = getTokenString(&parser.current);
        hexa[0] = '0';
        a = parseHexNumber(hexa);
        free(hexa);
    }
    else if(parser.current.type == TOKEN_LABEL)
    {
        a = symbol();
    }
    else if(parser.current.type == TOKEN_IMMEDIATE)
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

static int32_t strongOperators(bool* isImmediate)
{
    // strong binary operators: *,/,//,%,<<,>>,&
    int32_t a = term(isImmediate);

    if(parser.panic_mode) // something went wrong in term
        return -1;

    advance();
    bool bIsImmediate = false;
    int32_t b;
    while(!isAtEnd() && !isRightParen() && !isWeakOperator() && !check(TOKEN_COMMA) && !isEndOfInstr())
    {
        *isImmediate = true;
        switch(parser.current.type)
        {
            case TOKEN_STAR:
                b = term(&bIsImmediate);
                if(!isImmediate || !bIsImmediate)
                {
                    errorAtCurrent("Can only multiply immediate values in operand.");
                    return -1;
                }
                a *= b;
                break;
            case TOKEN_AND:
                b = term(&bIsImmediate);
                if(!isImmediate || !bIsImmediate)
                {
                    errorAtCurrent("Can apply bitwise AND operation only on immediate values.");
                    return -1;
                }
                a &= b;
                break;
            case TOKEN_MOD:
                b = term(&bIsImmediate);
                if(!isImmediate || !bIsImmediate)
                {
                    errorAtCurrent("Can use mod operation only on immediate values.");
                    return -1;
                }
                a %= b;
                break;
            case TOKEN_DSLASH:
                b = term(&bIsImmediate);
                if (!isImmediate || !bIsImmediate)
                {
                    errorAtCurrent("");
                    return -1;
                }
                // TO DO
                break;
            case TOKEN_SLASH:
                b = term(&bIsImmediate);
                if (!isImmediate || !bIsImmediate)
                {
                    errorAtCurrent("Can apply division only on immediate values.");
                    return -1;
                }
                a /= b;
                break;
            case TOKEN_LSHIFT:
                b = term(&bIsImmediate);
                if (!isImmediate || !bIsImmediate)
                {
                    errorAtCurrent("Can apply shift operation only on immediate values.");
                    return -1;
                }
                a <<= b;
                break;
            case TOKEN_RSHIFT:
                b = term(&bIsImmediate);
                if (!isImmediate || !bIsImmediate)
                {
                    errorAtCurrent("Can apply shift operation only on immediate values.");
                    return -1;
                }
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

static int32_t expression(bool* isImmediate)
{
    // weak binary operators: +,-,|,^
    bool aIsImmediate = false;
    int a = strongOperators(&aIsImmediate);
    
    if (parser.panic_mode)
        return -1;

    //advance();
    bool bIsImmediate = false;
    int b;
    while(!isAtEnd() && !isRightParen() && !check(TOKEN_COMMA) && !isEndOfInstr())
    {
        switch(parser.current.type)
        {
            case TOKEN_PLUS:
                b = strongOperators(&bIsImmediate);
                if (!aIsImmediate && !bIsImmediate)
                {
                    errorAtCurrent("Can do addition only between immediates, and between a register and an immediate.");
                    return -1;
                }
                *isImmediate = aIsImmediate && bIsImmediate;
                a += b;
                break;
            case TOKEN_MINUS:
                b = strongOperators(&bIsImmediate);
                if ((!bIsImmediate) ^ (!aIsImmediate))
                {
                    errorAtCurrent("Can do subtraction only between immediates, or between registers.");
                    return -1;
                }
                *isImmediate = true;
                a -= b;
                break;
            case TOKEN_OR:
                b = strongOperators(&bIsImmediate);
                if (!bIsImmediate || !aIsImmediate)
                {
                    errorAtCurrent("Can apply bitwise OR operation only on immediates.");
                    return -1;
                }
                *isImmediate = true;
                a |= b;
                break;
            case TOKEN_XOR:
                b = strongOperators(&bIsImmediate);
                if (!bIsImmediate || !aIsImmediate)
                {
                    errorAtCurrent("Can apply bitwise XOR operation only on immediates.");
                    return -1;
                }
                *isImmediate = true;
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

static uint32_t commaStatement()
{
    bool isImmediate = false;
    uint32_t operand1 = expression(&isImmediate);

    if (parser.panic_mode)
        return 0;

    uint32_t operand2 = 0;
    uint32_t operand3 = 0;
    uint8_t arity = 1;

    if(check(TOKEN_COMMA)) // 2 arguments
    {
        advance();
        operand2 = expression(&isImmediate);

        if (parser.panic_mode)
            return 0;

        arity++;
    }
    if(check(TOKEN_COMMA)) // 3 arguments
    {
        advance();
        operand3 = expression(&isImmediate);
        
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

static void instructionStatement(char* label, uint64_t label_length)
{
    if (label != NULL)
    {

    }

    char* instruction = getTokenString(&parser.current);
    EntryValue emitValue;

    uint8_t bytes[4];
    
    if(findInTable(&instr_indices, instruction, strlen(instruction), &emitValue) != false)
    {
        bytes[0] = (uint8_t)emitValue.int_value;
    }
    else
    {
        errorAtCurrent("Unknown instruction.");
    }

    advance(); // consume TOKEN_INSTRUCTION
    uint32_t temp = commaStatement();
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
        emitByte(bytes[i]);
    }

    advance(); // consume TOKEN_ENDLINE

    free(instruction);
}

static void isStatement(char* label, uint64_t label_length)
{
    advance(); // consume TOKEN_IS 

    if (label != NULL)
    {
        uint64_t labelLocation = scanner.start - scanner.source;
        addToTable_uint64_t(parser.symbols, label, label_length, labelLocation); 
    }

    while(!check(TOKEN_ENDLINE) && !check(TOKEN_EOF))
    {
        advance();
    }

    if (check(TOKEN_ENDLINE))
        advance();
}

static void gregStatement(char* label, uint64_t label_length)
{
    advance(); // consume TOKEN_GREG

    if (label != NULL)
    {
    }
}

static void locStatement(char* label, uint64_t label_length)
{
    advance(); // consume TOKEN_LOC

    if (label != NULL)
    {
    }
}

static void byteStatement(char* label, uint64_t label_length)
{
    advance(); // consume TOKEN_BYTE

    if (label != NULL)
    {
    }
}

static void wydeStatement(char* label, uint64_t label_length)
{
    advance(); // consume TOKEN_WYDE

    if (label != NULL)
    {
    }
}

static void tetraStatement(char* label,  uint64_t label_length)
{
    advance(); // consume TOKEN_TETRA

    if (label != NULL)
    {
    }
}

static void octaStatement(char* label, uint64_t label_length)
{
    advance(); // consume TOKEN_OCTA

    if (label != NULL)
    {
    }
}

static void prefixStatement(char* label, uint64_t label_length) // I'll implement these when everything else works :)
{
    advance(); // consume TOKEN_PREFIX

    if (label != NULL)
    {
    }
}

static void labelStatement()
{
    if(check(TOKEN_LABEL))
    {
        char* label = (char*)malloc(parser.previous.length * sizeof(char));
        strncpy(parser.previous.start, label, parser.previous.length);

        switch (parser.current.type)
        {
            case TOKEN_IS:
                isStatement(label, parser.current.length);
                break;
            case TOKEN_GREG:
                gregStatement(label, parser.current.length);
                break;
            case TOKEN_LOC:
                locStatement(label, parser.current.length);
                break;
            case TOKEN_BYTE:
                byteStatement(label, parser.current.length);
                break;
            case TOKEN_WYDE:
                wydeStatement(label, parser.current.length);
                break;
            case TOKEN_TETRA:
                tetraStatement(label, parser.current.length);
                break;
            case TOKEN_OCTA:
                octaStatement(label, parser.current.length);
                break;
            case TOKEN_PREFIX:
                prefixStatement(label, parser.current.length);
                break;
            case TOKEN_INSTRUCTION:
                instructionStatement(label, parser.current.length);
                break;
            default:
                errorAtCurrent("Invalid token type.");
        }
    }
    else
    {
        switch (parser.current.type)
        {
            case TOKEN_IS:
                isStatement(NULL, 0);
                break;
            case TOKEN_GREG:
                gregStatement(NULL, 0);
                break;
            case TOKEN_LOC:
                locStatement(NULL, 0);
                break;
            case TOKEN_BYTE:
                byteStatement(NULL, 0);
                break;
            case TOKEN_WYDE:
                wydeStatement(NULL, 0);
                break;
            case TOKEN_TETRA:
                tetraStatement(NULL, 0);
                break;
            case TOKEN_OCTA:
                octaStatement(NULL, 0);
                break;
            case TOKEN_PREFIX:
                prefixStatement(NULL, 0);
                break;
            case TOKEN_INSTRUCTION:
                instructionStatement(NULL, 0);
                break;
            default:
                errorAtCurrent("Invalid token type.");
                break;
        }
    }
}

static void semicolonStatement() // if you write macros/instructions on the same line
{
    parser.panic_mode = false;

    labelStatement();

    while(check(TOKEN_SEMICOLON))
    {
        advance(); // consume semicolon
        labelStatement();
    }
}

void initParser(char* output_file)
{
    parser.fp = fopen(output_file, "wb");
    parser.panic_mode = false;
    parser.symbols = (Table*)malloc(sizeof(Table));
    initTable(&instr_indices);
    initTable(parser.symbols);
     
    for(int i = 0; i < 256; i++)
    {
        addToTable_uint64_t(&instr_indices, instructions[i].name, strlen(instructions[i].name), i);
    }
}

void freeParser()
{
    freeTable(&instr_indices);
    free(parser.symbols);
    fclose(parser.fp);
}

void parse()
{
    advance();

    while(!check(TOKEN_EOF))
    {
        semicolonStatement();
    }
}
