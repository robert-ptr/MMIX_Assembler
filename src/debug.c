#include "debug.h"

#ifdef ASM_TEST_SCANNER

static void dump_tokens()
{
    Token token;
    while(token.type != TOKEN_EOF)
    {
        token = scan_token();

        print_token(&token); 
    }
}

void debug_scanner()
{
    dump_tokens();
}
#endif

#ifdef ASM_TEST_PARSER
void debug_parser()
{
    parse();
}
#endif

#ifdef ASM_TEST_TABLE
void debug_table()
{
    // the simplest way to debug the table is to create one and perform all the actions you would perform on a hashtable
    // and see if/where it breaks

    Table test_table;
    init_table(&test_table);

    TableData bool_key;
    bool_key.as_bool = false;
    bool_key.type = TYPE_BOOL;

    TableData float_key;
    float_key.as_float = 3.156f;
    float_key.type = TYPE_FLOAT;

    TableData double_key;
    double_key.as_double = 2.718;
    double_key.type = TYPE_DOUBLE;

    TableData int_key;
    int_key.as_int = 74;
    int_key.type = TYPE_INT;

    TableData bool_value;
    bool_value.as_bool = true;
    bool_value.type = TYPE_BOOL;

    TableData float_value;
    float_value.as_float = -3.156f;
    float_value.type = TYPE_FLOAT;

    TableData double_value;
    double_value.as_double = -2.718;
    double_value.type = TYPE_DOUBLE;

    TableData int_value;
    int_value.as_int = 351;
    int_value.type = TYPE_INT;

    if(add_to_table(&test_table, &bool_key, &bool_value))
    {
        printf("Added bool value:%d to table, key: %d\n", bool_value.as_bool, bool_key.as_bool);
    }

    for (int i = 0; i < test_table.size; i++)
    {
        printf("%d: ", i);
        switch(test_table.entries[i].key.type)
        {
            case TYPE_UNASSIGNED:
                printf("TYPE_UNASSIGNED\n");
                break;
            case TYPE_INT:
                printf("TYPE_INT\n");
                break;
            case TYPE_STR:
                printf("TYPE_STR\n");
                break;
            case TYPE_FLOAT:
                printf("TYPE_FLOAT\n");
                break;
            case TYPE_DOUBLE:
                printf("TYPE_DOUBLE\n");
                break;
            case TYPE_BOOL:
                printf("TYPE_BOOL\n");
                break;
        }
    }

    if(add_to_table(&test_table, &float_key, &float_value))
    {
        printf("Added float value:%f to table, key:%f\n", float_value.as_float, float_key.as_float);
    }

    for (int i = 0; i < test_table.size; i++)
    {
        printf("%d: ", i);
        switch(test_table.entries[i].key.type)
        {
            case TYPE_UNASSIGNED:
                printf("TYPE_UNASSIGNED\n");
                break;
            case TYPE_INT:
                printf("TYPE_INT\n");
                break;
            case TYPE_STR:
                printf("TYPE_STR\n");
                break;
            case TYPE_FLOAT:
                printf("TYPE_FLOAT\n");
                break;
            case TYPE_DOUBLE:
                printf("TYPE_DOUBLE\n");
                break;
            case TYPE_BOOL:
                printf("TYPE_BOOL\n");
                break;
        }
    }

    if(add_to_table(&test_table, &double_key, &double_value))
    {
        printf("Added float value:%lf to table, key:%lf\n", double_value.as_double, double_key.as_double);
    }

    for (int i = 0; i < test_table.size; i++)
    {
        printf("%d: ", i);
        switch(test_table.entries[i].key.type)
        {
            case TYPE_UNASSIGNED:
                printf("TYPE_UNASSIGNED\n");
                break;
            case TYPE_INT:
                printf("TYPE_INT\n");
                break;
            case TYPE_STR:
                printf("TYPE_STR\n");
                break;
            case TYPE_FLOAT:
                printf("TYPE_FLOAT\n");
                break;
            case TYPE_DOUBLE:
                printf("TYPE_DOUBLE\n");
                break;
            case TYPE_BOOL:
                printf("TYPE_BOOL\n");
                break;
        }
    }

    if(add_to_table(&test_table, &int_key, &int_value))
    {
        printf("Added int value:%d to table, key: %d\n", int_value.as_int, int_key.as_int);
    }

    printf("\n\n");

    for (int i = 0; i < test_table.size; i++)
    {
        printf("%d: ", i);
        switch(test_table.entries[i].key.type)
        {
            case TYPE_UNASSIGNED:
                printf("TYPE_UNASSIGNED\n");
                break;
            case TYPE_INT:
                printf("TYPE_INT\n");
                break;
            case TYPE_STR:
                printf("TYPE_STR\n");
                break;
            case TYPE_FLOAT:
                printf("TYPE_FLOAT\n");
                break;
            case TYPE_DOUBLE:
                printf("TYPE_DOUBLE\n");
                break;
            case TYPE_BOOL:
                printf("TYPE_BOOL\n");
                break;
        }
    }

    TableData value;

    if(find_in_table(&test_table, &bool_key, &value))
    {
        printf("bool: %d\n", value.as_bool);
    }

    if(find_in_table(&test_table, &float_key, &value))
    {
        printf("float: %f\n", value.as_float);
    }

    if(find_in_table(&test_table, &double_key, &value))
    {
        printf("double: %lf\n", value.as_double);
    }

    if(find_in_table(&test_table, &int_key, &value))
    {
        printf("%d\n", value.as_int);
    }

    free_table(&test_table);
}
#endif

