#include <asm-generic/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"

Parser parser;
Table instr_indices;

static void advance();
static int64_t expression(bool* isImmediate);
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
        printToken(&parser.current);
        if(!check(TOKEN_ERR))
            break;

        errorAtCurrent("Unknown expression");
    }
}

static void consume(TokenType type, const char* message)
{
    if (check(type))
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

static int64_t symbol(bool* isImmediate)
{
    char* symbol = getTokenString(&parser.current);
    stringToLowercase(symbol);

    TableData key;
    key.as_str.lexeme = symbol;
    key.as_str.n = strlen(symbol);
    key.type = TYPE_STR;

    TableData togo; // I would have used the name goto, but y'know... goto is taken
    if(parser.aliases->size != 0 && findInTable(parser.aliases, &key, &togo))
    {
        if(togo.type == TYPE_INT)
            return togo.as_int;

        parser.current.start -= togo.as_int;

        int32_t val = expression(isImmediate);
        if (val < 0) 
        {
            errorAtCurrent("MMIXAL IS Statement accepts only unsigned values.");
        }

        parser.current.start += togo.as_int;
        return val;
    }

    printf("Unknown symbol.\n");
    return -1;
}

static int64_t number()
{
    return parseNumber(getTokenString(&parser.current));
}

static int64_t term(bool* isImmediate)
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
    else if(check(TOKEN_LPAREN))
    {
        advance(); // skip the '('
        a = expression(isImmediate);
        advance(); // skip the ')'
    }
    else if(check(TOKEN_AROUND))
    {
        *isImmediate = false;
        a = parser.current_location;
    }
    else if(check(TOKEN_CONSTANT))
    {
        a = parseHexNumber(getTokenString(&parser.current));
    }
    else if(check(TOKEN_LABEL))
    {
        a = symbol(isImmediate);
    }
    else if(check(TOKEN_IMMEDIATE))
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

static int64_t strongOperators(bool* isImmediate)
{
    // strong binary operators: *,/,//,%,<<,>>,&
    int32_t a = term(isImmediate);

    if(parser.panic_mode) // something went wrong in term
        return -1;

    advance();
    bool bIsImmediate = false;
    int32_t b;
    if(isStrongOperator())
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

static int64_t expression(bool* isImmediate)
{
    // weak binary operators: +,-,|,^
    bool aIsImmediate = false;
    int a = strongOperators(&aIsImmediate);
    
    if (parser.panic_mode)
        return -1;

    //advance();
    bool bIsImmediate = false;
    int b;
    while(isWeakOperator())
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

static uint32_t commaStatement(int index)
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
  
    for (int i = 0; i < 3; i++)
    {
        if(instructions[index].arity[i] == 0)
            errorAtCurrent("Wrong number of arguments!");
        else if(instructions[index].arity[i] == arity)
            break;
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

    while (!check(TOKEN_ENDLINE) && !check(TOKEN_EOF) && !check(TOKEN_SEMICOLON)) // there can be comments right after the instruction
    {
        advance();
    }

    return operand1 + (operand2 << 8) + (operand3 << 16);
}

static void instructionStatement(char* label, uint64_t label_length)
{
    if (label != NULL)
    {

    }

    char* instruction = getTokenString(&parser.current);
    TableData emitValue, key;
    key.as_str.lexeme = instruction;
    key.as_str.n = strlen(instruction);
    key.type = TYPE_STR;

    uint8_t bytes[4];
    
    if(findInTable(&instr_indices, &key, &emitValue) != false)
    {
        bytes[0] = (uint8_t)emitValue.as_int;
    }
    else
    {
        errorAtCurrent("Unknown instruction.");
    }

    // findInTable(&instr_indices, key, &index); // doesn't this do a similiar thing to the findInTable above?
    advance(); // consume TOKEN_INSTRUCTION
    
    uint32_t temp = commaStatement(bytes[0]);
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
       
        TableData key;
        key.as_str.lexeme = (char*)malloc(label_length * sizeof(char));
        strncpy(key.as_str.lexeme, label, label_length);
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            TableData value;

            value.as_int = labelLocation;
            value.type = TYPE_INT;

            addToTable(parser.aliases, &key, &value); 
        }
        else 
        {
            free(label);
            errorAtCurrent("Symbol redefinition!");
        }
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

    if(parser.general_reg > 16)
    {
        errorAtCurrent("Can't use GREG for a general register with index over 16!");
        return;
    }

    if (label != NULL)
    { 
        TableData key, value;
        key.as_str.lexeme = strdup(label);
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            value.as_int = parser.general_reg;
            value.type = TYPE_INT;

            addToTable(parser.aliases, &key, &value); 

            parser.register_values[parser.general_reg] = expression(NULL);
            
            parser.general_reg++;
        }
        else 
        {
            free(label);
            errorAtCurrent("Symbol redefinition!");
        }
    }
    else 
    {
        uint8_t greg = parser.general_reg;
        parser.register_values[parser.general_reg] = expression(NULL); 
        
        parser.general_reg++;
    }
}

static void locStatement(char* label, uint64_t label_length)
{
    advance(); // consume TOKEN_LOC

    uint64_t loc = expression(NULL);

    if (label != NULL)
    {
        TableData key, value;
        key.as_str.lexeme = strdup(label);
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            value.as_int = loc;
            value.type = TYPE_INT;

            addToTable(parser.locations, &key, &value); 
        }
        else 
        {
            free(label);
            errorAtCurrent("Symbol redefinition!");
        }
    }
}

