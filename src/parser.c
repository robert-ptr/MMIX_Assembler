#include <asm-generic/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"

Parser parser;
Table instr_indices;

static void initStack(Stack* s)
{
    s->size = 0;
    s->top = 0;
    s->values = NULL;
}

static void freeStack(Stack* s)
{
    free(s->values);
    initStack(s);
}

static void addToStack(Stack* s, uint64_t value)
{
    if (s->top >= s->size)
    {
        uint64_t new_size = (s->size == 0 ? 8 : s->size * 2);

        uint64_t* new_list = (uint64_t*)malloc(new_size * sizeof(uint64_t));
        for (int i = 0; i < s->size; i++)
        {
            new_list[i] = s->values[i];
        }
        s->size = new_size;

        free(s->values);
        s->values = new_list;
    }

    s->values[s->top++] = value;
}

static uint64_t popStack(Stack* s)
{
    return s->values[(s->top--) - 1];
}

static uint64_t peekStack(Stack* s)
{
    return s->values[s->top - 1];
}

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
#ifdef MMIX_PARSER_DEBUG
        printf("%d %d %d\n", op1, op2, op3);
#endif
        error("Arguments of wrong size!");
        return false;
    }

    return true;
}

static int64_t symbol(bool* isImmediate)
{
    char* symbol = (char*)malloc((parser.current.length + parser.prefix_length + 1) * sizeof(char));
    strncpy(symbol, parser.current_prefix, parser.prefix_length);
    strncpy(symbol + parser.prefix_length, parser.current.start, parser.current.length);
    symbol[parser.current.length + parser.prefix_length] = '\0';
    stringToLowercase(symbol);

    TableData key;
    key.as_str.lexeme = symbol;
    key.as_str.n = strlen(symbol);
    key.type = TYPE_STR;

    TableData label_value;

    if (parser.aliases->size != 0 && findInTable(parser.aliases, &key, &label_value))
    {
        free(symbol);
        if (label_value.type == TYPE_INT)
            return label_value.as_int;
        else 
            errorAtCurrent("Invalid symbol!");
    }

    // for now I'll also accept locations (for JMP) for example, although I'm not sure you can use a location for, say, ADDU 
    // must check MMIXware
    if (parser.locations->size != 0 && findInTable(parser.locations, &key, &label_value))
    {
        free(symbol);
        if (label_value.type == TYPE_INT)
            return label_value.as_int;
        else
            errorAtCurrent("Invalid symbol!");
    }

    free(symbol);
    errorAtCurrent("Unknown symbol!");

    return -1;
}

static int64_t number()
{
    char* str = (char*)malloc((parser.current.length + 1) * sizeof(char));
    strncpy(str, parser.current.start, parser.current.length);
    str[parser.current.length] = '\0';
    int64_t n = parseNumber(str);
    free(str);
    return n;
}

