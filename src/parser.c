#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"

Parser parser;
Table instr_indices;

static void init_stack(Stack* s)
{
    s->size = 0;
    s->top = 0;
    s->values = NULL;
}

static void free_stack(Stack* s)
{
    free(s->values);
    init_stack(s);
}

static void add_to_stack(Stack* s, uint64_t value)
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

static uint64_t pop_stack(Stack* s)
{
    return s->values[(s->top--) - 1];
}

static uint64_t peek_stack(Stack* s)
{
    return s->values[s->top - 1];
}

static void advance();
static int64_t expression(bool* is_immediate);
static bool is_at_end();
static bool check(TokenType type);

static void trigger_panic_mode() // something wrong happened, trigger panic mode, and try to get to the next line
{
    parser.panic_mode = true;
    while(!is_at_end() && !check(TOKEN_ENDLINE) && !check(TOKEN_SEMICOLON)) // find next instruction, if available
    {
        advance();
    }

    if(!is_at_end())
    {
        advance();
    }
}

static void error_at(Token* token, const char* message)
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

    trigger_panic_mode();
}

static void error_at_current(const char* message)
{
    error_at(&parser.current, message);
}

static void error(const char* message)
{
    error_at(&parser.previous, message);
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
        Token new_token = scan_token();
        parser.current = new_token;
        print_token(&parser.current);
        if(!check(TOKEN_ERR))
            break;

        error_at_current("Unknown expression");
    }
}

static bool is_strong_operator()
{
    return check(TOKEN_STAR) || check(TOKEN_MOD) || check(TOKEN_AND) || check(TOKEN_SLASH) || check(TOKEN_LSHIFT) || check(TOKEN_RSHIFT);
}

static bool is_weak_operator()
{
    return check(TOKEN_PLUS) || check(TOKEN_MINUS) || check(TOKEN_OR) || check(TOKEN_XOR);
}

static bool is_at_end()
{
    return check(TOKEN_EOF);
}

static bool is_unary()
{
    return check(TOKEN_PLUS) || check(TOKEN_MINUS) || check(TOKEN_COMPLEMENT) || check(TOKEN_GENERAL_REGISTER);
}

static void emit_byte(uint8_t byte)
{
    fwrite(&byte, sizeof(uint8_t), 1, parser.fp);
}

static bool check_operand_sizes(uint32_t op1, uint32_t op2, uint32_t op3)
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

static int64_t symbol()
{
    char* symbol = (char*)malloc((parser.current.length + parser.prefix_length + 1) * sizeof(char));
    strncpy(symbol, parser.current_prefix, parser.prefix_length);
    strncpy(symbol + parser.prefix_length, parser.current.start, parser.current.length);
    symbol[parser.current.length + parser.prefix_length] = '\0';
    string_to_lowercase(symbol);

    TableData key;
    key.as_str.lexeme = symbol;
    key.as_str.n = strlen(symbol);
    key.type = TYPE_STR;

    TableData label_value;

    if (parser.aliases->size != 0 && find_in_table(parser.aliases, &key, &label_value))
    {
        free(symbol);
        if (label_value.type == TYPE_INT)
            return label_value.as_int;
        else
            error_at_current("Invalid symbol!");
    }

    // for now I'll also accept locations (for JMP) for example, although I'm not sure you can use a location for, say, ADDU
    // must check MMIXware
    if (parser.locations->size != 0 && find_in_table(parser.locations, &key, &label_value))
    {
        free(symbol);
        if (label_value.type == TYPE_INT)
            return label_value.as_int;
        else
            error_at_current("Invalid symbol!");
    }

    free(symbol);
    error_at_current("Unknown symbol!");

    return -1;
}

static int64_t number()
{
    char* str = (char*)malloc((parser.current.length + 1) * sizeof(char));
    strncpy(str, parser.current.start, parser.current.length);
    str[parser.current.length] = '\0';
    int64_t n = parse_number(str);
    free(str);
    return n;
}

