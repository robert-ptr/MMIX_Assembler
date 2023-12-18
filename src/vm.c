#include <stdlib.h>
#include "vm.h"
#include "common.h"

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

void growByteSet(ByteSet* byte_set)
{
	int capacity = GROW_LIST(byte_set->capacity);
	Byte* new_list = (Byte*)malloc(capacity * sizeof(Byte)); 

	for(int i = 0; i < byte_set->count; i++)
	{
		new_list[i] = byte_set->bytes[i];
	}

	byte_set->capacity = capacity;
	byte_set->bytes = new_list;
}

void addByte(ByteSet* byte_set, Byte byte)
{
	if(byte_set->count + 1 > byte_set->capacity)
	{
		growByteSet(byte_set);		
	}
	byte_set->bytes[byte_set->count++] = byte;
}

void freeByteSet(ByteSet* byte_set)
{
	free(byte_set->bytes);
	initByteSet(byte_set);
}

void initVM(VM* vm)
{
	resetStack(vm);
	initByteSet(vm->byte_set);
	initTable(vm->table);
}

void freeVM(VM* vm)
{
	freeByteSet(vm->byte_set);
	freeTable(vm->table);
}
