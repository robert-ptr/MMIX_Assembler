#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../table.h"
#include "../common.h"

struct
{
	char* start;
	char* current;
} scanner;

Table table;

char advance()
{
	scanner.current++;
	return scanner.current[-1];
}

char peek()
{
	return *scanner.current;
}

char peekNext()
{
	if(peek() == '\0') return '\0';
	return scanner.current[1];
}

bool isStrongOperator()
{
	char c = peek();
	char next = peekNext();
	return (c == '*' || c == '%' || c == '&' || c == '/' || (c == '<' && next == '<') || (c == '>' && next == '>'));
}

bool isWeakOperator()
{
	char c = peek();

	return (c == '+' || c == '-' || c == '|' || c == '^');
}

bool isAtEnd()
{
	if(peek() == '\0' || peek() == '\n')
		return true;

	return false;
}

bool isUnary()
{
	char c = peek();
	return c == '+' || c == '-' || c == '~' || c == '$';
		
}
bool isAlphanumeric()
{
		char c = peek();
		return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == ':');
}

bool isNumeric()
{
	char c = peek();
	return c >= '0' && c <= '9';
}

bool isHexadecimal()
{
	char c = peek();
	toLowercaseC(&c);
	return isNumeric() || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f';
}

int symbol()
{
	// retrieve the symbol from the hashtable
	scanner.start = scanner.current;
	while(isAlphanumeric())
	{
		advance();
	}

	int length = scanner.current - scanner.start;
	char* symbol = (char*)malloc((length + 1) * sizeof(char));
	symbol[length] = '\0';

	for(int i = 0; i < length; i++)
	{
		symbol[i] = *(scanner.start + i);
	}

	toLowercase(&symbol);

	int value;
	if(findInTable(&table, symbol, &value))
		return value;
	else
	{
		printf("Unknown symbol.\n");
		return -1;
	}
}

int constant()
{
	int n = 0;
	while(isNumeric())
	{
		n = n * 10 + (advance() - '0');
	}

	return n;
}

int reg()
{
	// here's the deal: register arithmetic only works in cases like:
	// $1 + 2 = $3 and $3 - $1 = 2
	// +,- are the only arithmetic operators allowed
}

int location()
{}

int fromHexadecimal()
{
	int n = 0;
	char c = peek();
	toLowercaseC(&c);
	while(isHexadecimal())
	{
		if(c >= 'a')
		{
			n = n * 16 + (c - 'a' + 10);
		}
		else
		{
			n = n * 16 + (c - '0'); 
		}
		advance();
		c = peek();
		toLowercaseC(&c);
	}

	return n;
}

int statement();

int term()
{
	// possible terms: primaries(a symbol, constant, @, an expression enclosed in parentheses or a unary operator followed by a primary
	// unary operators: +, -, ~, $
	int a;
	if(isUnary())
	{
		char op = advance();
		a = term();
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
				a = reg(a);
				break;
		}
	}
	else if(peek() == '(')
	{
		advance(); // skip the '('
		a = statement();
		advance(); // skip the ')'
	}
	else if(peek() == '@')
	{
		a = location();
	}
	else if(peek() == '#')
	{
		advance();
		a = fromHexadecimal();
	}
	else if(isAlphanumeric() && !isNumeric())
	{
		a = symbol();
	}
	else if(isNumeric())
	{
		a = constant();
	}
	else
	{
		// Report an error
		printf("Unknown term.\n");
		return -1;
	}
	return a;
}

int expression(double* opt)
{
	// strong binary operators: *,/,//,%,<<,>>,&
	bool fpo = false;
	int a = term();
	int b;
	double opt_b;
	while(!isAtEnd() && !isWeakOperator())
	{
		printf("%c ", peek());
		if(isStrongOperator())
		{
			char op = advance();
			int b;
			switch(op)
			{
				case '*':
					b = term();
					if(!fpo)
						a *= b;
					else
						*opt *=opt_b;
					break;
				case '&':
					b = term();
					if(!fpo)
						a &= b;
					else
						printf("Invalid operands.\n");
					break;
				case '%':
					b = term();
					if(!fpo)
						a %= b;
					else
						printf("Invalid operands.\n");
					break;
				case '/':
				{
					if(peekNext() == '/')
					{
						advance();
						b = term();
						fpo = true;
						*opt = (double)a / (double)b;	
					}
					else
					{
						b = term();
						if(fpo)
							a /= b;
						else
							*opt /= opt_b;
					}
					break;
				}
				case '<':
				{
					advance();
					b = term();
					if(!fpo)
						a <<= b;
					else
						printf("Invalid operands.\n");
					break;
				}
				case '>':
				{
					advance();
					b = term();
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
			printf("Error in expression.\n");
			return -1;
		}
	}

	return a;
}

int statement()
{
	// weak binary operators: +,-,|,^
	double opt;
	int a = expression(&opt);
	int b;
	while(!isAtEnd())
	{
		if(isWeakOperator())
		{
			char op = advance();
			double opt_b;
			b = expression(&opt_b);
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
			printf("Error in statement\n");
			return -1;
		}
	}

	return a;
}

void initScanner(const char* source)
{
	scanner.start = source;
	scanner.current = source;
}

int main(int argc, char* argv[])
{
	if(argc == 2)
	{
		char* buffer = readFile(argv[1]);
		initScanner(buffer);
		initTable(&table);

		printf("%d", statement());

		freeTable(&table);
	}
	else
	{
		printf("[Usage] path");
	}
}