static int64_t term(bool* is_immediate)
{
    // possible terms: primaries(a symbol, constant, @, and strongOperators enclosed in parentheses or a unary operator followed by a primary
    // unary operators: +, -, ~, $
    int a;
    if(is_unary())
    {
        while(is_unary())
        {
            TokenType token_type = parser.current.type;
            advance();
            a = term(is_immediate);

            if(parser.panic_mode) // something went wrong in term()
                return -1;

            switch(token_type)
            {
                case TOKEN_PLUS:
                    if (!is_immediate)
                    {
                        error_at_current("Can only apply '+' unary operator to immediate value.");
                        return -1;
                    }
                    break;
                case TOKEN_MINUS:
                    if (!is_immediate)
                    {
                        error_at_current("Can only apply '-' unary operator to immediate value.");
                        return -1;
                    }
                    a = -a;
                    break;
                case TOKEN_COMPLEMENT:
                    if (!is_immediate)
                    {
                        error_at_current("Can only apply '~' unary operator to immediate value.");
                        return -1;
                    }
                    a = ~a;
                    break;
                case TOKEN_GENERAL_REGISTER:
                    *is_immediate = false;
                    a = number();
                    break;
                default:
                    error_at_current("Unknown term!");
                    return -1;
            }
        }
    }
    else if(check(TOKEN_LPAREN))
    {
        advance(); // skip the '('
        a = expression(is_immediate);
        advance(); // skip the ')'
    }
    else if(check(TOKEN_AROUND))
    {
        *is_immediate = false;
        a = parser.current_location;
    }
    else if(check(TOKEN_CONSTANT))
    {
        char* hex = (char*)malloc((parser.current.length + 1) * sizeof(char));
        hex[parser.current.length] = '\0';
        strncpy(hex, parser.current.start, parser.current.length);
        a = parse_hex_number(hex);
        free(hex);
    }
    else if(check(TOKEN_LABEL))
    {
        a = symbol(is_immediate);
    }
    else if(check(TOKEN_IMMEDIATE))
    {
        a = number();
    }
    else
    {
        // Report an error
        error_at_current("Unknown term!\n");
        return -1;
    }
    return a;
}

static int64_t strong_operators(bool* is_immediate)
{
    // strong binary operators: *,/,//,%,<<,>>,&
    int32_t a = term(is_immediate);

    if(parser.panic_mode) // something went wrong in term
        return -1;

    advance();
    bool b_is_immediate = false; // TO DO: what exactly is up with this?
    int32_t b;
    while(is_strong_operator())
    {
        *is_immediate = true;
        switch(parser.current.type)
        {
            case TOKEN_STAR:
                b = term(&b_is_immediate);
                if(!is_immediate || !b_is_immediate)
                {
                    error_at_current("Can only multiply immediate values in operand.");
                    return -1;
                }
                a *= b;
                break;
            case TOKEN_AND:
                b = term(&b_is_immediate);
                if(!is_immediate || !b_is_immediate)
                {
                    error_at_current("Can apply bitwise AND operation only on immediate values.");
                    return -1;
                }
                a &= b;
                break;
            case TOKEN_MOD:
                b = term(&b_is_immediate);
                if(!is_immediate || !b_is_immediate)
                {
                    error_at_current("Can use mod operation only on immediate values.");
                    return -1;
                }
                a %= b;
                break;
            case TOKEN_DSLASH:
                b = term(&b_is_immediate);
                if (!is_immediate || !b_is_immediate)
                {
                    error_at_current("");
                    return -1;
                }
                // TO DO
                break;
            case TOKEN_SLASH:
                b = term(&b_is_immediate);
                if (!is_immediate || !b_is_immediate)
                {
                    error_at_current("Can apply division only on immediate values.");
                    return -1;
                }
                a /= b;
                break;
            case TOKEN_LSHIFT:
                b = term(&b_is_immediate);
                if (!is_immediate || !b_is_immediate)
                {
                    error_at_current("Can apply shift operation only on immediate values.");
                    return -1;
                }
                a <<= b;
                break;
            case TOKEN_RSHIFT:
                b = term(&b_is_immediate);
                if (!is_immediate || !b_is_immediate)
                {
                    error_at_current("Can apply shift operation only on immediate values.");
                    return -1;
                }
                a >>= b;
                break;
            default:
                error_at_current("Unknown strong operator!");
                return -1;
        }
    }

    if (parser.panic_mode) // something went wrong when calling term() for b
        return -1;

    return a;
}

