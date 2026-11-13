#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"

Scanner scanner;
uint32_t scanned_chars = 0;

static bool is_at_end()
{
    return *scanner.current == '\0';
}

static TokenType check_keyword()
{
    if (scanner.current - scanner.start == 2 && memcmp(scanner.start, "IS", 2) == 0)
        return TOKEN_IS;
    if (scanner.current - scanner.start == 4 && memcmp(scanner.start, "GREG", 4) == 0)
        return TOKEN_GREG;
    if (scanner.current - scanner.start == 4 && memcmp(scanner.start, "BYTE", 4) == 0)
        return TOKEN_BYTE;
    if (scanner.current - scanner.start == 4 && memcmp(scanner.start, "WYDE", 4) == 0)
        return TOKEN_WYDE;
    if (scanner.current - scanner.start == 5 && memcmp(scanner.start, "TETRA", 5) == 0)
        return TOKEN_TETRA;
    if (scanner.current - scanner.start == 4 && memcmp(scanner.start, "OCTA", 4) == 0)
        return TOKEN_OCTA;
    if (scanner.current - scanner.start == 6 && memcmp(scanner.start, "PREFIX", 6) == 0)
        return TOKEN_PREFIX;
    if (scanner.current - scanner.start == 5 && memcmp(scanner.start, "LOCAL", 5) == 0)
        return TOKEN_LOCAL;

    return TOKEN_LABEL;
}

static Token make_token(TokenType type)
{
    Token token;
    token.start = scanner.start;
    token.line = scanner.line;
    token.length = (int32_t)(scanner.current - scanner.start);
    token.offset = scanned_chars;
    token.type = type;

    scanned_chars += token.length;

    return token;
}

static Token error_token(char* message)
{
    Token token;
    token.start = message;
    token.line = scanner.line;
    token.length = strlen(message);
    token.type = TOKEN_ERR;

    return token;
}

static char advance()
{
    scanner.current++;
    return scanner.current[-1];
}

static char peek()
{
    return *scanner.current;
}

static void skip_whitespace()
{
    for(;;)
    {
        char c = peek();
        switch(c)
        {
            case ' ':
            case '\t':
            case '\r':
                advance();
                scanned_chars++;
                break;
            case '%': 
            {
                while(!is_at_end() && peek() != '\n')
                {
                    advance();
                    scanned_chars++;
                }
                break;
            }
            default:
                return;
        }
    }
}

static bool is_numeric()
{
    char c = peek();
    return c >= '0' && c <= '9';
}

static bool is_hexadecimal()
{
    char c = tolower(peek());
    return (c >= '0'  && c <= '9') || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e';
}

static bool is_alphanumeric()
{
    char c = peek();
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == ':';
}

static TokenType identifier_type()
{
    int32_t length = scanner.current - scanner.start;
    char* word = (char*)malloc((length + 1) * sizeof(char));
    word[length] = '\0';
    strncpy(word, scanner.start, length);
    string_to_lowercase(word);

    if(find_word(scanner.instruction_trie, word))
    {
        free(word);
        return TOKEN_INSTRUCTION;
    }
    
    free(word);
    return check_keyword();
}

void init_scanner(const char* source)
{
    scanner.source = source;
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;

    scanner.instruction_trie = get_node();
    for(int i = 0; i < 150; i++)
    {
        insert_node(scanner.instruction_trie, instructions[i].name, true);
    }
}

static Token immediate()
{
    while(is_numeric())
    {
        advance();
    }

    return make_token(TOKEN_IMMEDIATE);
}

static Token identifier()
{
    while(is_alphanumeric())
    {
        advance();
    }

    return make_token(identifier_type());
}

static Token general_register()
{
    advance();
    while(is_alphanumeric())
    {
        advance();
    }

    return make_token(TOKEN_GENERAL_REGISTER);
}

static Token special_register()
{
    advance();
    while(is_alphanumeric())
    {
        advance();
    }

    return make_token(TOKEN_SPECIAL_REGISTER);
}

static Token constant()
{
    advance();
    scanner.start++;
    while(is_hexadecimal())
    {
        advance();
    }

    return make_token(TOKEN_CONSTANT);
}

static Token string()
{
        advance();
        while(peek() != '"' && !is_at_end())
        {
            if(is_at_end() || peek() == '\n')
                return error_token("Unterminated string.");
            advance();
        }

        advance();

        Token token;
        token.start = scanner.start + 1;
        token.length = scanner.current - scanner.start - 2;
        token.type = TOKEN_STRING;
        token.line = scanner.line;

        return token;
}

static Token skip_to_end_of_line()
{
    while(peek() != '\0' && peek() != '\n')
        advance();
    if (peek() == '\n')
        advance();
    return scan_token();
}