static int64_t term(bool* isImmediate)
{
    // possible terms: primaries(a symbol, constant, @, and strongOperators enclosed in parentheses or a unary operator followed by a primary
    // unary operators: +, -, ~, $
    int a;
    if(isUnary)
    {
        while(isUnary())
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
        char* hex = (char*)malloc((parser.current.length + 1) * sizeof(char));
        hex[parser.current.length] = '\0';
        strncpy(hex, parser.current.start, parser.current.length);
        a = parseHexNumber(hex);
        free(hex);
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
    while(isStrongOperator())
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

static int64_t expression(bool* isImmediate) // instruction arguments may be expressions that need to be evaluated
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

static void instructionStatement(char* label, uint64_t label_length) // fundamental block of the assembler
                                                                     // each MMIX program is basically a list of instructions
                                                                     // and some of them have labels in front
                                                                     // IS,GREG,PREFIX,BYTE,WYDE,TETRA,OCTA,LOC,LOCAL don't count as instructions 
                                                                     // they are more like macros
{
    if (label != NULL) // will be used for control flow
    {
        TableData key, value;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;


        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            addToTable(parser.locations, &key, &value);

            free(label); // add to table performs hard copy
        }
        else 
        {
            free(label);
            errorAtCurrent("Symbol redefinition!");
        }
    }

    advance(); // consume TOKEN_INSTRUCTION
    
    bool op1_immediate = false;
    bool op2_immediate = false;
    bool op3_immediate = false;

    Token instr_token = parser.previous;

    uint8_t opcode;

    char* instruction = (char*)malloc((parser.previous.length + 1) * sizeof(char));
    instruction[parser.previous.length] = '\0';
    strncpy(instruction, parser.previous.start, parser.previous.length);

    stringToLowercase(instruction);
    TableData emitValue, key;
    key.as_str.lexeme = instruction;
    key.as_str.n = strlen(instruction);
    key.type = TYPE_STR;

    
    if(findInTable(&instr_indices, &key, &emitValue))
    {
        opcode = (uint8_t)emitValue.as_int;
    }
    else
    {
        errorAt(&instr_token, "Unknown instruction.");
        free(instruction);
        return;
    }

    int64_t operand1 = expression(&op1_immediate);
    int64_t operand2 = 0;
    int64_t operand3 = 0;
    uint8_t arity = 1;
    
    if (operand1 < 0) // something went wrong;
    {
        free(instruction);
        return;
    }
    
    if(check(TOKEN_COMMA)) // 2 arguments
    {
        advance();
        operand2 = expression(&op2_immediate);

        if (operand2 < 0) // something went wrong
        {
            free(instruction);
            return;
        }

        arity++;
    }
    if(check(TOKEN_COMMA)) // 3 arguments
    {
        advance();
        operand3 = expression(&op3_immediate);

        if (operand3 < 0) // something went wrong 
        {
            free(instruction);
            return;
        }

        arity++;
    }

    if(arity == 3 && (instructions[opcode].description >> 15) & 0b1) // the instruction accepts 3 arguments
        ;
    else if (arity == 2 && (instructions[opcode].description >> 14) & 0b1) // instruction accepts 2 arguments 
        ;
    else if (arity == 1)
    {
        if (strcmp(instructions[opcode].name, "jmp") &&
            strcmp(instructions[opcode].name, "trap") &&
            strcmp(instructions[opcode].name, "trip") && 
            strcmp(instructions[opcode].name, "resume") &&
            strcmp(instructions[opcode].name, "unsave") &&
            strcmp(instructions[opcode].name, "sync"))  // strcmp returns 0 if there is a match  
        {    
            errorAt(&instr_token, "Wrong number of arguments for instruction!");
            free(instruction);
            return;
        }
    }
    else 
    {
        errorAt(&instr_token, "Wrong number of arguments for instruction!");
        free(instruction);
        return;
    }
    
    
    
    if(check(TOKEN_COMMA)) // wrong number of arguments
    {
        free(instruction);
        errorAt(&instr_token, "Wrong number of arguments! Maximum 3.");
        return;
    }
    
    if (strncmp(instr_token.start, "set", instr_token.length) == 0 && op2_immediate != true)
    {
        opcode = 0xC0;
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

    if (!checkOperandSizes(operand1, operand2, operand3))
    {
        free(instruction);
        return;
    }

    while (!check(TOKEN_ENDLINE) && !check(TOKEN_EOF) && !check(TOKEN_SEMICOLON)) // there can be comments right after the instruction
    {
        advance();
    }

    if (parser.panic_mode)
    {
        free(instruction);
        return;
    }

    if (check(TOKEN_ENDLINE)) // consume endline
        advance();

    free(instruction);

    emitByte(opcode);
    emitByte(operand1);
    emitByte(operand2);
    emitByte(operand3);
    parser.current_location += 4;
}

static void isStatement(char* label, uint64_t label_length) // MMIXAL statement that associates a name with a value
{
    advance(); // consume TOKEN_IS 

    if (label != NULL)
    {
        uint64_t label_value = expression(NULL);
       
        TableData key;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            TableData value;

            value.as_int = label_value;
            value.type = TYPE_INT;

            addToTable(parser.aliases, &key, &value); 

            free(label); // add to table performs hard copy
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

static void gregStatement(char* label, uint64_t label_length) // MMIXAL statement to make a new general register available
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
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            value.as_int = parser.general_reg;
            value.type = TYPE_INT;

            addToTable(parser.aliases, &key, &value); 

            free(label); // add to table performs hard copy

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

static void locStatement(char* label, uint64_t label_length) // MMIXAL statement that changes the value of the current location 
                                                             // it's not a jump, it's just that the following instructions will have a different
                                                             // location address
{
    advance(); // consume TOKEN_LOC

    uint64_t loc = expression(NULL);

    if (label != NULL)
    {
        TableData key, value;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            value.as_int = loc;
            value.type = TYPE_INT;

            addToTable(parser.locations, &key, &value);

            free(label); // add to table performs hard copy
        }
        else 
        {
            free(label);
            errorAtCurrent("Symbol redefinition!");
        }
    }
}

static void byteStatement(char* label, uint64_t label_length) // MMIXAL statement for allocating memory 1 byte at a time
{
    advance(); // consume TOKEN_BYTE

    if (label != NULL)
    {
        TableData key, value;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            addToTable(parser.locations, &key, &value); 

            free(label); // add to table performs hard copy
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

static void wydeStatement(char* label, uint64_t label_length) // MMIXAL statement for allocating memory 2 bytes at a time
{
    advance(); // consume TOKEN_WYDE

    if (label != NULL)
    {
        TableData key, value;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;


        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            addToTable(parser.locations, &key, &value);

            free(label); // add to table performs hard copy
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
            parser.current_location += 2;
        }
        advance();
    }
}

static void tetraStatement(char* label, uint64_t label_length) // MMIXAL statement for allocating memory 4 bytes at a time
{
    advance(); // consume TOKEN_TETRA

    if (label != NULL)
    {
        TableData key, value;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        uint64_t labelLocation = parser.current_location;
        if(!findInTable(parser.locations, &key, NULL) && !findInTable(parser.aliases, &key, NULL))
        {
            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            addToTable(parser.locations, &key, &value);

            free(label); // add to table performs hard copy
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
            parser.current_location += 4;
        }
        advance();
    }
}

static void octaStatement(char* label, uint64_t label_length) // MMIXAL statement for allocating memory 8 bytes at a time
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

            free(label); // add to table performs hard copy
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
            int64_t temp = expression(NULL); // I'm not ok with this, it should be an unsigned value 
                                             // but I need to change some things first
            
            emitByte((uint8_t)(temp >> 56 & 0xFF));
            emitByte((uint8_t)(temp >> 48 & 0xFF));
            emitByte((uint8_t)(temp >> 40 & 0xFF));
            emitByte((uint8_t)(temp >> 32 & 0xFF));
            emitByte((uint8_t)(temp >> 24 & 0xFF));
            emitByte((uint8_t)(temp >> 16 & 0xFF));
            emitByte((uint8_t)(temp >> 8 & 0xFF));
            emitByte((uint8_t)(temp & 0xFF));
            parser.current_location += 8;
        }
        advance();
    }
}

static void prefixStatement(char* label, uint64_t label_length) // used to prevent naming conflicts, as the name says, a prefix is added to the names of labels
{
    advance(); // consume TOKEN_PREFIX
    /*
    if (parser.previous.start[parser.previous.length - 1] != ':')
    {
        errorAt(&parser.previous, "Prefix label must end with a ':'!"); // actually not true, but for now this is how it works
    }
    */
   
    uint64_t previous_prefix_length = parser.prefix_length;
    parser.prefix_length = parser.previous.length;

    if (parser.prefix_length == 1) // then prefix is ':' and the prefix is reset
    {
        strncpy(parser.current_prefix, parser.previous.start, parser.prefix_length);
    }
    else // it concatenates to the previously active prefix 
    {
        strncpy(parser.current_prefix + previous_prefix_length, parser.previous.start, parser.prefix_length); // change the prefix
        parser.prefix_length = previous_prefix_length + parser.prefix_length;
    }
}

static void labelStatement()
{
    if(check(TOKEN_LABEL)) // there is a label on this line
    {
        char* label = (char*)malloc((parser.current.length + parser.prefix_length) * sizeof(char));
        if (parser.current.length == 2) // check if it's a label of the form dH, dB, dF 
                                        // I still don't know if the prefix affects resolution for these kinds of labels 
                                        // I think so? After all, they are symbols. But I need to verify it.
        {
            if (parser.current.start[0] >= '0' && parser.current.start[0] <= '9')
            {
                if (parser.current.start[1] == 'h')
                {
                    addToStack(parser.local_labels[parser.current.start[0] - '0'], parser.current_location);
                }
                else if (parser.current.start[1] == 'b')
                {
                }
                else if (parser.current.start[1] == 'f')
                {
                }
            }
        }
        if (parser.current.start[0] != ':') // add the prefix to the label
        {
            strncpy(label, parser.current_prefix, parser.prefix_length);
            strncpy(label + parser.prefix_length, parser.current.start, parser.current.length);
        }
        else 
        {
            strncpy(label, parser.current.start, parser.current.length);
        }
        advance(); // consume label

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
                errorAtCurrent("Label field should be blank!");
                break;
            case TOKEN_INSTRUCTION:
                instructionStatement(label, parser.current.length);
                break;
            default:
                errorAtCurrent("Invalid token type.");
        }

        free(label);
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

    while(check(TOKEN_SEMICOLON)) // there is another statement on the same line
    {
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

    for (int i = 0; i < 10; i++)
    {
        parser.local_labels[i] = (Stack*)malloc(sizeof(Stack));
        initStack(parser.local_labels[i]);
    }

    for(int i = 0; i < 150; i++)
    {
        TableData key, value;
        
        if (instructions[i].name == NULL) {
            fprintf(stderr, "Error: instructions[%d].name is NULL\n", i);
            exit(EXIT_FAILURE);
        }

        key.as_str.lexeme = strdup(instructions[i].name);

        if (key.as_str.lexeme == NULL) {
            fprintf(stderr, "Error: Failed to allocate memory for string key\n");
            exit(EXIT_FAILURE);
        }
        
        key.as_str.n = strlen(instructions[i].name);
        key.type = TYPE_STR;
       
        value.as_int = instructions[i].description & 0xFF;
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

    for (int i = 0; i < 10; i++)
    {
        freeStack(parser.local_labels[i]);
        free(parser.local_labels[i]);
    }
   
    fclose(parser.fp);
}

void parse() // main parse function
{
    advance(); // makes sure parser.current is not NULL

    while(!check(TOKEN_EOF)) // while not at the end of the program, continue
    {
        semicolonStatement();
    }
}