static int64_t expression(bool* is_immediate) // instruction arguments may be expressions that need to be evaluated
{
    // weak binary operators: +,-,|,^
    bool a_is_immediate = false;
    int64_t a = strong_operators(&a_is_immediate);

    if (parser.panic_mode)
        return -1;

    //advance();
    bool b_is_immediate = false;
    int64_t b;
    while(is_weak_operator())
    {
        switch(parser.current.type)
        {
            case TOKEN_PLUS:
                b = strong_operators(&b_is_immediate);
                if (!a_is_immediate && !b_is_immediate)
                {
                    error_at_current("Can do addition only between immediates, and between a register and an immediate.");
                    return -1;
                }
                *is_immediate = a_is_immediate && b_is_immediate;
                a += b;
                break;
            case TOKEN_MINUS:
                b = strong_operators(&b_is_immediate);
                if ((!b_is_immediate) ^ (!a_is_immediate))
                {
                    error_at_current("Can do subtraction only between immediates, or between registers.");
                    return -1;
                }
                *is_immediate = true;
                a -= b;
                break;
            case TOKEN_OR:
                b = strong_operators(&b_is_immediate);
                if (!b_is_immediate || !a_is_immediate)
                {
                    error_at_current("Can apply bitwise OR operation only on immediates.");
                    return -1;
                }
                *is_immediate = true;
                a |= b;
                break;
            case TOKEN_XOR:
                b = strong_operators(&b_is_immediate);
                if (!b_is_immediate || !a_is_immediate)
                {
                    error_at_current("Can apply bitwise XOR operation only on immediates.");
                    return -1;
                }
                *is_immediate = true;
                a ^= b;
                break;
            default:
                error_at_current("Unknown operator!");
                return -1;
        }
    }

    if (parser.panic_mode)
        return -1;

    return a;
}

static bool accepts_single_operand(uint8_t opcode)
{
    switch (opcode)
    {
        case 0xF0: // jmp
        case 0x00: // trap
        case 0xFF: // trip
        case 0xF9: // resume
        case 0xFB: // unsave
        case 0xFC: // sync
            return true;
        default:
            return false;
    }
}

