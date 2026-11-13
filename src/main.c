#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"
#include "common.h"

int32_t main(int32_t argc, const char* argv[])
{
	if (argc < 2 || argc > 3)
	{
		fprintf(stderr, "Usage: mmix [input file path] [output file path(default is './output')]\n");
		exit(64);
	}

	char* source = read_file(argv[1]);
	const char* output_path = (argc == 3) ? argv[2] : "output";

	init_scanner(source);
	init_parser((char*)output_path);

	parse();

	free_parser();
	free_scanner();
	free(source);

	return 0;
}
