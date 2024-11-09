#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"

Scanner scanner;

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
	token.type = type;

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

static char peekNext()
{
	if(*scanner.current == '\0') return '\0';

	return scanner.current[1];
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
				break;
			case '\n':
				scanner.line++;
				advance();
				break;
			case '%': 
			{
				while(!isAtEnd() && peek() != '\n')
				{
					advance();
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
	char c = peek();
	charToLowercase(&c);
	return (c >= '0'  && c <= '9') || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e';
}

static bool isAlphanumeric()
{
		char c = peek(scanner);
		return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == ':';
}

static TokenType identifierType()
{
	int32_t length = scanner.current - scanner.start;
	char* word = (char*)malloc(length * sizeof(char));
	for(int32_t i = 0; i < length; i++)
	{
		word[i] = *(scanner.start + i);
	}
	stringToLowercase(&word);
	if(findWord(scanner.instruction_trie, word))
		return TOKEN_INSTRUCTION;

    return checkKeyword();
}

void initScanner(char* source)
{
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;

	int32_t size;
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
	advance();
	while(peek() != '\0' && peek() != '\n')
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
		case ',': return makeToken(TOKEN_COMMA);
		case '$': return makeToken(TOKEN_REGISTER);
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
		{
			if(peekNext() == '/') // this could also mark the beginning of a comment, according to Knuth
			{				
				advance();
				return makeToken(TOKEN_DSLASH);
			}
			else if(peekNext() == '*')
			{
				advance();
				while(peekNext() != '\0' && peek() != '*' && peekNext() != '/')
				{
					advance();
				}
				if(peek() != '\0')
					advance();

				return scanToken();
			}
			else
				return makeToken(TOKEN_SLASH);
		}
		case ';':
		{
			if(peekNext() == ';')
			{
				return skipToEndOfLine();
			}
			else
				return makeToken(TOKEN_SEMICOLON);
		}
		case '<':
		{
			if(peekNext() == '<')
			{
				advance();
				return makeToken(TOKEN_LSHIFT);
			}
			else
				return errorToken("'<' operator is not defined, did you mean '<<'?");
		}
		case '>':
		{
			if(peekNext() == '>')
			{
				advance();
				return makeToken(TOKEN_RSHIFT);
			}
			else
				return errorToken("'>' operator is not defined, did you mean '>>'?");
		}
		case '@': return makeToken(TOKEN_AROUND);
		case '"':
		{
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
		}
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
