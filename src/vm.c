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
			case OP_FADD: // floating add: f($X)<-f($Y)+f($Z)
				break;
			case OP_FIX:
				break;
			case OP_FSUB: // floating subtract: f($X)<-f($Y)-f($Z)
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
			case OP_FMUL: // floating multiply: f($X)<-f($Y)/f($Z)
				break;
			case OP_FCMPE:
				break;
			case OP_FUNE:
				break;
			case OP_FEQLE:
				break;
			case OP_FDIV: // floating divide: f($X)<-f($Y)/f($Z)
				break;
			case OP_FSQRT: // floating square root: f($X)<-f($Z)^(1/2)
				break;
			case OP_FREM: // floating remainder: f($X)<f($Y) rem f($Z)
				break;
			case OP_FINT: // floating integer: f($X)<-int f($Z)
				break;
			case OP_MUL: //multiply s($X)<-s($Y)xs($Z)
				break;
			case OP_MULI:
				break;
			case OP_MULU: // u(rH $X)<-u($Y)xu($Z)
				break;
			case OP_MULUI:
				break;
			case OP_DIV: //divide s($X)<-floor(s($Y)/s($Z))[$Z!=0] and s(rR)<-s($Y)mod s($Z)
				break;
			case OP_DIVI:
				break;
			case OP_DIVU: // u($X)<-floor(u(rD $Y) / u($Z)), u(rR)<-u(rD $Y) mod u($Z), if (u($Z) > u(rD)); otherwise $X<-rD, rR<-$Y
				break;
			case OP_DIVUI:
				break;
			case OP_ADD: //add s($X)<-s($Y)+s($Z)
				break;
			case OP_ADDI:
				break;
			case OP_ADDU: // u($X)<-(u($Y)+u($Z)) mod 2^64 ;OP_LDA is equivalent to a version of this
				break;
			case OP_ADDUI:
				break;
			case OP_SUB: //subtract s($X)<-S($Y)-S($Z)
				break;
			case OP_SUBI:
				break;
			case OP_SUBU:
				break;
			case OP_SUBUI:
				break;
			case OP_2ADDU: //times 2 and add unsigned u($X)<-(u($Y)x2+u($Z)) mod 2^64
				break;
			case OP_2ADDUI:
				break;
			case OP_4ADDU: // times 4 and add unsigned
				break;
			case OP_4ADDUI:
				break;
			case OP_8ADDU: // times 8 and add unsigned
				break;
			case OP_8ADDUI:
				break;
			case OP_16ADDU: // times 16 and add unsigned
				break;
			case OP_16ADDUI:
				break;
			case OP_CMP: // compare s($X)<-[s($Y) > s($Z)] - [s($Y) < s($Z)]
				break;
			case OP_CMPI:
				break;
			case OP_CMPU: // s($X)<-[u($Y) > u($Z)] - [u($Y) < u($Z)]
				break;
			case OP_CMPUI:
				break;
			case OP_NEG:	// negate s($X)<-Y-s($Z)
				break;
			case OP_NEGI:
				break;
			case OP_NEGU: // s($X)<-(Y-u($Z))mod 2^64
				break;
			case OP_NEGUI:
				break;
			case OP_SL: // shift left s($X)<-s($Y)x2^(u($Z))
				break;
			case OP_SLI:
				break;
			case OP_SLU: // s($X)<-s($Y)x2^(u($Z)) mod 2^64
				break;
			case OP_SLUI:
				break;
			case OP_SR: // shift right s($X)<-floor(s($Y)/2^u($Z))
				break;
			case OP_SRI:
				break;
			case OP_SRU: // u($X)<-floor(u($Y)/2^u($Z))
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
			case OP_CSN: // conditional set if negative: if s($Y) < 0, set $X<-$Z
				break;
			case OP_CSNI:
				break;
			case OP_CSZ: // conditional set if zero: if $Y = 0, set $X<-$Z
				break;
			case OP_CSZI:
				break;
			case OP_CSP: // conditional set if positive: if s($Y) > 0, set $X<-$Z
				break;
			case OP_CSPI:
				break;
			case OP_CSOD: // conditional set if odd: if s($Y) mod 2 = 1, set $X<-$Z
				break;
			case OP_CSODI:
				break;
			case OP_CSNN: // conditional set if nonnegative: if s($Y) >= 0, set $X<-$Z
				break;
			case OP_CSNNI:
				break;
			case OP_CSNZ: // conditional set if nonzero: if $Y != 0, set $X<-$Z
				break;
			case OP_CSNZI:
				break;
			case OP_CSNP: // conditional set if nonpositive: if s($Y) <= 0, set $X<-$Z
				break;
			case OP_CSNPI:
				break;
			case OP_CSEV: // conditional set if even:	if s($Y) mod 2 = 0, set $X<-$Z
				break;
			case OP_CSEVI:
				break;
			case OP_ZSN: // zero or set if negative: $X<-$Z[s($Y) < 0]
				break;
			case OP_ZSNI:
				break; 
			case OP_ZSZ: // zero or set if zero: $X<-$Z[$Y = 0]
				break;
			case OP_ZSZI:
				break;
			case OP_ZSP: // zero or set if positive: $X<-$Z[s($Y) > 0]
				break;
			case OP_ZSPI:
				break;
			case OP_ZSOD: // zero or set if odd: $X<-$Z[s($Y) mod 2 = 1]
				break;
			case OP_ZSODI:
				break;
			case OP_ZSNN: // zero or set if nonnegative: $X<-$Z[s($Y) >= 0]
				break;
			case OP_ZSNNI:
				break;
			case OP_ZSNZ: // zero or set if nonzero: $X<-$Z[$Y!=0]
				break;
			case OP_ZSNZI:
				break;
			case OP_ZSNP: // zero or set if nonpositive: $X<-$Z[s($Y) <= 0]
				break;
			case OP_ZSNPI:
				break;
			case OP_ZSEV: // zero or set if even: $X<-$Z[s($Y) mod 2 = 0]
				break;
			case OP_ZSEVI:
				break;
			case OP_LDB: // load byte s($X)<-s(M1[A])
				break;
			case OP_LDBI: 
				break;
			case OP_LDBU: 
				break;
			case OP_LDBUI:
				break;
			case OP_LDW: // load wyde s($X)<-s(M2[A])
				break;
			case OP_LDWI:
				break;
			case OP_LDWU:
				break;
			case OP_LDWUI:
				break;
			case OP_LDT: // load tetra s($X)<-s(M4[A])
				break;
			case OP_LDTI:
				break;
			case OP_LDTU:
				break;
			case OP_LDTUI:
				break;
			case OP_LDO: // load octra s($X)<-s(M8[A])
				break;
			case OP_LDOI:
				break;
			case OP_LDOU:
				break;
			case OP_LDOUI:
				break;
			case OP_LDSF: // load short float
				break;
			case OP_LSDFI:
				break;
			case OP_LDHT: // load high tetra
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
			case OP_STB: // store byte s(M1[A])<-s($X)
				break;
			case OP_STBI:
				break;
			case OP_STBU: // u(M1[A])<-u($X) mod 2^8
				break;
			case OP_STBUI:
				break;
			case OP_STW: // store wyde s(M2[A])<-s($X)
				break;
			case OP_STWI:
				break;
			case OP_STWU: // u(M2[A])<-u($X) mod 2^16
				break;
			case OP_STWUI:
				break;
			case OP_STT: // store tetra s(M4[A])<-s($X)
				break;
			case OP_STTI:
				break;
			case OP_STTU: // u(M4[A])<-u($X) mod 2^32
				break;
			case OP_STTUI:
				break;
			case OP_STO: // store octo s(M8[A])<-s($X)
				break;
			case OP_STOI:
				break;
			case OP_STOU: // u(M8[A])<-u($X) mod 2^64
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
			case OP_STCO: // store constant octabyte u(M8[A])<-X
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
			case OP_OR: // bitwise or: v($X)<-v($Y) | v($Z)
				break;
			case OP_ORI:
				break;
			case OP_ORN: // bitwise or-not: v($X)<-v($Y) | !v($Z)
				break;
			case OP_ORNI:
				break;
			case OP_NOR: // bitwise not-or: !v($X)<-v($Y) | v($Z)
				break;
			case OP_NORI:
				break;
			case OP_XOR: // bitwise exclusive-or: v($X)<-v($Y) ^ v($Z)
				break;
			case OP_XORI:
				break;
			case OP_AND: // bitwise and: v($X)<-v($Y) & v($Z)
				break;
			case OP_ANDI:
				break;
			case OP_ANDN: // bitwise and-not: v($X)<-v($Y) & !v($Z)
				break;
			case OP_ANDNI:
				break;
			case OP_NAND: // bitwise not-and: !v($X)<-v($Y) & v($Z)
				break;
			case OP_NANDI:
				break;
			case OP_NXOR: // bitwise not-exclusive-or: !v($X)<-v($Y) ^ v($Z)
				break;
			case OP_NXORI:
				break;
			// y .-. z = max(0, y - z) (saturating subtraction)
			case OP_BDIF: // byte difference: b($X)<-b($Y) .-. b($Z)
				break;
			case OP_BDIFI:
				break;
			case OP_WDIF: // wyde difference: w($X)<-w($Y) .-. w($Z)
				break;
			case OP_WDIFI:
				break;
			case OP_TDIF: // tetra difference: t($X)<-t($Y) .-. t($Z)
				break;
			case OP_TDIFI:
				break;
			case OP_ODIF: // octa difference: u($X)<-u($Y) .-. u($Z)
				break;
			case OP_ODIFI:
				break;
			case OP_MUX: // bitwise multiplex (combines two bit vectors by looking at the special multiples mask regiser rM: v($X)<-(v($Y) & v(rM)) | (v($Z) & !v(rM))
				break;
			case OP_MUXI:
				break;
			case OP_SADD: // sideways add(counts the number of bit positions in which register $Y has a 1 while register $Z has a 0: s($X)<-s(sum(v($Y) & !v($Z)))
				break;
			case OP_SADDI:
				break;
			case OP_MOR: // multiple or: m^T($X)<-m^T($Y) |(on matrices) m^T($Z) <=> m($X)<-m($Z) |(on matrices) m($Y)
				break;
			case OP_MORI:
				break;
			case OP_MXOR: // multipe exclusive-or (same as MOR but with  ^(on matrices)
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
