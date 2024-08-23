#ifndef mmix_proj_debugger
#define mmix_proj_debugger

#include "vm.h"

#define FLAG_ALL_GREGS 0x0001
#define FLAG_ALL_SPREGS 0x0002
#define FLAG_REGSTACK 0x0004
#define FLAG_INSTRSTACK 0x0008
#define FLAG_STACKTOP 0x0010
#define FLAG_MEM 0x0020
#define FLAG_REG 0x0040
#define FLAG_SPREG 0x0080

#define FORMAT_HEX 0x01
#define FORMAT_BIN 0x02
#define FORMAT_DEC 0x03

typedef struct
{
	VM* vm;
	uint16_t flags;
	uint8_t format;
} Debugger;

void debug(Debugger* debugger);

#endif
