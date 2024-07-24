#ifndef MMIX_PROJ_TRIE
#define MMIX_PROJ_TRIE

#include <stdbool.h>
#include <stdint.h>
#include "common.h"

#define ALPHABET_SIZE 36

typedef struct TrieNode TrieNode;

typedef struct
{
	char* name;
	uint8_t op_code;
	int arity;
} Instruction;

Instruction instr[256] = {
	{	.name = "TRAP", 	.op_code= 0, 		.arity = -1},
	{	.name = "FCMP", 	.op_code = 1, 	.arity = 3},
	{	.name = "FUN", 		.op_code = 2,		.arity = 3},
	{	.name = "FEQL", 	.op_code = 3,		.arity = 3},
	{	.name = "FADD", 	.op_code = 4,		.arity = 3},
	{	.name = "FIX", 		.op_code = 5, 	.arity = 3},
	{	.name = "FSUB", 	.op_code = 6, 	.arity = 3},
	{	.name = "FIXU", 	.op_code = 7, 	.arity = 3},
	{	.name = "FLOT", 	.op_code = 8, 	.arity = 3},
	{	.name = "FLOTU", 	.op_code = 10, 	.arity = 3},
	{	.name = "SFLOT", 	.op_code = 12, 	.arity = 3},
	{	.name = "SFLOTU", .op_code = 14, 	.arity = 3},
	{	.name = "FMUL", 	.op_code = 16,	.arity = 3},
	{	.name = "FCMPE", 	.op_code = 17,  .arity = 3},
	{	.name = "FUNE", 	.op_code = 18, 	.arity = 3},
	{	.name = "FEQLE", 	.op_code = 19,  .arity = 3},
	{	.name = "FDIV", 	.op_code = 20,  .arity = 3},
	{	.name = "FSQRT", 	.op_code = 21,  .arity = 3},
	{	.name = "FREM",   .op_code = 22,  .arity = 3},
	{	.name = "FINT",   .op_code = 23,  .arity = 3},
	{	.name = "MUL", 		.op_code = 24,  .arity = 3},
	{	.name = "MULU", 	.op_code = 26,  .arity = 3},
	{	.name = "DIV", 		.op_code = 28, 	.arity = 3},
	{	.name = "DIVU", 	.op_code = 30, 	.arity = 3},
	{	.name = "ADD", 		.op_code = 32,  .arity = 3},
	{	.name = "ADDU", 	.op_code = 34,  .arity = 3},
	{	.name = "SUB", 		.op_code = 36,  .arity = 3},
	{	.name = "SUBU", 	.op_code = 38,  .arity = 3},
	{	.name = "2ADDU", 	.op_code = 40,  .arity = 3},
	{	.name = "4ADDU",  .op_code = 42,  .arity = 3},
	{	.name = "8ADDU",  .op_code = 44,  .arity = 3},
	{ .name = "16ADDU", .op_code = 46,  .arity = 3},
	{	.name = "CMP", 		.op_code = 48,  .arity = 3},
	{	.name = "CMPU", 	.op_code = 50,  .arity = 3},
	{	.name = "NEG", 		.op_code = 52,  .arity = 3},
	{	.name = "NEGU", 	.op_code = 54,  .arity = 3},
	{	.name = "SL", 		.op_code = 56,  .arity = 3},
	{	.name = "SLU", 		.op_code = 58,  .arity = 3},
	{	.name = "SR", 		.op_code = 60,  .arity = 3},
	{	.name = "SRU", 		.op_code = 62, 	.arity = 3},
	{	.name = "BN", 		.op_code = 64, 	.arity = 2},
	{	.name = "BZ", 		.op_code = 66,  .arity = 2},
	{	.name = "BP", 		.op_code = 68,  .arity = 2},
	{	.name = "BOD", 		.op_code = 70,  .arity = 2},
	{	.name = "BNN", 		.op_code = 72,  .arity = 2},
	{	.name = "BNZ", 		.op_code = 74,  .arity = 2},
	{	.name = "BNP", 		.op_code = 76,  .arity = 2},
	{	.name = "BEV", 		.op_code = 78,  .arity = 2},
	{	.name = "PBN", 		.op_code = 80,  .arity = 2},
	{	.name = "PBZ", 		.op_code = 82,  .arity = 2},
	{	.name = "PBP", 		.op_code = 84,  .arity = 2},
	{	.name = "PBOD", 	.op_code = 86,  .arity = 2},
	{	.name = "PBNN", 	.op_code = 88,  .arity = 2},
	{	.name = "PBNZ", 	.op_code = 90,  .arity = 2},
	{	.name = "PBNP", 	.op_code = 92,  .arity = 2},
	{	.name = "PBEV", 	.op_code = 94,  .arity = 2},
	{	.name = "CSN", 		.op_code = 96,  .arity = 3},
	{	.name = "CSZ", 		.op_code = 98,  .arity = 3},
	{	.name = "CSP", 		.op_code = 100, .arity = 3},
	{	.name = "CSOD", 	.op_code = 102, .arity = 3},
	{	.name = "CSNN", 	.op_code = 104, .arity = 3},
	{	.name = "CSNZ", 	.op_code = 106, .arity = 3},
	{	.name = "CSNP", 	.op_code = 108, .arity = 3},
	{ .name = "CSEV", 	.op_code = 110, .arity = 3},
	{	.name = "ZSN", 		.op_code = 112, .arity = 3},
	{	.name = "ZSZ", 		.op_code = 114, .arity = 3},
	{	.name = "ZSP", 		.op_code = 116, .arity = 3},
	{	.name = "ZSOD", 	.op_code = 118, .arity = 3},
	{	.name = "ZSNN", 	.op_code = 120, .arity = 3},
	{	.name = "ZSNZ", 	.op_code = 122, .arity = 3},
	{	.name = "ZSNP", 	.op_code = 124, .arity = 3},
	{	.name = "ZSEV", 	.op_code = 126, .arity = 3},
	{	.name = "LDB", 		.op_code = 128, .arity = 3},
	{	.name = "LDBU", 	.op_code = 130, .arity = 3},
	{	.name = "LDW", 		.op_code = 132, .arity = 3},
	{	.name = "LDWU", 	.op_code = 134, .arity = 3},
	{	.name = "LDT", 		.op_code = 136, .arity = 3},
	{	.name = "LDTU", 	.op_code = 138, .arity = 3},
	{	.name = "LDO", 		.op_code = 140, .arity = 3},
	{	.name = "LDOU", 	.op_code = 142, .arity = 3},
	{	.name = "LDSF", 	.op_code = 144, .arity = 3},
	{	.name = "LDHT", 	.op_code = 146, .arity = 3},
	{	.name = "CSWAP", 	.op_code = 148, .arity = 3},
	{	.name = "LDUNC", 	.op_code = 150, .arity = 3},
	{	.name = "LDVTS", 	.op_code = 152, .arity = 3},
	{	.name = "PRELD", 	.op_code = 154, .arity = 3},
	{	.name = "PREGO", 	.op_code = 156, .arity = 3},
	{	.name = "GO", 		.op_code = 158, .arity = 3},
	{	.name = "STB", 		.op_code = 160, .arity = 3},
	{	.name = "STBU", 	.op_code = 162, .arity = 3},
	{	.name = "STW", 		.op_code = 164, .arity = 3},
	{	.name = "STWU", 	.op_code = 166, .arity = 3},
	{	.name = "STT", 		.op_code = 168, .arity = 3},
	{	.name = "STTU", 	.op_code = 170, .arity = 3},
	{	.name = "STO", 		.op_code = 172, .arity = 3},
	{ .name = "STOU", 	.op_code = 174, .arity = 3},
	{	.name = "STSF", 	.op_code = 176, .arity = 3},
	{	.name = "STHT", 	.op_code = 178, .arity = 3},
	{	.name = "STCO", 	.op_code = 180, .arity = 3},
	{	.name = "STUNC", 	.op_code = 182, .arity = 3},
	{	.name = "SYNCD",  .op_code = 184, .arity = 3},
	{	.name = "PREST",  .op_code = 186, .arity = 3},
	{	.name = "SYNCID", .op_code = 188, .arity = 3},
	{	.name = "PUSHGO", .op_code = 190, .arity = 3},
	{	.name = "OR", 		.op_code = 192, .arity = 3},
	{	.name = "ORN", 		.op_code = 194, .arity = 3},
	{	.name = "NOR", 		.op_code = 196, .arity = 3},
	{	.name = "XOR", 		.op_code = 198, .arity = 3},
	{	.name = "AND", 		.op_code = 200, .arity = 3},
	{	.name = "ANDN", 	.op_code = 202, .arity = 3},
	{	.name = "NAND", 	.op_code = 204, .arity = 3},
	{	.name = "NXOR", 	.op_code = 206, .arity = 3},
	{	.name = "BDIF", 	.op_code = 208, .arity = 3},
	{	.name = "WDIF", 	.op_code = 210, .arity = 3},
	{	.name = "TDIF", 	.op_code = 212, .arity = 3},
	{	.name = "ODIF", 	.op_code = 214, .arity = 3},
	{	.name = "MUX", 		.op_code = 216, .arity = 3},
	{	.name = "SADD", 	.op_code = 218, .arity = 3},
	{	.name = "MOR", 		.op_code = 220, .arity = 3},
	{	.name = "MXOR", 	.op_code = 222, .arity = 3},
	{	.name = "SETH", 	.op_code = 224, .arity = 2},
	{	.name = "SETMH", 	.op_code = 225, .arity = 2},
	{	.name = "SETML", 	.op_code = 226, .arity = 2},
	{	.name = "SETL", 	.op_code = 227, .arity = 2},
	{	.name = "INCH", 	.op_code = 228, .arity = 2},
	{	.name = "INCMH", 	.op_code = 229, .arity = 2},
	{	.name = "INCML", 	.op_code = 230, .arity = 2},
	{ .name = "INCL", 	.op_code = 231, .arity = 2},
	{	.name = "ORH", 		.op_code = 232, .arity = 2},
	{	.name = "ORMH", 	.op_code = 233, .arity = 2},
	{	.name = "ORML", 	.op_code = 234, .arity = 2},
	{	.name = "ORL", 		.op_code = 235, .arity = 2},
	{	.name = "ANDNH", 	.op_code = 236, .arity = 2},
	{	.name = "ANDNMH", .op_code = 237, .arity = 2},
	{	.name = "ANDNML", .op_code = 238, .arity = 2},
	{	.name = "ANDNL", 	.op_code = 239, .arity = 2},
	{	.name = "JMP", 		.op_code = 240, .arity = 1},
	{	.name = "PUSHJ", 	.op_code = 242, .arity = 2},
	{	.name = "GETA", 	.op_code = 244, .arity = 2},
	{	.name = "PUT", 		.op_code = 246, .arity = 2},
	{	.name = "POP", 		.op_code = 248, .arity = 2},
	{	.name = "RESUME", .op_code = 249, .arity = 1},
	{	.name = "SAVE", 	.op_code = 250, .arity = 1}, // ?
	{	.name = "UNSAVE", .op_code = 251, .arity = 1},
	{	.name = "SYNC", 	.op_code = 252, .arity = 1},
	{	.name = "SWYM", 	.op_code = 253, .arity = -1}, // NOP
	{	.name = "GET", 		.op_code = 254, .arity = 2},
	{	.name = "TRIP", 	.op_code = 255, .arity = -1}
};

struct TrieNode
{
	TrieNode* nodes[ALPHABET_SIZE];
	int32_t isWord;
};

TrieNode* getNode(void);
void insertNode(TrieNode* node, char* word, int32_t value);
void createTrie(TrieNode* node, char** words, int32_t n);
int32_t findWord(TrieNode* node, char* word);
void createInstructionTrie(TrieNode* node);

#endif
