#ifndef mmix_proj_vm
#define mmix_proj_vm

#include <stddef.h>

#define STACK_MAX 512

typedef uint8_t Byte

typedef struct
{
	Byte* bytes;
	Byte* ip;
	int64_t stack[STACK_MAX];
	int64_t* stackTop;
} VM;

void initVM(VM* vm);
void freeVM(VM* vm);

#endif
