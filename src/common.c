#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

static const char to_lowercase[] = {
    ['A'] = 'a', ['B'] = 'b', ['C'] = 'c', ['D'] = 'd', ['E'] = 'e', ['F'] = 'f', 
    ['G'] = 'g', ['H'] = 'h', ['I'] = 'i', ['J'] = 'j', ['K'] = 'k', ['L'] = 'l', 
    ['M'] = 'm', ['N'] = 'n', ['O'] = 'o', ['P'] = 'p', ['Q'] = 'q', ['R'] = 'r',
    ['S'] = 's', ['T'] = 't', ['U'] = 'u', ['V'] = 'v', ['W'] = 'w', ['X'] = 'x',
    ['Y'] = 'y', ['Z'] = 'z',
    ['a'] = 'a', ['b'] = 'b', ['c'] = 'c', ['d'] = 'd', ['e'] = 'e', ['f'] = 'f', 
    ['g'] = 'g', ['h'] = 'h', ['i'] = 'i', ['j'] = 'j', ['k'] = 'k', ['l'] = 'l', 
    ['m'] = 'm', ['n'] = 'n', ['o'] = 'o', ['p'] = 'p', ['q'] = 'q', ['r'] = 'r',
    ['s'] = 's', ['t'] = 't', ['u'] = 'u', ['v'] = 'v', ['w'] = 'w', ['x'] = 'x',
    ['y'] = 'y', ['z'] = 'z',
    ['0'] = '0', ['1'] = '1', ['2'] = '2', ['3'] = '3', ['4'] = '4', ['5'] = '5', ['6'] = '6', ['7'] = '7', ['8'] = '8', ['9'] = '9'
};

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
    *c = to_lowercase[*c];
}

void stringToLowercase(char** s)
{
	for(int32_t i = 0; i < (*s)[i] != '\0'; i++)
	{
		charToLowercase(&(*s)[i]);
	}
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
		if(n & 1)
			str[i] = '1';
		else
			str[i] = '0';

		n >>= 1;
	}

    return str;
}

uint64_t fromHexadecimal(const char* str)
{
	uint64_t result = 0;
	int32_t n = strlen(str);
	char* new_str = (char*)malloc((n + 1) * sizeof(char));
	stringToLowercase(&new_str);	
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
