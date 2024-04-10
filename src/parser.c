#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include "scanner.h"
#include "vm.h"
#include "trie.h"

static bool isStrongOperator(Scanner* scanner)
{
	char c = peek(scanner);
	char next = peekNext(scanner);
	return (c == '*' || c == '%' || c == '&' || c == '/' || (c == '<' && next == '<') || (c == '>' && next == '>'));
}

static bool isWeakOperator(Scanner* scanner)
{
	char c = peek(scanner);

	return (c == '+' || c == '-' || c == '|' || c == '^');
}

static bool isAtEnd(Scanner* scanner)
{
	if(peek(scanner) == '\0' || peek(scanner) == '\n')
		return true;

	return false;
}

static bool isUnary(Scanner* scanner)
{
	char c = peek(scanner);
	return c == '+' || c == '-' || c == '~' || c == '$';
}

static bool isAlphanumeric(Scanner* scanner)
{
		char c = peek(scanner);
		return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == ':');
}

static bool isNumeric(Scanner* scanner)
{
	char c = peek(scanner);
	return c >= '0' && c <= '9';
}

static bool isRightParen(Scanner* scanner)
{
	return peek(scanner) == ')';
}

static bool isHexadecimal(Scanner* scanner)
{
	char c = peek(scanner);
	toLowercaseC(&c);
	return isNumeric(scanner) || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f';
}

static int32_t symbol(Scanner* scanner)
{
	// retrieve the symbol from the hashtable
	scanner->start = scanner->current;
	while(isAlphanumeric(scanner))
	{
		advance(scanner);
	}

	int32_t length = scanner->current - scanner->start;
	char* symbol = (char*)malloc((length + 1) * sizeof(char));
	symbol[length] = '\0';

	for(int32_t i = 0; i < length; i++)
	{
		symbol[i] = *(scanner->start + i);
	}

	toLowercase(&symbol);

	int32_t value;
	if(findInTable(&table, symbol, &value))
		return value;
	else
	{
		printf("Unknown symbol.\n");
		return -1;
	}
}

static int32_t constant(Scanner* scanner)
{
	int32_t n = 0;
	while(isNumeric(scanner))
	{
		n = n * 10 + (advance(scanner) - '0');
	}

	return n;
}

static int32_t reg()
{
	// here's the deal: register arithmetic only works in cases like:
	// $1 + 2 = $3 and $3 - $1 = 2
	// +,- are the only arithmetic operators allowed
}

static int32_t fromHexadecimal(Scanner* scanner)
{
	int32_t n = 0;
	char c = peek(scanner);
	toLowercaseC(&c);
	while(isHexadecimal(scanner))
	{
		if(c >= 'a')
		{
			n = n * 16 + (c - 'a' + 10);
		}
		else
		{
			n = n * 16 + (c - '0'); 
		}
		advance(scanner);
		c = peek(scanner);
		toLowercaseC(&c);
	}

	return n;
}

static int32_t expression(Scanner* scanner);

static int32_t term(Scanner* scanner)
{
	// possible terms: primaries(a symbol, constant, @, an strongOperators enclosed in parentheses or a unary operator followed by a primary
	// unary operators: +, -, ~, $
	int32_t a;
	if(isUnary(scanner))
	{
		char op = advance(scanner);
		a = term(scanner);
		switch(op)
		{
			case '+':
				break;
			case '-':
				a = -a;
				break;
			case '~':
				a = ~a;
				break;
			case '$':
				a = reg(scanner, a);
				break;
		}
	}
	else if(peek(scanner) == '(')
	{
		advance(scanner); // skip the '('
		a = expression(scanner);
		advance(scanner); // skip the ')'
	}
	else if(peek(scanner) == '@')
	{
		a = location(scanner);
	}
	else if(peek() == '#')
	{
		advance(scanner);
		a = fromHexadecimal(scanner);
	}
	else if(isAlphanumeric(scanner) && !isNumeric(scanner))
	{
		a = symbol(scanner);
	}
	else if(isNumeric(scanner))
	{
		a = constant(scanner);
	}
	else
	{
		// Report an error
		printf("Unknown term.\n");
		return -1;
	}
	return a;
}

static int32_t strongOperators(double* opt)
{
	// strong binary operators: *,/,//,%,<<,>>,&
	bool fpo = false;
	int32_t a = term(scanner);
	int32_t b;
	double opt_b;
	while(!isAtEnd(scanner) && !isRightParen(scanner) && !isWeakOperator(scanner))
	{
		printf("%c ", peek(scanner));
		if(isStrongOperator(scanner))
		{
			char op = advance(scanner);
			int32_t b;
			switch(op)
			{
				case '*':
					b = term(scanner);
					if(!fpo)
						a *= b;
					else
						*opt *=opt_b;
					break;
				case '&':
					b = term(scanner);
					if(!fpo)
						a &= b;
					else
						printf("Invalid operands.\n");
					break;
				case '%':
					b = term(scanner);
					if(!fpo)
						a %= b;
					else
						printf("Invalid operands.\n");
					break;
				case '/':
				{
					if(peekNext(scanner) == '/')
					{
						advance(scanner);
						b = term(scanner);
						fpo = true;
						*opt = (double)a / (double)b;	
					}
					else
					{
						b = term(scanner);
						if(!fpo)
							a /= b;
						else
							*opt /= opt_b;
					}
					break;
				}
				case '<':
				{
					advance(scanner);
					b = term(scanner);
					if(!fpo)
						a <<= b;
					else
						printf("Invalid operands.\n");
					break;
				}
				case '>':
				{
					advance(scanner);
					b = term(scanner);
					if(!fpo)
						a >>= b;
					else
						printf("Invalid operands.\n");
					break;
				}
			}
		}
		else
		{
			printf("Error in strongOperators.\n");
			return -1;
		}
	}

	return a;
}

