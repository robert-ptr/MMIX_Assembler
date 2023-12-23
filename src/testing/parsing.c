#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct
{
	char* start;
	char* current;
} scanner;

char* readFile(const char* path)
{
	FILE* fd = fopen(path, "rb");
	fseek(fd, 0L, SEEK_END);
	size_t file_size = ftell(fd);
	rewind(fd);

	char* buffer = (char*)malloc(file_size + 1);
	size_t bytes_read = fread(buffer, sizeof(char), file_size, fd);
	buffer[bytes_read] = '\0';

	fclose(fd);
	return buffer;
}

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
	return (c == '*' || c == '%' || c == '&' || c == '/' || (c == '<' && next == '<') || (c == '>' && next == '>');
}

bool isWeakOperator()
{
	char c = peek();

	return (c == '+' || c == '-' || c == '|' || c == '^');
}

bool isAtEnd()
{
	if(peek() == '\0')
		return true;
}

bool isUnary()
{
	char c = peek();
	return c == '+' || c == '-' || c == '~' || c == '$';
		
}

bool isConstant()
{
	char c = peek();
	return c >= '0' && c <= '9';
}

bool isHexadecimal()
{
	char c == toLowercase(peek());
	return isConstant() || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f';
}

int symbol()
{}

int constant()
{
	int n = 0;
	while(isConstant())
	{
		n = n * 10 + (peek() - '0');
		advance();
	}
}

int reg()
{
	// retrieve value in register
	// I believe all registers have value 0 at the beginning of the program
	// I have to check that
}

int location()
{}

int fromHexadecimal()
{
	int n = 0;
	char c = toLowercase(peek());
	while(isHexadecimal())
	{
		if(c >= 'a')
		{
			n = n * 16 + (c - 'a' + 10)
		}
		else
		{
			n = n * 16 + (c - '0'); 
		}
		advance();
		c = toLowercase(peek());
	}
}

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
		advance();
		a = statement();
	}
	else if(peek() == '@')
	{
		a = location();
	}
	else if(peek() == '#')
	{
		a = fromHexadecimal();
	}
	else if(isAlphanumeric())
	{
		a = symbol();
	}
	else if(isConstant())
	{
		a = constant();
	}
	else
	{
		// Report an error
		printf("Unknown term.");
		return -1;
	}

	return a;
}

int expression(double &opt)
{
	// strong binary operators: *,/,//,%,<<,>>,&
	bool fpo = false;
	int a = term();
	int b;
	double opt_b;
	while(!isAtEnd() && !isWeakOperator())
	{
		if(isStrongOperator())
		{
			char op = advance();
			b = term(opt_b); // might be an int, might be a float
			switch(op)
			{
				case '*':
					if(!fpo)
						a *= b;
					else
						opt *= opt_b;
					break;
				case '&':
					if(!fpo)
						a &= b;
					else
						opt &= opt_b;
					break;
				case '%':
					if(!fpo)
						a %= b;
					else
						opt %= opt_b;
					break;
				case '/':
				{
					if(peekNext() == '/')
					{
						advance();
						fpo = true;
						opt = (double)a / (double)b;	
					}
					else
					{
						if(fpo)
							a /= b;
						else
							opt /= opt_b;
					}
					break;
				}
				case '<':
				{
					advance();
					if(!fpo)
						a <<= b;
					else
						opt <<= opt_b;
					break;
				}
				case '>':
				{
					advance();
					if(!fpo)
						a >>= b;
					else
						opt <<= opt_b;
					break;
				}
			}
		}
		else
		{
			printf("Error in expression.");
			return -1;
		}
	}

	return a;
}

int statement()
{
	// weak binary operators: +,-,|,^
	int a = expression();
	int b;
	while(!isAtEnd())
	{
		if(isWeakOperator())
		{
			char op = advance();
			b = expression();
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
			printf("Error in statement");
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
	}
	else
	{
		printf("[Usage] path");
	}
}
