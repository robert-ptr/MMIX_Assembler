#ifndef MMIX_TESTGER
#define MMIX_TESTGER

#include "vm.h"
#include "scanner.h"
#include "parser.h"
#include "trie.h"
#include "common.h"

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
} Tester;

void test(Tester* tester); // for the VM basically

#define ASM_TEST_SCANNER
#define ASM_TEST_PARSER
#define ASM_TEST_TABLE
#define ASM_TEST_TRIE

#ifdef ASM_TEST_SCANNER
void test_scanner();
#endif

#ifdef ASM_TEST_PARSER
void test_parser();
#endif

#ifdef ASM_TEST_TABLE
void test_table();
#endif

#ifdef ASM_TEST_TRIE
void test_trie();
#endif

#endif
