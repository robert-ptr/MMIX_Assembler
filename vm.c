#include <stdlib.h>
#include "vm.h"

static void resetStack(VM* vm)
{
	vm->stackTop = vm->stack;
}

void initVM(VM* vm)
{
	resetStack(vm);
}

void freeVM(VM* vm)
{
}
