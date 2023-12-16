#include <stdlib.h>
#include "vm.h"

static void resetStack(VM* vm)
{
	vm->stack_top = vm->stack;
}

void initByteSet(ByteSet* byte_set)
{
	byte_set->count = 0;
	byte_set->capacity = 0;
	byte_set->bytes = NULL;
}

void freeByteSet(ByteSet* byte_set)
{
	free(byte_set->bytes);
	initByteSet(byte_set);
}

void initVM(VM* vm)
{
	resetStack(vm);
}

void freeVM(VM* vm)
{
}