static void byteStatement(char* label, uint64_t label_length)
{
    advance(); // consume TOKEN_BYTE

    if (label != NULL)
    {
        TableData key, value;
        key.as_str.lexeme = strdup(label);
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            addToTable(parser.locations, &key, &value); 
        }
        else 
        {
            free(label);
            errorAtCurrent("Symbol redefinition!");
        }
    }

    while(!check(TOKEN_ENDLINE) && !check(TOKEN_SEMICOLON) && !check(TOKEN_EOF))
    {
        if(check(TOKEN_STRING))
        {
            if (parser.current.length == 0)
            {
                errorAtCurrent("Can't create an empty string!");
                return;
            }
            for(int i = 0; i < parser.current.length; i++)
            {
                emitByte((uint8_t)parser.current.start[i]); 
                parser.current_location++;
            }
        }
        else
        {
            int64_t temp = expression(NULL);
            if (temp > 255 || temp < 0)
            {
                errorAtCurrent("Expression can't fit in one byte!");
            }
            
            emitByte((uint8_t)(temp & 0xFF));
            parser.current_location++;
        }
        advance();
    }
}

static void wydeStatement(char* label, uint64_t label_length)
{
    advance(); // consume TOKEN_WYDE

    if (label != NULL)
    {
        TableData key, value;
        key.as_str.lexeme = strdup(label);
        key.as_str.n = label_length;
        key.type = TYPE_STR;


        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            addToTable(parser.locations, &key, &value); 
        }
        else 
        {
            free(label);
            errorAtCurrent("Symbol redefinition!");
        }
    }

    if (parser.current_location % 2)
        parser.current_location++;
    
    while (!check(TOKEN_ENDLINE) && !check(TOKEN_SEMICOLON) && !check(TOKEN_EOF))
    {
        if (check(TOKEN_STRING))
        {
            if (parser.current.length == 0)
            {
                errorAtCurrent("Can't create an empty string!");
                return;
            }
            for (int i = 0; i < parser.current.length; i++)
            {
                emitByte((uint8_t)parser.current.start[i]); 
                parser.current_location++;
            }
        }
        else
        {
            int64_t temp = expression(NULL);
            if (temp > 65535 || temp < 0)
            {
                errorAtCurrent("Expression can't fit in two bytes!");
            }
            
            emitByte((uint8_t)(temp >> 8 & 0xFF));
            emitByte((uint8_t)(temp >> 16 & 0xFF));
            parser.current_location++;
        }
        advance();
    }
}

static void tetraStatement(char* label, uint64_t label_length)
{
    advance(); // consume TOKEN_TETRA

    if (label != NULL)
    {
        TableData key, value;
        key.as_str.lexeme = strdup(label);
        key.as_str.n = label_length;
        key.type = TYPE_STR;


        uint64_t labelLocation = parser.current_location;
        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            addToTable(parser.locations, &key, &value); 
        }
        else 
        {
            free(label);
            errorAtCurrent("Symbol redefinition!");
        }
    }
    
    while (parser.current_location % 4)
        parser.current_location++;

    while(!check(TOKEN_ENDLINE) && !check(TOKEN_SEMICOLON) && !check(TOKEN_EOF))
    {
        if(check(TOKEN_STRING))
        {
            if (parser.current.length == 0)
            {
                errorAtCurrent("Can't create an empty string!");
                return;
            }
            for(int i = 0; i < parser.current.length; i++)
            {
                emitByte((uint8_t)parser.current.start[i]); 
                parser.current_location++;
            }
        }
        else
        {
            int64_t temp = expression(NULL);
            if ((temp >> 32) > 0 || temp < 0)
            {
                errorAtCurrent("Expression can't fit in four byte!");
            }
            
            emitByte((uint8_t)(temp >> 24 & 0xFF));
            emitByte((uint8_t)(temp >> 16 & 0xFF));
            emitByte((uint8_t)(temp >> 8 & 0xFF));
            emitByte((uint8_t)(temp & 0xFF));
            parser.current_location++;
        }
        advance();
    }
}

