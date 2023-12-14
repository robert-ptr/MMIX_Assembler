#include <stdio.h>
#include <string.h>
#include "scanner.h"

Scanner scanner;

void initScanner(const char* source)
{
	scanner.start = source;
	scanner.current = source;
	line = 1;
}
