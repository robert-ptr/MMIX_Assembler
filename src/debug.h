#ifndef MMIX_PROJ_DEBUGGER
#define MMIX_PROJ_DEBUGGER

#include "vm.h"
#include "scanner.h"
#include "parser.h"

#define FLAG_GREGS 0x0001
#define FLAG_SPREGS 0x0002
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

void debug(Debugger* debugger); // for the VM basically

#define ASM_DEBUG_SCANNER
#define ASM_DEBUG_PARSER
#define ASM_DEBUG_TABLE
#define ASM_DEBUG_TRIE

#ifdef ASM_DEBUG_SCANNER
void debugScanner();
#endif

#ifdef ASM_DEBUG_PARSER
void debugParser();
#endif

#ifdef ASM_DEBUG_TABLE
void debugTable();
#endif

#ifdef ASM_DEBUG_TRIE
void debugTrie();
#endif

#endif
