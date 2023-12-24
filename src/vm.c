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

Byte toByte(int value)
{
	return (Byte)value;
}

void execute(VM* vm)
{
	ByteSet byte_set = vm->byte_set;
	for(int i = 0; i < byte_set.count; i++)
	{
		switch(byte_set.bytes[i])
		{
			case OP_TRAP:
				break;
			case OP_FCMP:
				break;
			case OP_FUN:
				break;
			case OP_FEQL:
				break;
			case OP_FADD:
				break;
			case OP_FIX:
				break;
			case OP_FSUB:
				break;
			case OP_FIXU:
				break;
			case OP_FLOT:
				break;
			case OP_FLOTI:
				break;
			case OP_FLOTU:
				break;
			case OP_FLOTUI:
				break;
			case OP_SFLOT:
				break;
			case OP_SFLOTI:
				break;
			case OP_SFLOTU:
				break;
			case OP_SFLOTUI:
				break;
			case OP_FMUL:
				break;
			case OP_FCMPE:
				break;
			case OP_FUNE:
				break;
			case OP_FEQLE:
				break;
			case OP_FDIV:
				break;
			case OP_FSQRT:
				break;
			case OP_FREM:
				break;
			case OP_FINT:
				break;
			case OP_MUL:
				break;
			case OP_MULI:
				break;
			case OP_MULU:
				break;
			case OP_MULUI:
				break;
			case OP_DIV:
				break;
			case OP_DIVI:
				break;
			case OP_DIVU:
				break;
			case OP_DIVUI:
				break;
			case OP_ADD:
				break;
			case OP_ADDI:
				break;
			case OP_ADDU:
				break;
			case OP_ADDUI:
				break;
			case OP_SUB:
				break;
			case OP_SUBI:
				break;
			case OP_SUBU:
				break;
			case OP_SUBUI:
				break;
			case OP_2ADDU:
				break;
			case OP_2ADDUI:
				break;
			case OP_4ADDU:
				break;
			case OP_4ADDUI:
				break;
			case OP_8ADDU:
				break;
			case OP_8ADDUI:
				break;
			case OP_16ADDU:
				break;
			case OP_16ADDUI:
				break;
			case OP_CMP:
				break;
			case OP_CMPI:
				break;
			case OP_CMPU:
				break;
			case OP_CMPUI:
				break;
			case OP_NEG:
				break;
			case OP_NEGI:
				break;
			case OP_NEGU:
				break;
			case OP_NEGUI:
				break;
			case OP_SL:
				break;
			case OP_SLI:
				break;
			case OP_SLU:
				break;
			case OP_SLUI:
				break;
			case OP_SR:
				break;
			case OP_SRI:
				break;
			case OP_SRU:
				break;
			case OP_SRUI:
				break;
			case OP_BN:
				break;
			case OP_BNB:
				break;
			case OP_BZ:
				break;
			case OP_BZB:
				break;
			case OP_BP:
				break;
			case OP_BPB:
				break;
			case OP_BOD:
				break;
			case OP_BODB:
				break;
			case OP_BNN:
				break;
			case OP_BNNB:
				break;
			case OP_BNZ:
				break;
			case OP_BNZB:
				break;
			case OP_BNP:
				break;
			case OP_BNPB:
				break;
			case OP_BEV:
				break;
			case OP_BEVB:
				break;
			case OP_PBN:
				break;
			case OP_PBNB:
				break;
			case OP_PBZ:
				break;
			case OP_PBZB:
				break;
			case OP_PBP:
				break;
			case OP_PBPB:
				break;
			case OP_PBOD:
				break;
			case OP_PBODB:
				break;
			case OP_PBNN:
				break;
			case OP_PBNNB:
				break;
			case OP_PBNZ:
				break;
			case OP_PBNZB:
				break;
			case OP_PBNP:
				break;
			case OP_PBNPB:
				break;
			case OP_PBEV:
				break;
			case OP_PBEVB:
				break;
			case OP_CSN:
				break;
			case OP_CSNI:
				break;
			case OP_CSZ:
				break;
			case OP_CSZI:
				break;
			case OP_CSP:
				break;
			case OP_CSPI:
				break;
			case OP_CSOD:
				break;
			case OP_CSODI:
				break;
			case OP_CSNN:
				break;
			case OP_CSNNI:
				break;
			case OP_CSNZ:
				break;
			case OP_CSNZI:
				break;
			case OP_CSNP:
				break;
			case OP_CSNPI:
				break;
			case OP_CSEV:
				break;
			case OP_CSEVI:
				break;
			case OP_ZSN:
				break;
			case OP_ZSNI:
				break;
			case OP_ZSZ:
				break;
			case OP_ZSZI:
				break;
			case OP_ZSP:
				break;
			case OP_ZSPI:
				break;
			case OP_ZSOD:
				break;
			case OP_ZSODI:
				break;
			case OP_ZSNN:
				break;
			case OP_ZSNII:
				break;
			case OP_ZSNZ:
				break;
			case OP_ZSNZI:
				break;
			case OP_ZSNP:
				break;
			case OP_ZSNPI:
				break;
			case OP_ZSEV:
				break;
			case OP_ZSEVI:
				break;
			case OP_LDB:
				break;
			case OP_LDBI:
				break;
			case OP_LDBU:
				break;
			case OP_LDBUI:
				break;
			case OP_LDW:
				break;
			case OP_LDWI:
				break;
			case OP_LDWU:
				break;
			case OP_LDWUI:
				break;
			case OP_LDT:
				break;
			case OP_LDTI:
				break;
			case OP_LDTU:
				break;
			case OP_LDTUI:
				break;
			case OP_LDO:
				break;
			case OP_LDOI:
				break;
			case OP_LDOU:
				break;
			case OP_LDOUI:
				break;
			case OP_LDSF:
				break;
			case OP_LSDFI:
				break;
			case OP_LDHT:
				break;
			case OP_LDHTI:
				break;
			case OP_CSWAP:
				break;
			case OP_CSWAPI:
				break;
			case OP_LDUNC:
				break;
			case OP_LDUNCI:
				break;
			case OP_LDVTS:
				break;
			case OP_LDVTSI:
				break;
			case OP_PRELD:
				break;
			case OP_PRELDI:
				break;
			case OP_PREGO:
				break;
			case OP_PREGOI:
				break;
			case OP_GO:
				break;
			case OP_GOI:
				break;
			case OP_STB:
				break;
			case OP_STBI:
				break;
			case OP_STBU:
				break;
			case OP_STBUI:
				break;
			case OP_STW:
				break;
			case OP_STWI:
				break;
			case OP_STWU:
				break;
			case OP_STWUI:
				break;
			case OP_STT:
				break;
			case OP_STTI:
				break;
			case OP_STTU:
				break;
			case OP_STTUI:
				break;
			case OP_STO:
				break;
			case OP_STOI:
				break;
			case OP_STOU:
				break;
			case OP_STOUI:
				break;
			case OP_STSF:
				break;
			case OP_STSFI:
				break;
			case OP_STHT:
				break;
			case OP_STHTI:
				break;
			case OP_STCO:
				break;
			case OP_STCOI:
				break;
			case OP_STUNC:
				break;
			case OP_STUNCI:
				break;
			case OP_SYNCD:
				break;
			case OP_SYNCDI:
				break;
			case OP_PREST:
				break;
			case OP_PRESTI:
				break;
			case OP_SYNCID:
				break;
			case OP_SYNCIDI:
				break;
			case OP_PUSHGO:
				break;
			case OP_PUSHGOI:
				break;
			case OP_OR:
				break;
			case OP_ORI:
				break;
			case OP_ORN:
				break;
			case OP_ORNI:
				break;
			case OP_NOR:
				break;
			case OP_NORI:
				break;
			case OP_XOR:
				break;
			case OP_XORI:
				break;
			case OP_AND:
				break;
			case OP_ANDI:
				break;
			case OP_ANDN:
				break;
			case OP_ANDNI:
				break;
			case OP_NAND:
				break;
			case OP_NANDI:
				break;
			case OP_NXOR:
				break;
			case OP_NXORI:
				break;
			case OP_BDIF:
				break;
			case OP_BDIFI:
				break;
			case OP_WDIF:
				break;
			case OP_WDIFI:
				break;
			case OP_TDIF:
				break;
			case OP_TDIFI:
				break;
			case OP_ODIF:
				break;
			case OP_ODIFI:
				break;
			case OP_MUX:
				break;
			case OP_MUXI:
				break;
			case OP_SADD:
				break;
			case OP_SADDI:
				break;
			case OP_MOR:
				break;
			case OP_MORI:
				break;
			case OP_MXOR:
				break;
			case OP_MXORI:
				break;
			case OP_SETH:
				break;
			case OP_SETMH:
				break;
			case OP_SETML:
				break;
			case OP_SETL:
				break;
			case OP_INCH:
				break;
			case OP_INCHMH:
				break;
			case OP_INCML:
				break;
			case OP_INCL:
				break;
			case OP_ORH:
				break;
			case OP_ORMH:
				break;
			case OP_ORML:
				break;
			case OP_ORL:
				break;
			case OP_ANDH:
				break;
			case OP_ANDNMH:
				break;
			case OP_ANDML:
				break;
			case OP_ANDNL:
				break;
			case OP_JMP:
				break;
			case OP_JMPB:
				break;
			case OP_PUSHJ:
				break;
			case OP_PUSHJB:
				break;
			case OP_GETA:
				break;
			case OP_GETAB:
				break;
			case OP_PUT:
				break;
			case OP_PUTI:
				break;
			case OP_POP:
				break;
			case OP_RESUME:
				break;
			case OP_SAVE:
				break;
			case OP_UNSAVE:
				break;
			case OP_SYNC:
				break;
			case OP_SWYM:
				break;
			case OP_GET:
				break;
			case OP_TRIP:
				break;
		}
	}
}
