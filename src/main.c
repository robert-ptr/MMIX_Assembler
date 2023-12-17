#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "parser.h"
#include "vm.h"
#include "common.h"

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
	if (argc == 2)
	{
		const char* source = readFile(argv[1]);
		Scanner* scanner = initScanner(source);
		/*
		Token token = scanToken(scanner);
		while(token.type != TOKEN_EOF)
		{
			Token prev_token = token;
			switch(token.type)
			{
				case TOKEN_EOF:
					printf("TOKEN_EOF ");
					break;
				case TOKEN_INSTRUCTION:
					printf("TOKEN_INSTRUCTION ");
					break;
				case TOKEN_GENERAL_REGISTER:
					printf("TOKEN_GENERAL_REGISTER ");
					break;
				case TOKEN_SPECIAL_REGISTER:
					printf("TOKEN_SPECIAL_REGISTER ");
					break;
				case TOKEN_IMMEDIATE:
					printf("TOKEN_IMMEDIATE ");
					break;
				case TOKEN_STRING:
					printf("TOKEN_STRING ");
					break;
				case TOKEN_LABEL:
					printf("TOKEN_LABEL ");
					break;
				case TOKEN_COMMA:
					printf("TOKEN_COMMA ");
					break;
				case TOKEN_ERR:
					printf("TOKEN_ERR ");
					break;
				case TOKEN_LOCATION:
					printf("TOKEN_LOCATION ");
					break;
			}
			token = scanToken(scanner);
			if(token.line != prev_token.line)
				printf("\n");
		}
		*/
		freeScanner(scanner);
	}
	else
	{
		fprintf(stderr, "Usage: mmix [path]\n");
		exit(64);
	}

//	freeVM();
	return 0;
}
