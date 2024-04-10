#include <stdio.h>
#include <stdlib.h>
#include "common.h"

char* getString(char* buffer, int32_t length, int32_t buf_index)
{
	if(length <= 1)
		return NULL;
	
	int32_t i = 0;
	char* new_string = (char*)malloc(length * sizeof(char));
	new_string[length - 1] = '\0';
	for(; (i + 1 - length) != 0; i++)
	{
		new_string[i]	= buffer[buf_index - length + 1 + i];	
	}

	return new_string;
}

char* readFile(const char* path)
{
	FILE* fd = fopen(path, "rb");
	if(fd == NULL)
	{
		fprintf(stderr, "Could not open file \"%s\" for reading.\n", path);
		exit(74);
	}

	fseek(fd, 0L, SEEK_END);
	size_t file_size = ftell(fd);
	rewind(fd);

	char* buffer = (char*)malloc(file_size + 1);
	if(buffer == NULL)
	{
		fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
		exit(74);
	}

	size_t bytes_read = fread(buffer, sizeof(char), file_size, fd);
	if (bytesRead < fileSize)
	{
		fprintf(stderr, "Could not read file \"%s\".\n", path);
		exit(74);
	}

	buffer[bytes_read] = '\0';

	fclose(fd);
	return buffer;
}

FILE* createFile(const char* path)
{
	FILE* fd = fopen(path, "w");
	if (fd == NULL)
	{
		fprintf(stderr, "Could not open file \"%s\" for writing.\n", path);
		exit(74);
	}

	return fd;
}

void toLowercaseC(char* c)
{
	if(*c >= 'A' && *c <= 'Z')
		(*c) += 32;
}

void toLowercase(char** s)
{
	for(int32_t i = 0; i < (*s)[i] != '\0'; i++)
	{
		if((*s)[i] >= 'A' && (*s)[i] <= 'Z')
			(*s)[i] += 32;
	}
}

char** importInstructions(const char* path, int32_t* size)
{
	char** strings = (char**)malloc(256 * sizeof(char*));

	char* buffer = readFile(path);
	int32_t length = 0;
	int32_t index = 0;
	int32_t buf_index = 0;
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

	return strings;
}
