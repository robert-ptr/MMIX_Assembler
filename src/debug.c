#include "debug.h"

#ifdef ASM_DEBUG_SCANNER
static void dumpTokens()
{

}

void debugScanner()
{
    dumpTokens();
}
#endif

#ifdef ASM_DEBUG_PARSER
static void dumpBinary()
{
}

void debugParser()
{
    dumpBinary();
}
#endif

#ifdef ASM_DEBUG_TABLE
void debugTable()
{
}
#endif

#ifdef ASM_DEBUG_TRIE
void debugTrie()
{
}
#endif

/* these are all for debugging code in the VM */
static void dumpGeneralRegister(Debugger* debugger, uint8_t index)
{
	printf("G. Reg. number: %d value: %s", index, "placeholder"); // support for showing register value in binary,hexa and decimal
}

static void dumpSpecialRegister(Debugger* debugger, uint8_t index)
{
}

static void dumpGeneralRegisters(Debugger* debugger)
{
	for(int i = 0; i < 256; i++)
	{
		dumpGeneralRegister(debugger, i);
	}
}

static void dumpSpecialRegisters(Debugger* debugger)
{
	for(int i = 0; i < 32; i++)
	{
		dumpSpecialRegister(debugger, i);
	}
}

static void dumpMemory(Debugger* debugger)
{
}

static void dumpRegisterStack(Debugger* debugger)
{
}

static void dumpStackTop(Debugger* debugger)
{
}

static void dumpStack(Debugger* debugger)
{
}

void debug(Debugger* debugger)
{
	// TO DO: think of features to add
	// step by step debugging
	// thinking of doing an interactive one and one that just dumps the state
	if(debugger->flags & FLAG_GREGS)
	{
		dumpGeneralRegisters(debugger);
	}

	if(debugger->flags & FLAG_SPREGS)
	{
		dumpSpecialRegisters(debugger);
	}

	if(debugger->flags & FLAG_REGSTACK)
	{
		dumpRegisterStack(debugger);
	}

	if(debugger->flags & FLAG_INSTRSTACK)
	{
		dumpStack(debugger);
	}

	if(debugger->flags & FLAG_STACKTOP)
	{
		dumpStackTop(debugger);
	}

	if(debugger->flags & FLAG_MEM)
	{
		dumpMemory(debugger);
	}

	if(debugger->flags & FLAG_REG)
	{
		if(debugger->flags & FLAG_SPREG)
		{
			dumpSpecialRegister(debugger, debugger->flags & 0xFF00 >> 8);
		}
		else
		{
			dumpGeneralRegister(debugger, debugger->flags & 0xFF00 >> 8);
		}
	}
}


