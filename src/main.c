#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "parser.h"
#include "vm.h"
#include "trie.h"

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
	size_t bytes_read = fread(buffer, sizeof(char), file_size, fd);
	buffer[bytes_read] = '\0';

	fclose(fd);
	return buffer;
}

ErrorType runFile(const char* source)
{
	//parse();
	//execute();
}

int main(int argc, const char* argv[])
{
//	initVM();
//	initScanner();
	if (argc == 2)
	{
		const char* source = readFile(argv[1]);
		//runFile(source);
		int size;
		char** words = importInstructions(argv[1], &size);
		for(int i = 0; i < size; i++)
		{
			toLowercase(&words[i]);
			printf("%s ", words[i]);
		}
		TrieNode* root = getNode();
		createTrie(root ,words, size);

		if(findWord(root, "2addu"))
				printf("\ntrue\n");
		else
			printf("\nfalse\n");
	}
	else
	{
		fprintf(stderr, "Usage: mmix [path]\n");
		exit(64);
	}

//	freeVM();
//	freeScanner();
	return 0;
}