static void instruction_statement(char* label, uint64_t label_length) // fundamental block of the assembler
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


        if(!find_in_table(parser.locations, &key, NULL) && !find_in_table(parser.aliases, &key, NULL))
        {
            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            add_to_table(parser.locations, &key, &value);
        }
        else
        {
            error_at_current("Symbol redefinition!");
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

    string_to_lowercase(instruction);
    TableData emitValue, key;
    key.as_str.lexeme = instruction;
    key.as_str.n = strlen(instruction);
    key.type = TYPE_STR;


    if(find_in_table(&instr_indices, &key, &emitValue))
    {
        uint64_t instruction_index = emitValue.as_int;
        opcode = (uint8_t)(instructions[instruction_index].description & 0xFF);
    }
    else
    {
        error_at(&instr_token, "Unknown instruction.");
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

    bool valid_arity =
        (arity == 1 && accepts_single_operand(opcode)) ||
        (arity == 2 && ACCEPTS_2_ARGS(instructions[emitValue.as_int].description)) ||
        (arity == 3 && ACCEPTS_3_ARGS(instructions[emitValue.as_int].description));

    if (!valid_arity)
    {
        error_at(&instr_token, "Wrong number of arguments for instruction!");
        free(instruction);
        return;
    }

    if(check(TOKEN_COMMA)) // wrong number of arguments
    {
        free(instruction);
        error_at(&instr_token, "Wrong number of arguments! Maximum 3.");
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

    if (!check_operand_sizes(operand1, operand2, operand3))
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

    emit_byte(opcode);
    emit_byte(operand1);
    emit_byte(operand2);
    emit_byte(operand3);
    parser.current_location += 4;
}

static void is_statement(char* label, uint64_t label_length) // MMIXAL statement that associates a name with a value
{
    advance(); // consume TOKEN_IS

    if (label != NULL)
    {
        uint64_t label_value = expression(NULL);

        TableData key;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        if(!find_in_table(parser.locations, &key, NULL) && !find_in_table(parser.aliases, &key, NULL))
        {
            TableData value;

            value.as_int = label_value;
            value.type = TYPE_INT;

            add_to_table(parser.aliases, &key, &value);
        }
        else
        {
            error_at_current("Symbol redefinition!");
        }
    }

    while(!check(TOKEN_ENDLINE) && !check(TOKEN_EOF))
    {
        advance();
    }

    if (check(TOKEN_ENDLINE))
        advance();
}

static void greg_statement(char* label, uint64_t label_length) // MMIXAL statement to make a new general register available
{
    advance(); // consume TOKEN_GREG

    if(parser.general_reg > 16)
    {
        error_at_current("Can't use GREG for a general register with index over 16!");
        return;
    }

    if (label != NULL)
    {
        TableData key, value;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        if(!find_in_table(parser.locations, &key, NULL) && !find_in_table(parser.aliases, &key, NULL))
        {
            value.as_int = parser.general_reg;
            value.type = TYPE_INT;

            add_to_table(parser.aliases, &key, &value);

            parser.register_values[parser.general_reg] = expression(NULL);
            parser.general_reg++;
        }
        else
        {
            error_at_current("Symbol redefinition!");
        }
    }
    else
    {
        uint8_t greg = parser.general_reg;
        parser.register_values[parser.general_reg] = expression(NULL);

        parser.general_reg++;
    }
}

static void loc_statement(char* label, uint64_t label_length) // MMIXAL statement that changes the value of the current location
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

        if(!find_in_table(parser.locations, &key, NULL) && !find_in_table(parser.aliases, &key, NULL))
        {
            value.as_int = loc;
            value.type = TYPE_INT;

            add_to_table(parser.locations, &key, &value);
        }
        else
        {
            error_at_current("Symbol redefinition!");
        }
    }

    parser.current_location = loc;

    while(!check(TOKEN_ENDLINE) && !check(TOKEN_EOF))
    {
        advance();
    }

    if (check(TOKEN_ENDLINE))
        advance();
}

static void byte_statement(char* label, uint64_t label_length) // MMIXAL statement for allocating memory 1 byte at a time
{
    advance(); // consume TOKEN_BYTE

    if (label != NULL)
    {
        TableData key, value;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        if(!find_in_table(parser.locations, &key, NULL) && !find_in_table(parser.aliases, &key, NULL))
        {
            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            add_to_table(parser.locations, &key, &value);
        }
        else
        {
            error_at_current("Symbol redefinition!");
        }
    }

    while(!check(TOKEN_ENDLINE) && !check(TOKEN_SEMICOLON) && !check(TOKEN_EOF))
    {
        if(check(TOKEN_STRING))
        {
            if (parser.current.length == 0)
            {
                error_at_current("Can't create an empty string!");
                return;
            }
            for(int i = 0; i < parser.current.length; i++)
            {
                emit_byte((uint8_t)parser.current.start[i]);
                parser.current_location++;
            }
        }
        else
        {
            int64_t temp = expression(NULL);
            if (temp > 255 || temp < 0)
            {
                error_at_current("Expression can't fit in one byte!");
            }

            emit_byte((uint8_t)(temp & 0xFF));
            parser.current_location++;
        }
        advance();
    }
}

static void wyde_statement(char* label, uint64_t label_length) // MMIXAL statement for allocating memory 2 bytes at a time
{
    advance(); // consume TOKEN_WYDE

    if (label != NULL)
    {
        TableData key, value;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;


        if(!find_in_table(parser.locations, &key, NULL) && !find_in_table(parser.aliases, &key, NULL))
        {
            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            add_to_table(parser.locations, &key, &value);
        }
        else
        {
            error_at_current("Symbol redefinition!");
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
                error_at_current("Can't create an empty string!");
                return;
            }
            for (int i = 0; i < parser.current.length; i++)
            {
                emit_byte((uint8_t)parser.current.start[i]);
                parser.current_location++;
            }
        }
        else
        {
            int64_t temp = expression(NULL);
            if (temp > 65535 || temp < 0)
            {
                error_at_current("Expression can't fit in two bytes!");
            }

            emit_byte((uint8_t)(temp >> 8 & 0xFF));
            emit_byte((uint8_t)(temp >> 16 & 0xFF));
            parser.current_location += 2;
        }
        advance();
    }
}

