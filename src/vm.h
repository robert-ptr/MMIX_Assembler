#ifndef mmix_proj_vm
#define mmix_proj_vm

#include <stddef.h>
#include <stdint.h>

#define STACK_MAX 512

typedef enum
{
/* 0x */  OP_TRAP,  OP_FCMP,    OP_FUN,   OP_FEQL,    OP_FADD,    OP_FIX,     OP_FSUB,    OP_FIXU,
/* 0x */  OP_FLOT,  OP_FLOTI,   OP_FLOTU, OP_FOLTUI,  OP_SFLOT,   OP_SFLOTI,  OP_SFLOTU,  OP_SFLOTUI,
/* 1x */  OP_FMUL,  OP_FCMPE,   OP_FUNE,  OP_FEQLE,   OP_FDIV,    OP_FSQRT,   OP_FREM,    OP_FINT,
/* 1x */  OP_MUL,   OP_MULI,    OP_MULU,  OP_MULUI,   OP_DIV,     OP_DIVI,    OP_DIVU,    OP_DIVUI,
/* 2x */  OP_ADD,   OP_ADDI,    OP_ADDU,  OP_ADDUI,   OP_SUB,     OP_SUBI,    OP_SUBU,    OP_SUBUI,
/* 2x */  OP_2ADDU, OP_2ADDUI,  OP_4ADDU, OP_4ADDUI,  OP_8ADDU,   OP_8ADDUI,  OP_16ADDU,  OP_16ADDUI,
/* 3x */  OP_CMP,   OP_CMPI,    OP_CMPU,  OP_CMPUI,   OP_NEG,     OP_NEGI,    OP_NEGU,    OP_NEGUI,
/* 3x */  OP_SL,    OP_SLI,     OP_SLU,   OP_SLUI,    OP_SR,      OP_SRI,     OP_SRU,     OP_SRUI,
/* 4x */  OP_BN,    OP_BNB,     OP_BZ,    OP_BZB,     OP_BP,      OP_BPB,     OP_BOD,     OP_BODB,
/* 4x */  OP_BNN,   OP_BNNB,    OP_BNZ,   OP_BNZB,    OP_BNP,     OP_BNPB,    OP_BEV,     OP_BEVB,
/* 5x */  OP_PBN,   OP_PBNB,    OP_PBZ,   OP_PBZB,    OP_PBP,     OP_PBPB,    OP_BPOD,    OP_PBODB,
/* 5x */  OP_PBNN,  OP_PBNNB,   OP_PBNZ,  OP_PBNZB,   OP_PBNP,    OP_PBNPB,   OP_PBEV,    OP_PBEVB,
/* 6x */  OP_CSN,   OP_CSNI,    OP_CSZ,   OP_CSZI,    OP_CSP,     OP_CSPI,    OP_CSOD,    OP_CSODI,
/* 6x */  OP_CSNN,  OP_CSNNI,   OP_CSNZ,  OP_CSNZI,   OP_CSNP,    OP_CSNPI,   OP_CSEV,    OP_CSEVI,
/* 7x */  OP_ZSN,   OP_ZSNI,    OP_ZSZ,   OP_ZSZI,    OP_ZSP,     OP_ZSPI,    OP_ZSOD,    OP_ZSODI,
/* 7x */  OP_ZSNN,  OP_ZSNNI,   OP_ZSNZ,  OP_ZSNZI,   OP_ZSNP,    OP_ZSNPI,   OP_ZSEV,    OP_ZSEVI,
/* 8x */  OP_LDB,   OP_LDBI,    OP_LDBU,  OP_LDBUI,   OP_LDW,     OP_LDWI,    OP_LDWU,    OP_LDWUI,
/* 8x */  OP_LDT,   OP_LDTI,    OP_LDTU,  OP_LDTUI,   OP_LDO,     OP_LDOI,    OP_LDOU,    OP_LDOUI,
/* 9x */  OP_LDSF,  OP_LDSFI,   OP_LDHT,  OP_LDHTI,   OP_CSWAP,   OP_CSWAPI,  OP_LDUNC,   OP_LDUNCI,
/* 9x */  OP_LDVTS, OP_LDVTSI,  OP_PRELD, OP_PRELDI,  OP_PREGO,   OP_PREGOI,  OP_GO,      OP_GOI,
/* Ax */  OP_STB,   OP_STBI,    OP_STBU,  OP_STBUI,   OP_STW,     OP_STWI,    OP_STWU,    OP_STWUI,
/* Ax */  OP_STT,   OP_STTI,    OP_STTU,  OP_STTUI,   OP_STO,     OP_STOI,    OP_STOU,    OP_STOUI,
/* Bx */  OP_STSF,  OP_STSFI,   OP_STHT,  OP_STHTI,   OP_STCO,    OP_STCOI,   OP_STUNC,   OP_STUNCI,
/* Bx */  OP_SYNCD, OP_SYNCDI,  OP_PREST, OP_PRESTI,  OP_SYNCID,  OP_SYNCIDI, OP_PUSHGO,  OP_PUSHGOI,
/* Cx */  OP_OR,    OP_ORI,     OP_ORN,   OP_ORNI,    OP_NOR,     OP_NORI,    OP_XOR,     OP_XORI,
/* Cx */  OP_AND,   OP_ANDI,    OP_ANDN,  OP_ANDNI,   OP_NAND,    OP_NANDI,   OP_NXOR,    OP_NXORI,
/* Dx */  OP_BDIF,  OP_BDIFI,   OP_WDIF,  OP_WDIFI,   OP_TDIF,    OP_TDIFI,   OP_ODIF,    OP_ODIFI,
/* Dx */  OP_MUX,   OP_MUXI,    OP_SADD,  OP_SADDI,   OP_MOR,     OP_MORI,    OP_MXOR,    OP_MXORI,
/* Ex */  OP_SETH,  OP_SETMH,   OP_SETML, OP_SETL,    OP_INCH,    OP_INCHMH,  OP_INCML,   OP_INCL,
/* Ex */  OP_ORH,   OP_ORMH,    OP_ORML,  OP_ORL,     OP_ANDH,    OP_ANDNMH,  OP_ANDML,   OP_ANDNL,
/* Fx */  OP_JMP,   OP_JMPB,    OP_PUSHJ, OP_PUSHJB,  OP_GETA,    OP_GETAB,   OP_PUT,     OP_PUTI,
/* Fx */  OP_POP,   OP_RESUME,  OP_SAVE,  OP_UNSAVE,  OP_SYNC,    OP_SWYM,    OP_GET,     OP_TRIP,
/* Other operations */ OP_IS, OP_GREG, OP_LOC, OP_BYTE, OP_WYDE, OP_TETRA, OP_OCTA,
} OpType;

typedef uint8_t Byte;

typedef struct
{
	int count;
	int capacity;
	Byte* bytes;
} ByteSet;

typedef struct
{
	ByteSet* byte_set;
	Byte* ip;
	int64_t stack[STACK_MAX];
	int64_t* stack_top;
} VM;

void initByteSet(ByteSet* byte_set);
void freeByteSet(ByteSet* byte_set);
void initVM(VM* vm);
void freeVM(VM* vm);

#endif
