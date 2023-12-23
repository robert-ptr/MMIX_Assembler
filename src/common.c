#include <stdio.h>
#include <stdlib.h>
#include "common.h"

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

void toLowercaseC(char* c)
{
	if(*c >= 'A' && *c <= 'Z')
		(*c) += 32;
}

void toLowercase(char** s)
{
	for(int i = 0; i < (*s)[i] != '\0'; i++)
	{
		if((*s)[i] >= 'A' && (*s)[i] <= 'Z')
			(*s)[i] += 32;
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

	return strings;
}