static int32_t expression(Scanner* scanner)
{
	// weak binary operators: +,-,|,^
	double opt;
	int32_t a = strongOperators(scanner, &opt);
	int32_t b;
	while(!isAtEnd(scanner) && !isRightParen(scanner))
	{
		if(isWeakOperator(scanner))
		{
			char op = advance(scanner);
			double opt_b;
			b = strongOperators(scanner, &opt_b);
			switch(op)
			{
				case '+':
					a += b;
					break;
				case '-':
					a -= b;
					break;
				case '|':
					a |= b;
					break;
				case '^':
					a ^= b;
					break;
			}
		}
		else
		{
			printf("Error in expression\n");
			return -1;
		}
	}

	return a;
}

static void errorAt(Token token, const char* message)
{
	fprintf(stderr, "[line %d] Error", token->line);
	if (token->type == TOKEN_EOF) 
	{
		fprintf(stderr, " at end");
	} 
	else if (token->type == TOKEN_ERROR) 
	{
		
	} 
	else 
	{
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	}
	fprintf(stderr, ": %s\n", message);
	parser.hadError = true;
}

static void errorAtCurrent(Parser* parser, const char* message)
{
	errorAt(parser->current, message);	
}

static void error(Parser* parser, const char* message)
{
	errorAt(parser->previous, message);
}

static void advance(Parser* parser, Scanner* scanner)
{
	parser->previous = parser->current;

	for (;;)
	{
		parser->current = scanToken(scanner);
		if(parser->current.type != TOKEN_ERR)
			break;

		errorAtCurrent(parser, );
	}
}

static void consume(Parser* parser, TokenType type, const char* message)
{
	if (parser->current.type == type)
	{
		advance();
		return;
	}

	errorAtCurrent(message);
}

static bool check(Parser* parser, TokenType type)
{
	return parser->current.type == type;
}

static void emitByte(VM* vm, Byte byte)
{
	addByte(vm->byte_set, byte);
}

static bool isMacro(Parser* parser)
{
	Token token = parser->current;
	// IS, GREG, BYTE, WYDE, TETRA, OCTA
	if(token.length == 2 && memcmp(token.start, "is", 2))
	{
		return true;	
	}
	else if (token.length == 4)
	{
		if(memcmp(token.start, "greg", 4))
			return true;
		else if(memcmp(token.start, "byte", 4))
			return true;
		else if(memcmp(token.start, "wyde", 4))
			return true;
		else if(memcmp(token.start, "octa", 4))
			return true;
	}
	else if (token.length == 5 && memcmp(token.start, "tetra", 5))
		return true;

	return false;
}

static bool match(char* instruction, int32_t start, char* pattern, int32_t length)
{
	if(memcmp(instruction + start, pattern, length) == 0)
		return true;
	
	return false;
}

static void commaStatement(Parser* parser, Scanner* scanner, VM* vm)
{
	int32_t value = expressionStatement(parser, scanner, vm);
	emitByte(vm, ); // complete later 
	while(check(parser, TOKEN_COMMA))
	{
		advance(parser, scanner);
		value = expressionStatement(parser, scanner, vm);
		emitByte(vm, value);
	}
}

static void instructionStatement(Parser* parser, Scanner* scanner, VM* vm)
{
	advance(parser, scanner);
	TrieNode* root = getNode();
	createInstructionTrie(root);
	if(parser->current.type == TOKEN_INSTRUCTION)
	{
		char* word = (char*)malloc((parser->current.length + 1) * sizeof(char));
		for(int32_t i = 0; i < parser->current.length; i++)
		{
			word[i] = parser->current.start[i];
		}
		word[parser->current.length] = '\0';
		int32_t emitValue;
		if((emitValue = findWord(root, word)) != -1)
		{
			Byte byte = toByte(emitValue);
			emitByte(vm, byte);	// check the operands in order to determine if you need to add 1
		}
		else
		{
			errorAtCurrent(parser, "Unknown instruction.");
		}

		free(word);
	}
	else
	{
		// Report an error
		errorAtCurrent(parser, "Unknown instruction.");
	}
	commaStatement();
}

static void labelStatement(Parser* parser, Scanner* scanner, VM* vm)
{
	char* word = (char*)malloc((parser->previous.length + 1) * sizeof(char));
	word = parser->previous.start;
	word[length] = '\0';

	if(isMacro(parser))
	{
		advance(parser, scanner);
		
		// Somethings needs to be done here
		addToTable(vm->table, word, parser->current.start); 
	}
	instructionStatement(parser, scanner, vm);
}

void parse(Parser* parser, Scanner* scanner, VM* vm)
{
	advance(parser, scanner);

	if(parser.previous == TOKEN_LABEL)
	{
		labelStatement(parser, scanner, vm);
	}
	else
	{
		instructionStatement(parser, scanner, vm);
	}
}
