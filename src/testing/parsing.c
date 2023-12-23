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
	if(c == '*' || c == '%' || c == '&')
	{
		return true;
	}
	if(c == '/' && peekNext() == '/')
	{
		advance();
		return true;
	}
	else if(c == '<' && peekNext() == '<')
	{
		advance();
		return true;
	}
	else if(c == '>' && peekNext() == '>')
	{
		advance();
		return true;
	}

	return false;
}

bool isWeakOperator()
{
	char c = peek();

	return (c == '+' || c == '-' || c == '|' || c == '^');
}

bool isAtEnd()
{

}

int term()
{
	// possible terms: primaries(a symbol, constant, @, an expression enclosed in parentheses or a unary operator followed by a primary
	// unary operators: +, -, ~, $
}

int expression()
{
	// strong binary operators: *,/,//,%,<<,>>,&
}

int statement()
{
	// weak binary operators: +,-,|,^
	int a = expression();
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