#ifdef ASM_TEST_TRIE
int number_of_words = 0;

static void traverse_trie(TrieNode* trie)
{
    bool leaf = true;
    for(int i = 0; i < ALPHABET_SIZE; i++)
    {
        if(trie->nodes[i] != NULL)
        {
            traverse_trie(trie->nodes[i]);
            leaf = false;
        }
    }

    if (leaf && !trie->is_word)
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
            printf("%c", index_to_char(buffer[j]));
        }
        printf("\n");
    }

    if(trie->is_word)
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
            printf("%c", index_to_char(buffer[j]));
        }
        printf("\n");
    }
}

void test_trie()
{
    TrieNode* trie = get_node();
    for (int i = 0; i < 256; i++)
    {
        insert_node(trie, instructions[i].name, true);
    }

    for (int i = 0; i < 256; i++)
    {
        if(!find_word(trie, instructions[i].name))
        {
            printf("Couldn't find word: '%s'!\n", instructions[i].name);
        }
    }

    traverse_trie(trie); // traverse the entire trie just to be sure everything is in order

    if(find_word(trie, "random"))
    {
        printf("Found word that isn't supposed to be there.\n");
    }

    if(find_word(trie, "sflo"))
    {
        printf("Found word that isn't supposed to be there.\n");
    }

    free_trie(trie);
}
#endif

/* these are all for debugging code in the VM */
static void dump_general_register(Tester* tester, uint8_t index)
{
	printf("G. Reg. number: %d value: %s", index, "placeholder"); // support for showing register value in binary,hexa and decimal
}

static void dump_special_register(Tester* tester, uint8_t index)
{
}

static void dump_general_registers(Tester* tester)
{
    for(int i = 0; i < 256; i++)
    {
        dump_general_register(tester, i);
    }
}

static void dump_special_registers(Tester* tester)
{
    for(int i = 0; i < 32; i++)
    {
        dump_special_register(tester, i);
    }
}

static void dump_memory(Tester* tester)
{
}

static void dump_register_stack(Tester* tester)
{
}

static void dump_stack_top(Tester* tester)
{
}

static void dump_stack(Tester* tester)
{
}

void debug(Tester* tester)
{
    // TO DO: think of features to add
    // step by step debugging
    // thinking of doing an interactive one and one that just dumps the state
    if(tester->flags & FLAG_GREGS)
    {
        dump_general_registers(tester);
    }

    if(tester->flags & FLAG_SPREGS)
    {
        dump_special_registers(tester);
    }

    if(tester->flags & FLAG_REGSTACK)
    {
        dump_register_stack(tester);
    }

    if(tester->flags & FLAG_INSTRSTACK)
    {
        dump_stack(tester);
    }

    if(tester->flags & FLAG_STACKTOP)
    {
        dump_stack_top(tester);
    }

    if(tester->flags & FLAG_MEM)
    {
        dump_memory(tester);
    }

    if(tester->flags & FLAG_REG)
    {
        if(tester->flags & FLAG_SPREG)
        {
            dump_special_register(tester, tester->flags & 0xFF00 >> 8);
        }
        else
        {
            dump_general_register(tester, tester->flags & 0xFF00 >> 8);
        }
    }
}
