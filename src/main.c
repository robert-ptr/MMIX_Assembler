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

char* getString(char* buffer, int length, int buf_index)
{
	if(length <= 1)
		return NULL;
	
	int i = 0;
	char* new_string = (char*)malloc(length * sizeof(char));
	new_string[length - 1] = '\0';
	for(; (i + 1 - length) != 0; i++)
	{
		new_string[i]	= buffer[buf_index - length + 1 + i];	
	}

	return new_string;
}

void toLowercase(char** string)
{
	for(int i = 0; i < (*string)[i] != '\0'; i++)
	{
		if((*string)[i] >= 'A' && (*string)[i] <= 'Z')
			(*string)[i] += 32;
	}
}

char** importInstructions(const char* path, int* size)
{
	char** strings = (char**)malloc(256 * sizeof(char*));

	char* buffer = readFile(path);
	int length = 0;
	int index = 0;
	int buf_index = 0;
	while(buffer[buf_index] != '\0')
	{
		if(buffer[buf_index] == ' ' || buffer[buf_index] == ';' || buffer[buf_index] == '\t' || buffer[buf_index] == '\n')
		{
			if(index == 0)
				length++;
			char* new_string = getString(buffer, length, buf_index);
			if(new_string != NULL)
			{
				strings[index++] = new_string;
			}
			length = 0;
		}
		buf_index++;
		length++;
	}
	*size = index;
	index--;
	while(index >= 0)
	{
		printf("%s ", strings[index]);
		index--;
	}

	return strings;
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
