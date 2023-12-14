#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"
#include "vm.h"

typedef enum
{
	OK,
	COMPILE_ERROR,
	RUNTIME_ERROR,
} ErrorType;

char* readFile(const char* path)
{
	FILE* fd = fopen(path, "rb");
	fseek(fd, 0L, SEEK_END);
	size_t file_size = ftell(fd);
	rewind(fd);

	char* buffer = (char*)malloc(file_size + 1);
	size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
	buffer[bytes_read] = '\0';

	fclose(file);
	return buffer;
}

char** importInstructions(const char* path)
{
	FILE* fd = fopen(path, "rb");
	size_t buf_size = 256;
	char* buffer = (char*)malloc(buf_size * sizeof(char));

	while(getline(&buffer, &buf_size, fd) > 0)
	{

	}
}

ErrorType runFile(const char* source)
{
	parse();
	execute();
}

int main(int argc, const char* argv[])
{
	initVM();
	initScanner();
	if (argc == 2)
	{
		const char* source = readFile(argv[1]);
		runFile(source);
	}
	else
	{
		fprintf(stderr, "Usage: mmix [path]\n");
		exit(64);
	}

	freeVM();
	freeScanner();
	return 0;
}