Token scan_token()
{
    skip_whitespace();
    scanner.start = scanner.current;

    if(is_numeric()) // add support for special constructs dH, dF, dB
    {
        return immediate();
    }
    else if(is_alphanumeric())
    {
        return identifier();
    }

    if (is_at_end())
        return make_token(TOKEN_EOF);

    char c = advance();

    switch(c)
    {
        case '\n':
            scanner.line++;
            scanned_chars = 0;
            return make_token(TOKEN_ENDLINE);
        case ',': return make_token(TOKEN_COMMA);
        case '$': return make_token(TOKEN_GENERAL_REGISTER);
        case 'r': return special_register();
        case '#': return constant();
        case '+': return make_token(TOKEN_PLUS);
        case '%': return make_token(TOKEN_MOD); // this might also be a comment
        case '-': return make_token(TOKEN_MINUS);
        case '*': return make_token(TOKEN_STAR);
        case '&': return make_token(TOKEN_AND);
        case '|': return make_token(TOKEN_OR);
        case '^': return make_token(TOKEN_XOR);
        case '~': return make_token(TOKEN_COMPLEMENT);
        case '(': return make_token(TOKEN_LPAREN);
        case ')': return make_token(TOKEN_RPAREN);
        case '/': 
            if(peek() == '/') // this could also mark the beginning of a comment, according to Knuth
            {
                advance();
                return make_token(TOKEN_DSLASH);
            }
            else if(peek() == '*')
            {
                advance();
                while(peek() != '\0' && peek() != '*' && peek() != '/')
                {
                    advance();
                }
                if(peek() != '\0')
                    advance();

                return scan_token();
            }
            else
                return make_token(TOKEN_SLASH);
        case ';':
            if(peek() == ';')
            {
                return skip_to_end_of_line();
            }
            else
                return make_token(TOKEN_SEMICOLON);
        case '<':
            if(peek() == '<')
            {
                advance();
                return make_token(TOKEN_LSHIFT);
            }
            else
                return error_token("'<' operator is not defined, did you mean '<<'?");
            case '>':
            if(peek() == '>')
            {
                advance();
                return make_token(TOKEN_RSHIFT);
            }
            else
                return error_token("'>' operator is not defined, did you mean '>>'?");
        case '@': return make_token(TOKEN_AROUND);
        case '"': 
            return string();
        case 'g':
            return make_token(TOKEN_GREG);
        case 'l':
            return make_token(TOKEN_LOC);
        case 'b':
            return make_token(TOKEN_BYTE);
        case 'w':
            return make_token(TOKEN_WYDE);
        case 't':
            return make_token(TOKEN_TETRA);
        case 'o':
            return make_token(TOKEN_OCTA);
        case 'i':
            return make_token(TOKEN_IS);
    }

    return error_token("Unexpected character.");
}

void free_scanner()
{
    free_trie(scanner.instruction_trie);
}

void print_token(Token* token)
{
    switch(token->type)
    {
        case TOKEN_LOCAL:
            printf("TOKEN_LOCAL: ");
            break;
        case TOKEN_INSTRUCTION:
            printf("TOKEN_INSTRUCTION: ");
            break;
        case TOKEN_GENERAL_REGISTER:
            printf("TOKEN_GENERAL_REGISTER: ");
            break;
        case TOKEN_SPECIAL_REGISTER:
            printf("TOKEN_SPECIAL_REGISTER: ");
            break;
        case TOKEN_IMMEDIATE:
            printf("TOKEN_IMMEDIATE: ");
            break;
        case TOKEN_LABEL:
            printf("TOKEN_LABEL: ");
            break;
        case TOKEN_STRING:
            printf("TOKEN_STRING: ");
            break;
        case TOKEN_COMMA:
            printf("TOKEN_COMMA: ");
            break;
        case TOKEN_CONSTANT:
            printf("TOKEN_CONSTANT: ");
            break;
        case TOKEN_ERR:
            printf("TOKEN_ERR: ");
            break;
        case TOKEN_PLUS:
            printf("TOKEN_PLUS: ");
            break;
        case TOKEN_MINUS:
            printf("TOKEN_MINUS: ");
            break;
        case TOKEN_SLASH:
            printf("TOKEN_SLASH: ");
            break;
        case TOKEN_STAR:
            printf("TOKEN_STAR: ");
            break;
        case TOKEN_AROUND:
            printf("TOKEN_AROUND: ");
            break;
        case TOKEN_DSLASH:
            printf("TOKEN_DSLASH: ");
            break;
        case TOKEN_LSHIFT:
            printf("TOKEN_LSHIFT: ");
            break;
        case TOKEN_RSHIFT:
            printf("TOKEN_RSHIFT: ");
            break;
        case TOKEN_AND:
            printf("TOKEN_AND: ");
            break;
        case TOKEN_OR:
            printf("TOKEN_OR: ");
            break;
        case TOKEN_XOR:
            printf("TOKEN_XOR: ");
            break;
        case TOKEN_MOD:
            printf("TOKEN_MOD: ");
            break;
        case TOKEN_SEMICOLON:
            printf("TOKEN_SEMICOLON: ");
            break;
        case TOKEN_REGISTER:
            printf("TOKEN_REGISTER: ");
            break;
        case TOKEN_COMPLEMENT:
            printf("TOKEN_COMPLEMENT: ");
            break;
        case TOKEN_LPAREN:
            printf("TOKEN_LPAREN: ");
            break;
        case TOKEN_RPAREN:
            printf("TOKEN_RPAREN: ");
            break;
        case TOKEN_SCONST:
            printf("TOKEN_SCONST: ");
            break;
        case TOKEN_IS:
            printf("TOKEN_IS: ");
            break;
        case TOKEN_GREG:
            printf("TOKEN_GREG: ");
            break;
        case TOKEN_LOC:
            printf("TOKEN_LOC: ");
            break;
        case TOKEN_BYTE:
            printf("TOKEN_BYTE: ");
            break;
        case TOKEN_WYDE:
            printf("TOKEN_WYDE: ");
            break;
        case TOKEN_TETRA:
            printf("TOKEN_TETRA: ");
            break;
        case TOKEN_OCTA:
            printf("TOKEN_OCTA: ");
            break;
        case TOKEN_PREFIX:
            printf("TOKEN_PREFIX: ");
            break;
        case TOKEN_ENDLINE:
            printf("TOKEN_ENDLINE: ");
            break;
        case TOKEN_EOF:
            printf("TOKEN_EOF");
            break;
        default:
            printf("Unknown token type!Lexeme:");
            break;
        }
        printf("%.*s\n", token->length, token->start);

    fflush(stdout);
}
