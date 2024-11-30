#include "debug.h"

#ifdef ASM_DEBUG_SCANNER
static void dumpTokens()
{
    Token token = scanToken();
    while(token.type != TOKEN_EOF)
    {
        switch(token.type)
        {
            case TOKEN_INSTRUCTION:
                printf("TOKEN_INSTRUCTION ");
                break;
            case TOKEN_GENERAL_REGISTER:
                printf("TOKEN_GENERAL_REGISTER ");
                break;
            case TOKEN_SPECIAL_REGISTER:
                printf("TOKEN_SPECIAL_REGISTER ");
                break;
            case TOKEN_IMMEDIATE:
                printf("TOKEN_IMMEDIATE ");
                break;
            case TOKEN_LABEL:
                printf("TOKEN_LABEL ");
                break;
            case TOKEN_STRING:
                printf("TOKEN_STRING ");
                break;
            case TOKEN_COMMA:
                printf("TOKEN_COMMA ");
                break;
            case TOKEN_CONSTANT:
                printf("TOKEN_CONSTANT ");
                break;
            case TOKEN_ERR:
                printf("TOKEN_ERR ");
                break;
            case TOKEN_PLUS:
                printf("TOKEN_PLUS ");
                break;
            case TOKEN_MINUS:
                printf("TOKEN_MINUS ");
                break;
            case TOKEN_SLASH:
                printf("TOKEN_SLASH ");
                break;
            case TOKEN_STAR:
                printf("TOKEN_STAR ");
                break;
            case TOKEN_AROUND:
                printf("TOKEN_AROUND ");
                break;
            case TOKEN_DSLASH:
                printf("TOKEN_DSLASH ");
                break;
            case TOKEN_LSHIFT:
                printf("TOKEN_LSHIFT ");
                break;
            case TOKEN_RSHIFT:
                printf("TOKEN_RSHIFT ");
                break;
            case TOKEN_AND:
                printf("TOKEN_AND ");
                break;
            case TOKEN_OR:
                printf("TOKEN_OR ");
                break;
            case TOKEN_XOR:
                printf("TOKEN_XOR ");
                break;
            case TOKEN_MOD:
                printf("TOKEN_MOD ");
                break;
            case TOKEN_SEMICOLON:
                printf("TOKEN_SEMICOLON ");
                break;
            case TOKEN_REGISTER:
                printf("TOKEN_REGISTER ");
                break;
            case TOKEN_COMPLEMENT:
                printf("TOKEN_COMPLEMENT ");
                break;
            case TOKEN_LPAREN:
                printf("TOKEN_LPAREN ");
                break;
            case TOKEN_RPAREN:
                printf("TOKEN_RPAREN ");
                break;
            case TOKEN_SCONST:
                printf("TOKEN_SCONST ");
                break;
            case TOKEN_IS:
                printf("TOKEN_IS ");
                break;
            case TOKEN_GREG:
                printf("TOKEN_GREG ");
                break;
            case TOKEN_LOC:
                printf("TOKEN_LOC ");
                break;
            case TOKEN_BYTE:
                printf("TOKEN_BYTE ");
                break;
            case TOKEN_WYDE:
                printf("TOKEN_WYDE ");
                break;
            case TOKEN_TETRA:
                printf("TOKEN_TETRA ");
                break;
            case TOKEN_OCTA:
                printf("TOKEN_OCTA ");
                break;
            case TOKEN_PREFIX:
                printf("TOKEN_PREFIX ");
                break;
            default:
                printf("Unknown token type!Lexeme:");
                for(int i = 0; i < token.length; i++)
                {
                    printf("%c", token.start[i]);
                }
                printf("\n");
        }
    }
    printf("\nTOKEN_EOF\n");
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