static void octaStatement(char* label, uint64_t label_length)
{
    advance(); // consume TOKEN_OCTA

    if (label != NULL)
    {
        TableData key;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;


        uint64_t labelLocation = parser.current_location;
        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            TableData key, value;
            key.as_str.lexeme = label;
            key.as_str.n = label_length;
            key.type = TYPE_STR;

            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            addToTable(parser.locations, &key, &value); 
        }
        else 
        {
            free(label);
            errorAtCurrent("Symbol redefinition!");
        }
    }
    
    while (parser.current_location % 7)
        parser.current_location++;

    while(!check(TOKEN_ENDLINE) && !check(TOKEN_SEMICOLON) && !check(TOKEN_EOF))
    {
        if(check(TOKEN_STRING))
        {
            if (parser.current.length == 0)
            {
                errorAtCurrent("Can't create an empty string!");
                return;
            }
            for(int i = 0; i < parser.current.length; i++)
            {
                emitByte((uint8_t)parser.current.start[i]); 
                parser.current_location++;
            }
        }
        else
        {
            int64_t temp = expression(NULL); // I'm not ok with, it should be an unsigned value 
                                             // but I need to change some things first
            
            emitByte((uint8_t)(temp >> 56 & 0xFF));
            emitByte((uint8_t)(temp >> 48 & 0xFF));
            emitByte((uint8_t)(temp >> 40 & 0xFF));
            emitByte((uint8_t)(temp >> 32 & 0xFF));
            emitByte((uint8_t)(temp >> 24 & 0xFF));
            emitByte((uint8_t)(temp >> 16 & 0xFF));
            emitByte((uint8_t)(temp >> 8 & 0xFF));
            emitByte((uint8_t)(temp & 0xFF));
            parser.current_location++;
        }
        advance();
    }
}

static void prefixStatement(char* label, uint64_t label_length) // I'll implement these when everything else works :)
{
    advance(); // consume TOKEN_PREFIX
    parser.prefix_length = parser.previous.length; 
    strncpy(parser.current_prefix, parser.previous.start, parser.prefix_length); // change the prefix

    if (label != NULL) // WIP
    {
        TableData key;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;


        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            TableData key, value;
            key.as_str.lexeme = label;
            key.as_str.n = label_length;
            key.type = TYPE_STR;

            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            addToTable(parser.locations, &key, &value); 
        }
        else 
        {
            free(label); // if the label weren't taken we would have freed it when we call freeTable()
            errorAtCurrent("Symbol redefinition!");
        }
    }
}

static void labelStatement()
{
    if(check(TOKEN_LABEL))
    {
        char* label = (char*)malloc((parser.current.length + parser.prefix_length) * sizeof(char));
        strncpy(label, parser.current_prefix, parser.prefix_length);
        strncpy(label + parser.prefix_length, parser.current.start, parser.current.length);
        advance();

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
    parser.locations = (Table*)malloc(sizeof(Table));
    parser.aliases = (Table*)malloc(sizeof(Table));
    parser.general_reg = 1;
    parser.prefix_length = 1;
    parser.current_prefix[0] = ':';

    initTable(&instr_indices);
    initTable(parser.locations);
    initTable(parser.aliases);

    for(int i = 0; i < 256; i++)
    {
        TableData key, value;
        key.as_str.lexeme = strdup(instructions[i].name);

        if (key.as_str.lexeme == NULL) {
            fprintf(stderr, "Error: Failed to allocate memory for string key\n");
            exit(EXIT_FAILURE);
        }
        if (instructions[i].name == NULL) {
            fprintf(stderr, "Error: instructions[%d].name is NULL\n", i);
            exit(EXIT_FAILURE);
        }
        key.as_str.n = strlen(instructions[i].name);
        key.type = TYPE_STR;
       
        value.as_int = i;
        value.type = TYPE_INT;
        
        addToTable(&instr_indices, &key, &value);

        free(key.as_str.lexeme);
    }
}

void freeParser()
{
    freeTable(&instr_indices);
    freeTable(parser.aliases);
    freeTable(parser.locations);
   
    free(parser.aliases);
    free(parser.locations);

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
