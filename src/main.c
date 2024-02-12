#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "parser.h"
#include "vm.h"
#include "common.h"
#include "debug.h"

typedef enum
{
	OK,
	COMPILE_ERROR,
	RUNTIME_ERROR,
} ErrorType;

ErrorType runFile(const char* source)
{
	//parse();
	//execute();
}

int main(int argc, const char* argv[])
{
//	initVM();
	const char* flags;
	const char* source;
	FILE* output_fd;
	if (argc == 2)
	{
		source = readFile(argv[1]);
		Scanner* scanner = initScanner(source);
		freeScanner(scanner);
	}
	else if (argc == 3)
	{
		if(argv[1][0] == '-')
		{
			flags = (char*)malloc(sizeof(argv[1]));
			strcpy(flags, ++argv[1]);
			source = readFile(argv[2]);
		}
		else
		{
			source = readFile(argv[1]);
			output_fd = createFile(argv[2]);
		}
	}	
	else if (argc == 4)
	{
		strcpy(flags, ++argv[1]);
		source = readFile(argv[2]);
		output_fd = createFile(argv[3]);
	}
	else
	{
		fprintf(stderr, "Usage: mmix [options] [input file path] [output file path(optional)\n");
		exit(64);
	}

//	freeVM();
	return 0;
}