static void tetra_statement(char* label, uint64_t label_length) // MMIXAL statement for allocating memory 4 bytes at a time
{
    advance(); // consume TOKEN_TETRA

    if (label != NULL)
    {
        TableData key, value;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;

        uint64_t labelLocation = parser.current_location;
        if(!find_in_table(parser.locations, &key, NULL) && !find_in_table(parser.aliases, &key, NULL))
        {
            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            add_to_table(parser.locations, &key, &value);
        }
        else
        {
            error_at_current("Symbol redefinition!");
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
                error_at_current("Can't create an empty string!");
                return;
            }
            for(int i = 0; i < parser.current.length; i++)
            {
                emit_byte((uint8_t)parser.current.start[i]);
                parser.current_location++;
            }
        }
        else
        {
            int64_t temp = expression(NULL);
            if ((temp >> 32) > 0 || temp < 0)
            {
                error_at_current("Expression can't fit in four byte!");
            }

            emit_byte((uint8_t)(temp >> 24 & 0xFF));
            emit_byte((uint8_t)(temp >> 16 & 0xFF));
            emit_byte((uint8_t)(temp >> 8 & 0xFF));
            emit_byte((uint8_t)(temp & 0xFF));
            parser.current_location += 4;
        }
        advance();
    }
}

static void octa_statement(char* label, uint64_t label_length) // MMIXAL statement for allocating memory 8 bytes at a time
{
    advance(); // consume TOKEN_OCTA

    if (label != NULL)
    {
        TableData key;
        key.as_str.lexeme = label;
        key.as_str.n = label_length;
        key.type = TYPE_STR;


        uint64_t labelLocation = parser.current_location;
        if(!find_in_table(parser.locations, &key, NULL) && !find_in_table(parser.aliases, &key, NULL))
        {
            TableData key, value;
            key.as_str.lexeme = label;
            key.as_str.n = label_length;
            key.type = TYPE_STR;

            value.as_int = parser.current_location;
            value.type = TYPE_INT;

            add_to_table(parser.locations, &key, &value);
        }
        else
        {
            error_at_current("Symbol redefinition!");
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
                error_at_current("Can't create an empty string!");
                return;
            }
            for(int i = 0; i < parser.current.length; i++)
            {
                emit_byte((uint8_t)parser.current.start[i]);
                parser.current_location++;
            }
        }
        else
        {
            int64_t temp = expression(NULL); // I'm not ok with this, it should be an unsigned value
                                             // but I need to change some things first

            emit_byte((uint8_t)(temp >> 56 & 0xFF));
            emit_byte((uint8_t)(temp >> 48 & 0xFF));
            emit_byte((uint8_t)(temp >> 40 & 0xFF));
            emit_byte((uint8_t)(temp >> 32 & 0xFF));
            emit_byte((uint8_t)(temp >> 24 & 0xFF));
            emit_byte((uint8_t)(temp >> 16 & 0xFF));
            emit_byte((uint8_t)(temp >> 8 & 0xFF));
            emit_byte((uint8_t)(temp & 0xFF));
            parser.current_location += 8;
        }
        advance();
    }
}

static void prefix_statement(char* label, uint64_t label_length) // used to prevent naming conflicts, as the name says, a prefix is added to the names of labels
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

