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
	if (bytes_read < file_size)
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

void charToLowercase(char* c)
{
	if(*c >= 'A' && *c <= 'Z')
		(*c) += 32;
}

void stringToLowercase(char** s)
{
	for(int32_t i = 0; i < (*s)[i] != '\0'; i++)
	{
		charToLowercase(&(*s)[i]);
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

char* intToHexString(uint64_t n)
{
	uint64_t c = n;
	uint64_t size = 0;
	while(c > 0)
	{
		c /= 16;
		size++;
	}

	char* str = (char*)malloc(19 * sizeof(char));
	str[0] = '0';
	str[1] = 'x';
	for(int i = 2; i < 18; i++)
	{
		str[i] = '0';
	}
	
	for(int i = 17; i > 1; i--, n /= 16)
	{
		c = n % 16;
		switch(c)
		{
			case 0:
				str[i] = '0';
				break;
			case 1:
				str[i] = '1';
				break;
			case 2:
				str[i] = '2';
				break;
			case 3:
				str[i] = '3';
				break;
			case 4:
				str[i] = '4';
				break;
			case 5:
				str[i] = '5';
				break;
			case 6:
				str[i] = '6';
				break;
			case 7:
				str[i] = '7';
				break;
			case 8:
				str[i] = '8';
				break;
			case 9:
				str[i] = '9';
				break;
			case 10:
				str[i] = 'A';
				break;
			case 11:
				str[i] = 'B';
				break;
			case 12:
				str[i] = 'C';
				break;
			case 13:
				str[i] = 'D';
				break;
			case 14:
				str[i] = 'E';
				break;
			case 15:
				str[i] = 'F';
				break;
		}
	}

	return str;
}

char* intToBinaryString(uint64_t n)
{
	char* str = (char*)malloc(65 * sizeof(char));

	for(int i = 0; i < 64; i++)
	{
		str[i] = '0';
	}

	for(int i = 63; i >= 0 && n > 0; i--)
	{
		if(n & 1 == 1)
			str[i] = '1';
		else
			str[i] = '0';

		n >>= 1;
	}
}

char* intToBinaryString(uint64_t n)
{
	char* str = (char*)malloc(11 * sizeof(char));

	for(int i = 0; i < 10; i++)
	{
		str[i] = '0';
	}

	for(int i = 9; i >= 0 && n > 0; i--)
	{
		str[i] = '0' + n % 10;
		n /= 10;
	}
}

uint64_t fromHexadecimal(const char* str)
{
	uint64_t result = 0;
	int32_t n = strlen(str);
	char* new_str = (char*)malloc((n + 1) * sizeof(char));
	stringToLowercase(new_str);	
	for(int i = 0; i < n; i++)
	{
		switch(new_str[i])
		{
			case '0':
				result *= 16;
				break;
			case '1':
				result = result * 16 + 1;
				break;
			case '2':
				result = result * 16 + 2;
				break;
			case '3':
				result = result * 16 + 3;
				break;
			case '4':
				result = result * 16 + 4;
				break;
			case '5':
				result = result * 16 + 5;
				break;
			case '6':
				result = result * 16 + 6;
				break;
			case '7':
				result = result * 16 + 7;
				break;
			case '8':
				result = result * 16 + 8;
				break;
			case '9':
				result = result * 16 + 9;
				break;
			case 'a':
				result = result * 16 + 10;
				break;
			case 'b':
				result = result * 16 + 11;
				break;
			case 'c':
				result = result * 16 + 12;
				break;
			case 'd':
				result = result * 16 + 13;
				break;
			case 'e':
				result = result * 16 + 14;
				break;
			case 'f':
				result = result * 16 + 15;
				break;
			default:
				fprintf(stderr, "Invalid hex digit!");
				break;
		}
	}

	free(new_str);
}
