#include "debug.h"

#ifdef ASM_DEBUG_SCANNER

static void dumpTokens()
{
    Token token;
    while(token.type != TOKEN_EOF)
    {
        token = scanToken();

        printToken(&token); 
    }
}

void debugScanner()
{
    dumpTokens();
}
#endif

#ifdef ASM_DEBUG_PARSER
void debugParser()
{
    parse();
}
#endif

#ifdef ASM_DEBUG_TABLE
void debugTable()
{
    // the simplest way to debug the table is to create one and perform all the actions you would perform on a hashtable
    // and see if/where it breaks

    Table test_table;
    initTable(&test_table);
    bool bool_key = false;
    float float_key = 3.156f;
    double double_key = 2.718;
    uint64_t int_key = 74;

    if(addToTable_bool(&test_table, &bool_key, sizeof(bool), true))
    {
        printf("Added bool value:true to table, key: false\n");
    }
    
    if(addToTable_float(&test_table, &float_key, sizeof(float), -3.156f))
    {
        printf("Added float value:-3.156 to table, key:3.156\n");
    }

    if(addToTable_double(&test_table, &double_key, sizeof(double), -2.718))
    {
        printf("Added float value:-2.718 to table, key:2.718\n");
    }

    if(addToTable_uint64_t(&test_table, &int_key, sizeof(uint64_t), 5))
    {
        printf("Added int value:5 to table, key: 74\n");
    }

    if(addToTable_bool(&test_table, "bool", 4, true))
    {
        printf("Added bool value:true to table, key: 'bool'\n");
    }
    
    if(addToTable_uint64_t(&test_table, "uint64_t", 7, 351))
    {
        printf("Added uint64_t value:351 to table, key: 'uint64_t'\n");
    }

    if(addToTable_string(&test_table, "string", 6, "string1223443245"))
    {
        printf("Added string value:string_value to table, key:'string'\n");
    }

    if(addToTable_float(&test_table, "float", 5, 3.14f))
    {
        printf("Added float value:3.14 to table, key:'float'\n");
    }

    if(addToTable_double(&test_table, "double", 6, 3.14159265))
    {
        printf("Added double value:3.14159265 to table, key:'double'\n");
    }

    printf("\n\n");

    EntryValue value;

    if(findInTable(&test_table, &bool_key, sizeof(bool), &value))
    {
        printf("bool: %d\n", value.bool_value);
    }

    if(findInTable(&test_table, &float_key, sizeof(float), &value))
    {
        printf("float: %f\n", value.float_value);
    }

    if(findInTable(&test_table, &double_key, sizeof(double), &value))
    {
        printf("double: %lf\n", value.double_value);
    }   

    if(findInTable(&test_table, &int_key, sizeof(uint64_t), &value))
    {
        printf("%d\n", value.int_value);
    }

    if(findInTable(&test_table, "bool", 4, &value))
    {
        printf("'bool': %d\n", value.bool_value);
    }

    if(findInTable(&test_table, "uint64_t", 7, &value))
    {
        printf("'uint64_t': %u\n", value.int_value);
    }

    if(findInTable(&test_table, "string", 6, &value))
    {
        printf("'string': %s\n", value.str_value);
    }

    if(findInTable(&test_table, "float", 5, &value))
    {
        printf("'float': %f\n", value.float_value);
    }

    if(findInTable(&test_table, "double", 6, &value))
    {
        printf("'double': %lf\n", value.double_value);
    }

    freeTable(&test_table);
}
#endif

#ifdef ASM_DEBUG_TRIE
int number_of_words = 0;

static void traverseTrie(TrieNode* trie)
{
    bool leaf = true;
    for(int i = 0; i < ALPHABET_SIZE; i++)
    {
        if(trie->nodes[i] != NULL)
        {
            traverseTrie(trie->nodes[i]);
            leaf = false;
        }
    }

    if (leaf && !trie->isWord)
    {
        uint8_t buffer[128]; // this trie isn't intended to be used with extremely long words
                          // but you could use a linked list or a dynamic buffer if there were long words in the trie
        int i = 0;
        while(trie->parent != NULL)
        {
            buffer[i++] = trie->index;
            trie = trie->parent;
        }

        printf("Reached leaf but isn't word!\n");
        for(int j = 0; j < i; j++)
        {
            printf("%c", indexToChar(buffer[j]));
        }
        printf("\n");
    }

    if(trie->isWord)
    {
        printf("Found word number %d!\n", number_of_words + 1);
        number_of_words++;
        uint8_t buffer[128]; // this trie isn't intended to be used with extremely long words
                          // but you could use a linked list or a dynamic buffer if there were long words in the trie
        int i = 0;
        while(trie->parent != NULL)
        {
            buffer[i++] = trie->index;
            trie = trie->parent;
        }

        for(int j = i - 1; j >= 0; j--)
        {
            printf("%c", indexToChar(buffer[j]));
        }
        printf("\n");
    }
}

void debugTrie()
{
    TrieNode* trie = getNode();
    for (int i = 0; i < 256; i++)
    {
        insertNode(trie, instructions[i].name, true);
    }

    for (int i = 0; i < 256; i++)
    {
        if(!findWord(trie, instructions[i].name))
        {
            printf("Couldn't find word: '%s'!\n", instructions[i].name);
        }
    }

    traverseTrie(trie); // traverse the entire trie just to be sure everything is in order

    if(findWord(trie, "random"))
    {
        printf("Found word that isn't supposed to be there.\n");
    }

    if(findWord(trie, "sflo"))
    {
        printf("Found word that isn't supposed to be there.\n");
    }

    freeTrie(trie);
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