static void label_statement()
{
    if(check(TOKEN_LABEL)) // there is a label on this line
    {
        uint64_t label_length = parser.current.length;
        bool add_prefix = parser.current.start[0] != ':';
        if (add_prefix)
            label_length += parser.prefix_length;

        char* label = (char*)malloc((label_length + 1) * sizeof(char));
        if (parser.current.length == 2) // check if it's a label of the form dH, dB, dF
                                        // I still don't know if the prefix affects resolution for these kinds of labels
                                        // I think so? After all, they are symbols. But I need to verify it.
        {
            if (parser.current.start[0] >= '0' && parser.current.start[0] <= '9')
            {
                if (parser.current.start[1] == 'h')
                {
                    add_to_stack(parser.local_labels[parser.current.start[0] - '0'], parser.current_location);
                }
                else if (parser.current.start[1] == 'b')
                {
                }
                else if (parser.current.start[1] == 'f')
                {
                }
            }
        }
        if (add_prefix) // add the prefix to the label
        {
            strncpy(label, parser.current_prefix, parser.prefix_length);
            strncpy(label + parser.prefix_length, parser.current.start, parser.current.length);
        }
        else
        {
            strncpy(label, parser.current.start, parser.current.length);
        }
        label[label_length] = '\0';
        string_to_lowercase(label);
        advance(); // consume label

        switch (parser.current.type)
        {
            case TOKEN_IS:
                is_statement(label, label_length);
                break;
            case TOKEN_GREG:
                greg_statement(label, label_length);
                break;
            case TOKEN_LOC:
                loc_statement(label, label_length);
                break;
            case TOKEN_BYTE:
                byte_statement(label, label_length);
                break;
            case TOKEN_WYDE:
                wyde_statement(label, label_length);
                break;
            case TOKEN_TETRA:
                tetra_statement(label, label_length);
                break;
            case TOKEN_OCTA:
                octa_statement(label, label_length);
                break;
            case TOKEN_PREFIX:
                error_at_current("Label field should be blank!");
                break;
            case TOKEN_INSTRUCTION:
                instruction_statement(label, label_length);
                break;
            default:
                error_at_current("Invalid token type.");
        }

        free(label);
    }
    else
    {
        switch (parser.current.type)
        {
            case TOKEN_IS:
                is_statement(NULL, 0);
                break;
            case TOKEN_GREG:
                greg_statement(NULL, 0);
                break;
            case TOKEN_LOC:
                loc_statement(NULL, 0);
                break;
            case TOKEN_BYTE:
                byte_statement(NULL, 0);
                break;
            case TOKEN_WYDE:
                wyde_statement(NULL, 0);
                break;
            case TOKEN_TETRA:
                tetra_statement(NULL, 0);
                break;
            case TOKEN_OCTA:
                octa_statement(NULL, 0);
                break;
            case TOKEN_PREFIX:
                prefix_statement(NULL, 0);
                break;
            case TOKEN_INSTRUCTION:
                instruction_statement(NULL, 0);
                break;
            default:
                error_at_current("Invalid token type.");
                break;
        }
    }
}

static void semicolon_statement() // if you write macros/instructions on the same line
{
    parser.panic_mode = false;

    label_statement();

    while(check(TOKEN_SEMICOLON)) // there is another statement on the same line
    {
        label_statement();
    }
}

void init_parser(char* output_file)
{
    parser.fp = fopen(output_file, "wb");
    parser.panic_mode = false;
    parser.locations = (Table*)malloc(sizeof(Table));
    parser.aliases = (Table*)malloc(sizeof(Table));
    parser.general_reg = 1;
    parser.prefix_length = 1;
    parser.current_prefix[0] = ':';

    init_table(&instr_indices);
    init_table(parser.locations);
    init_table(parser.aliases);

    for (int i = 0; i < 10; i++)
    {
        parser.local_labels[i] = (Stack*)malloc(sizeof(Stack));
        init_stack(parser.local_labels[i]);
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
       
        value.as_int = i;
        value.type = TYPE_INT;
        
        add_to_table(&instr_indices, &key, &value);

        free(key.as_str.lexeme);
    }
}

void free_parser()
{
    free_table(&instr_indices);
    free_table(parser.aliases);
    free_table(parser.locations);
   
    free(parser.aliases);
    free(parser.locations);

    for (int i = 0; i < 10; i++)
    {
        free_stack(parser.local_labels[i]);
        free(parser.local_labels[i]);
    }
   
    fclose(parser.fp);
}

void parse() // main parse function
{
    advance(); // makes sure parser.current is not NULL

    while(!check(TOKEN_EOF)) // while not at the end of the program, continue
    {
        semicolon_statement();
    }
}
