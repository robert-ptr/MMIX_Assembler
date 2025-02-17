#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"

Scanner scanner;
uint32_t scanned_chars = 0;

static bool isAtEnd()
{
	return *scanner.current == '\0';
}

static TokenType checkKeyword()
{
    for(int i = 0; i < 7; i++) // check if it's a keyword used by the preprocessor
    {
        switch(i)
        {
            case 0:
                if (scanner.current - scanner.start == 2 && memcmp(scanner.start, "IS", 2) == 0)
                    return TOKEN_IS;
            case 1:
                if (scanner.current - scanner.start == 4 && memcmp(scanner.start, "GREG", 4) == 0)
                    return TOKEN_GREG;
            case 2:
                if (scanner.current - scanner.start == 4 && memcmp(scanner.start, "BYTE", 4) == 0)
                    return TOKEN_BYTE;
            case 3:
                if (scanner.current - scanner.start == 4 && memcmp(scanner.start, "WYDE", 4) == 0)
                    return TOKEN_WYDE;
            case 4:
                if (scanner.current - scanner.start == 5 && memcmp(scanner.start, "TETRA", 5) == 0)
                    return TOKEN_TETRA;
            case 5:
                if (scanner.current - scanner.start == 4 && memcmp(scanner.start, "OCTA", 4) == 0)
                    return TOKEN_OCTA;
            case 6:
                if (scanner.current - scanner.start == 6 && memcmp(scanner.start, "PREFIX", 6) == 0)
                    return TOKEN_PREFIX;
        }
    }

    return TOKEN_LABEL;
}

static Token makeToken(TokenType type)
{
	Token token;
	token.start = scanner.start;
	token.line = scanner.line;
	token.length = (int32_t)(scanner.current - scanner.start);
	token.offset = scanned_chars + 1;
    token.type = type;

    scanned_chars += token.length;

	return token;
}

static Token errorToken(char* message)
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

static void skipWhitespace()
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
				while(!isAtEnd() && peek() != '\n')
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

static bool isNumeric()
{
	char c = peek();
	return c >= '0' && c <= '9';
}

static bool isHexadecimal()
{
	char c = tolower(peek());
	return (c >= '0'  && c <= '9') || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e';
}

static bool isAlphanumeric()
{
		char c = peek();
		return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == ':';
}

static TokenType identifierType()
{
	int32_t length = scanner.current - scanner.start;
	char* word = (char*)malloc((length + 1) * sizeof(char));
	word[length] = '\0';
    for(int32_t i = 0; i < length; i++)
	{
		word[i] = scanner.start[i];
	}
	stringToLowercase(word);

	if(findWord(scanner.instruction_trie, word))
    {
        return TOKEN_INSTRUCTION;
    }
    
    return checkKeyword();
}

void initScanner(const char* source)
{
    scanner.source = source;
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;

	scanner.instruction_trie = getNode();
    for(int i = 0; i < 256; i++)
    {
        insertNode(scanner.instruction_trie, instructions[i].name, true);
    }
}

static Token immediate()
{
	while(isNumeric())
	{
		advance();
	}

	return makeToken(TOKEN_IMMEDIATE);
}

static Token identifier()
{
	while(isAlphanumeric())
	{
		advance();
	}

	return makeToken(identifierType());
}

static Token generalRegister()
{
	advance();
	while(isAlphanumeric())
	{
		advance();
	}

	return makeToken(TOKEN_GENERAL_REGISTER);
}

static Token specialRegister()
{
	advance();
	while(isAlphanumeric())
	{
		advance();
	}

	return makeToken(TOKEN_SPECIAL_REGISTER);
}

static Token constant()
{
	advance();
	while(isHexadecimal())
	{
		advance();
	}

	return makeToken(TOKEN_CONSTANT);
}

static Token skipToEndOfLine()
{
	while(peek() != '\0' && peek() != '\n')
		advance();
    if (peek() == '\n')
        advance();
	return scanToken();
}

Token scanToken()
{
	skipWhitespace();
	scanner.start = scanner.current;

	if(isNumeric()) // add support for special constructs dH, dF, dB
	{
		return immediate();
	}
	else if(isAlphanumeric())
	{
		return identifier();
	}

	if (isAtEnd())
		return makeToken(TOKEN_EOF);

	char c = advance();

	switch(c)
	{
        case '\n':
				scanner.line++;
                return makeToken(TOKEN_ENDLINE);
		case ',': return makeToken(TOKEN_COMMA);
		case '$': return makeToken(TOKEN_GENERAL_REGISTER);
		case 'r': return specialRegister();
		case '#': return constant();
		case '+': return makeToken(TOKEN_PLUS);
		case '%': return makeToken(TOKEN_MOD); // this might also be a comment
		case '-': return makeToken(TOKEN_MINUS);
		case '*': return makeToken(TOKEN_STAR);	
		case '&': return makeToken(TOKEN_AND);
		case '|': return makeToken(TOKEN_OR);
		case '^': return makeToken(TOKEN_XOR);
		case '~': return makeToken(TOKEN_COMPLEMENT);
		case '(': return makeToken(TOKEN_LPAREN);
		case ')': return makeToken(TOKEN_RPAREN);
		case '/': 
			if(peek() == '/') // this could also mark the beginning of a comment, according to Knuth
			{				
				advance();
				return makeToken(TOKEN_DSLASH);
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

				return scanToken();
			}
			else
				return makeToken(TOKEN_SLASH);
		case ';':
			if(peek() == ';')
			{
				return skipToEndOfLine();
			}
			else
				return makeToken(TOKEN_SEMICOLON);
		case '<':
			if(peek() == '<')
			{
				advance();
				return makeToken(TOKEN_LSHIFT);
			}
			else
				return errorToken("'<' operator is not defined, did you mean '<<'?");
		case '>':
			if(peek() == '>')
			{
				advance();
				return makeToken(TOKEN_RSHIFT);
			}
			else
				return errorToken("'>' operator is not defined, did you mean '>>'?");
		case '@': return makeToken(TOKEN_AROUND);
		case '"':
			advance();
			while(peek() != '"' && !isAtEnd())
			{
				if(isAtEnd() || peek() == '\n')
					return errorToken("Unterminated string.");
				advance();
			}

			advance();

			Token token;
			token.start = scanner.start + 1;
			token.length = scanner.current - scanner.start - 2;
			token.type = TOKEN_STRING;
			token.line = scanner.line;

			return token;
        case 'g':
            return makeToken(TOKEN_GREG);
        case 'l':
            return makeToken(TOKEN_LOC);
        case 'b':
            return makeToken(TOKEN_BYTE);
        case 'w':
            return makeToken(TOKEN_WYDE);
        case 't':
            return makeToken(TOKEN_TETRA);
        case 'o':
            return makeToken(TOKEN_OCTA);
        case 'i':
            return makeToken(TOKEN_IS);
	}

	return errorToken("Unexpected character.");
}

void freeScanner()
{
    freeTrie(scanner.instruction_trie);
}

void printToken(Token* token)
{
    switch(token->type)
    {
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
