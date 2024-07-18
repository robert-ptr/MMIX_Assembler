#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vm.h"

static void addition64bit(uint64_t a, uint64_t b, uint64_t* result) // done to avoid overflow
{
	uint32_t p1 = a >> 32;
	uint32_t p2 = a & 0xFFFFFFFF;
	uint32_t p3 = b >> 32;
	uint32_t p4 = b & 0xFFFFFFFF;

	uint64_t _result1 = p1 + p3; // max 33 bits
	uint64_t _result2 = (p2 + p4 + _result1 >> 32) & 0xFFFFFFFF; // max 33 bits
	_result1 &= 0xFFFFFFFF;
	*result = _result1 | (result_2 << 32);
}

static void multiplication64bit(uint64_t a, uint64_t b, uint64_t* result1, uint64_t* result2)
{
	uint32_t p1 = a >> 32;
	uint32_t p2 = a & 0xFFFFFFFF;
	uint32_t p3 = b >> 32;
	uint32_t p4 = b & 0xFFFFFFFF;

	uint64_t _result1 = p2 * p4; // smallest, represents a part of the last 64 bits
	uint64_t _result2 = p2 * p3; // 32-96 both
	uint64_t _result3 = p2 * p4; // 32-96 both
	uint64_t _result4 = p1 * p3; // biggest, represents a part of the first 64 bits
					 
	if(_result4 >> 32 == 0)
	{
		*result1 = _result1 + _result2 + _result3 + _resul4;
	}
	else
	{
		uint64_t rest = _result1 >> 32 + _result2 & 0xFFFFFFFF + _result3 & 0xFFFFFFFF;
		*result1 = _result1 + rest & 0xFFFFFFFF;
		*result2 = _result4 + _result2 >> 32 + _result2 >> 32 + rest >> 32;
	}
}

static void initByteSet(ByteSet* byte_set)
{
	byte_set->count = 0;
	byte_set->capacity = 0;
	byte_set->bytes = NULL;
}

static void growByteSet(ByteSet* byte_set)
{
	int32_t capacity = GROW_LIST(byte_set->capacity);
	Byte* new_list = (Byte*)malloc(capacity * sizeof(Byte)); 

	for(int32_t i = 0; i < byte_set->count; i++)
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

static void freeByteSet(ByteSet* byte_set)
{
	free(byte_set->bytes);
	initByteSet(byte_set);
}

static void initMemory(Table* memory)
{
	initTable(memory);
}

static void freeMemory(Table* memory)
{
	freeTable(memory);
}

void initVM(VM* vm)
{
	initByteSet(vm->byte_set);
	initMemory(vm->memory);
}

void freeVM(VM* vm)
{
	freeByteSet(vm->byte_set);
	freeMemory(vm->memory);
}

static bool isAtEnd(VM* vm)
{
	return vm->ip == NULL;
}

static Byte currentByte(VM* vm)
{
	if(!isAtEnd(vm))
		return *vm->ip; 
	return NULL;
}

static Byte getByte(VM* vm)
{
	if(!isAtEnd(vm))
		return *(vm-ip++);

	return NULL;
}

static uint64_t* getReg(VM* vm, uint8_t index)
{
	return vm->general_registers[index];
}

static uint64_t* getSpecialReg(VM* vm, uint8_t index)
{
	return vm->special_registers[index];
}

static int64_t s(uint64_t unsig)
{
	int64_t sig;
	// convert from unsigned to signed
	sig = unsig - (unsig >> 63) * (1 >> 64);
	return sig;
}

static void addByteToMem(VM* vm, uint64_t address, Byte byte)
{
	uint8_t offset = address % 8;
	address -= offset;

	char* key = intToStr(address);
	
	uint64_t value;
	findInTable(vm->table, key, &value);
	value = value | (0xFF << (8 * offset)) & byte << (8 * offset);

	addToTable(vm->memory, key, value);
}

static void addWydeToMem(VM* vm, uint64_t address, Wyde wyde)
{
	uint8_t offset = address % 4;
	address -= offset;

	char* key = intToStr(address);
	
	uint64_t value;
	findInTable(vm->table, key, &value);
	value = value | (0xFFFF << (16 * offset)) & wyde << (16 * offset);

	addToTable(vm->memory, key, value);
}

static void addTetraToMem(VM* vm, uint64_t address, Tetra tetra)
{
	uint8_t offset = address % 2;
	address -= offset;

	char* key = intToStr(address);
	
	uint64_t value;
	findInTable(vm->table, key, &value);
	value = value | (0xFFFFFFFF << (32 * offset)) & tetra << (32 * offset);

	addToTable(vm->memory, key, value);
}

static void addOctaToMem(VM* vm, uint64_t address, Octa octa)
{
	char* key = intToStr(address);

	addToTable(vm->memory, key, octa);
}

static Byte getByteFromMem(VM* vm, uint64_t address)
{
	int* val;
	uint8_t offset = address % 8;
	address -= offset;

	char* key = intToStr(address);

	if(findInTable(vm->memory, key, val)
	{
		return *val << (8 * (7 - offset)) >> 56;
	}

	return 0;
}

static Wyde getWydeFromMem(VM* vm, uint64_t address)
{
	int* val;
	uint8_t offset = address % 4;
	address -= offset;

	char* key = intToStr(address);

	if(findInTable(vm->memory, key, val)
	{
		return *val << (16 * (3 - offset)) >> 48;
	}

	return 0;

}

static Tetra getTetraFromMem(VM* vm, uint64_t address)
{
	int* val;
	uint8_t offset = address % 2;
	address -= offset;

	char* key = intToStr(address);

	if(findInTable(vm->memory, key, val)
	{
		return *val << (32 * (1 - offset)) >> 32;
	}

	return 0;

}

static Octa getOctaFromMem(VM* vm, uint64_t address)
{
	int* val;
	char* key = intToStr(address);

	if(findInTable(vm->memory, key, val)
	{
		return *val;
	}

	return 0;

}

void execute(VM* vm)
{
	ByteSet byte_set = vm->byte_set;
	for(;;)
	{
		if(isAtEnd(vm))
			return;

		Byte X,Y,Z;
		Byte byte = getByte(vm);
		X = getByte(vm);
		Y = getByte(vm);
		Z = getByte(vm);
		
		Register* reg_X = getReg(vm, X);
		Register* reg_Y = getReg(vm, Y);
		Register* reg_Z = getReg(vm, Z);
		Octa* A;
		Octa* result1;
		Octa* result2;

		switch(byte)
		{
					case OP_TRAP: // thiscommand is analogous to TRIP, but it forces a trap to the operating system.
						break;
					case OP_FCMP: // floating compare: s($X)<-[f($Y) > f($Z)] - [f($Y) < f($Z)]
						break;
					case OP_FUN: // floating unordered: s($X)<-[f($Y) || f($Z)]?
						break;
					case OP_FEQL: // floating equal to: s($X)<-[f($Y)=f($Z)]
						break;
					case OP_FADD: // floating add: f($X)<-f($Y)+f($Z)
						break;
					case OP_FIX: // convert floating to fixed: s($X)<-int32_t f($Z)
						break;
					case OP_FSUB: // floating subtract: f($X)<-f($Y)-f($Z)
						break;
					case OP_FIXU: // convert floating to fixed unsigned: u($X)<-[int32_t f($Z)) mod 2^64
						break;
					case OP_FLOT: // convert fixed to floating: f($X)<-s($Z)
						break;
					case OP_FLOTI:
						break;
					case OP_FLOTU: // convert fixed to floating unsigned: f($X)<-u($Z)
						break;
					case OP_FLOTUI:
						break;
					case OP_SFLOT: // convert fixed to short float: f($X)<-f(T)<-s($Z)
						break;
					case OP_SFLOTI:
						break;
					case OP_SFLOTU: // convert fixed to short float unsigned: f($X)<-f(T)<-u($Z)
						break;
					case OP_SFLOTUI:
						break;
					case OP_FMUL: // floating multiply: f($X)<-f($Y)/f($Z)
						break;
					case OP_FCMPE: // floating compare with respect to epsilon: s($X)<-[f($Y) > f($Z) (f(rE))] - [f($Y) < f($Z) (f(rE))]
						break;
					case OP_FUNE: // floating unordered with respect to epsilon: s($X)<-[f($Y) || f($Z) (f(rE))]
						break;
					case OP_FEQLE: // floating equivalent with respect to epsilon: s($X)<-[f($Y) ~= f($Z) (f(rE))]
						break;
					case OP_FDIV: // floating divide: f($X)<-f($Y)/f($Z)
						break;
					case OP_FSQRT: // floating square root: f($X)<-f($Z)^(1/2)
						break;
					case OP_FREM: // floating remainder: f($X)<f($Y) rem f($Z)
						break;
					case OP_FINT: // floating integer: f($X)<-int32_t f($Z)
						break;
					case OP_MUL: //multiply s($X)<-s($Y)xs($Z)
						multiplication64bit(*reg_Y, *reg_Z, reg_X, result2);
						if(s(*reg_Y) > 0 && s(*reg_Z) < 0 || s(*reg_Z) < 0 && s(*reg_Y) > 0)
							*reg_X = -*reg_X;
						break;
					case OP_MULI:
						multiplication64bit(*reg_Y, Z, reg_X, result2);
						if(s(*reg_Y) > 0 && s(*reg_Z) < 0 || s(*reg_Z) < 0 && s(*reg_Y) > 0)
							*reg_X = -*reg_X;
						break;
					case OP_MULU: // u(rH $X)<-u($Y)xu($Z)
						multiplication64bit(*reg_Y, *reg_Z, reg_X, getSpecialReg(vm, rH));
						break;
					case OP_MULUI:
						multiplication64bit(*reg_Y, Z, reg_X, getSpecialReg(vm, rH));
						break;
					case OP_DIV: //divide s($X)<-floor(s($Y)/s($Z))[$Z!=0] and s(rR)<-s($Y)mod s($Z)
						*reg_X = s(*reg_Y) / s(*reg_Z);
						*getSpecialReg(vm, rR) = s(*reg_Y) % s(*reg_Z);
						break;
					case OP_DIVI:
						*reg_X = s(*reg_Y) / s(Z);
						*getSpecialReg(vm, rR) = s(*reg_Y) % s(Z);
						break;
					case OP_DIVU: // u($X)<-floor(u(rD $Y) / u($Z)), u(rR)<-u(rD $Y) mod u($Z), if (u($Z) > u(rD)); otherwise $X<-rD, rR<-$Y
						*reg_X = *reg_Y / *reg_Z;
						*getSpecialReg(vm, rR) = *reg_Y % *reg_Z;
						break;
					case OP_DIVUI:
						*reg_X = *reg_Y / Z;
						*getSpecialRegister(vm, rR) = *reg_Y % Z;
						break;
					case OP_ADD: //add s($X)<-s($Y)+s($Z)
						if(s(*reg_Y) < 0 && s(*reg_Z) > 0)
						{
							*reg_X = s(*reg_Z) + s(*reg_Y);
						}
						else if(s(*reg_Y) > 0 && s(*reg_Z) < 0)
						{
							*reg_X = s(*reg_Z) + s(*reg_Y); //same as above
						}
						else if(s(*reg_Y) < 0 && s(*reg_Z) < 0)
						{
							addition64bit(*reg_Y, *reg_Z, reg_X);
							*reg_X = -*reg_X;
						}
						else
						{
							addition64bit(*reg_Y, *reg_Z, reg_X);
						}
						break;
					case OP_ADDI:
						if(s(*reg_Y) < 0 && s(Z) > 0)
						{
							*reg_X = s(*reg_Z) + s(Z);
						}
						else if(s(*reg_Y) > 0 && s(Z) < 0)
						{
							*reg_X = s(*reg_Z) + s(Z); //same as above
						}
						else if(s(*reg_Y) < 0 && s(Z) < 0)
						{
							addition64bit(*reg_Y, Z, reg_X);
							*reg_X = -*reg_X;
						}
						else
						{
							addition64bit(*reg_Y, *reg_Z, reg_X);
						}
						break;
/* LOC*/	case OP_ADDU: // u($X)<-(u($Y)+u($Z)) mod 2^64 ;OP_LDA is equivalent to a version of this
						addition64bit(*reg_Y, *reg_Z, reg_X);
						break;
					case OP_ADDUI:
						addition64bit(*reg_Y, Z, reg_X);
						break;
					case OP_SUB: //subtract s($X)<-S($Y)-S($Z)
					case OP_SUBI:
					case OP_SUBU:
					case OP_SUBUI:
						byte -= OP_SUB;

						if(byte % 2 == 0 && byte / 2 == 0)
							*reg_X = s(*reg_Y) - s(*reg_Z);
						else if(byte % 2 == 1 && byte / 2 == 0)
							*reg_X = s(*reg_Y) - s(Z);
						else if(byte % 2 == 0 && byte / 2 == 1)
							*reg_X = *reg_Y - *reg_Z;
						else
							*reg_X = *reg_Y - Z;

						break;
					case OP_2ADDU: //times 2 and add unsigned u($X)<-(u($Y)x2+u($Z)) mod 2^64
					case OP_2ADDUI:
					case OP_4ADDU: // times 4 and add unsigned
					case OP_4ADDUI:
					case OP_8ADDU: // times 8 and add unsigned
					case OP_8ADDUI:
					case OP_16ADDU: // times 16 and add unsigned
					case OP_16ADDUI:
					{
						byte -= OP_2ADDU;
						uint64_t* result1;
						uint64_t* result2;
						uint64_t* result;
						uint64_t operand1;
						uint64_t operand2;
						uint64_t operand3;
					
						if(byte % 2 == 1)
							operand3 = Z;
						else
							operand3 = *reg_Z;

						if(byte / 2 == 0)
							operand2 = 2;
						else if(byte / 2 == 1)
							operand2 = 4;
						else if(byte / 2 == 2)
							operand2 = 8;
						else
							operand2 = 16;

						multiplication64bit(operand1, operand2, result1, result2);
						addition64bit(result1, operand3, reg_X);

						break;
					}
					case OP_CMP: // compare s($X)<-[s($Y) > s($Z)] - [s($Y) < s($Z)]
						*reg_X = (s(*reg_Y) > s(*reg_Z)) - (s(*reg_Y) < s(*reg_Z));
						break;
					case OP_CMPI:
						*reg_X = (s(*reg_Y) > s(Z)) - (s(*reg_Y) < s(Z));
						break;
					case OP_CMPU: // s($X)<-[u($Y) > u($Z)] - [u($Y) < u($Z)]
						*reg_X = (*reg_Y > *reg_Z) - (*reg_Y < *reg_Z);
						break;
					case OP_CMPUI:
						*reg_X = (*reg_Y > Z) - (*reg_Y < Z);
						break;
					case OP_NEG:	// negate s($X)<-Y-s($Z)
						*reg_X = Y - s(*reg_Z);
						break;
					case OP_NEGI:
						*reg_X = Y - s(Z);
						break;
					case OP_NEGU: // u($X)<-(Y-u($Z))mod 2^64
						*reg_X = Y - *reg_Z;
						*reg_X %= 1 << 64;
						break;
					case OP_NEGUI:
						*reg_X = Y - Z;
						*reg_X %= 1 << 64;
						break;
					case OP_SL: // shift left s($X)<-s($Y)x2^(u($Z))
						*reg_X = s(*reg_Y) << *reg_Z;
						break;
					case OP_SLI:
						*reg_X = s(*reg_Y) << Z;
						break;
					case OP_SLU: // s($X)<-u($Y)x2^(u($Z)) mod 2^64
						*reg_X = *reg_Y << *reg_Z;
						break;
					case OP_SLUI:
						*reg_X = *reg_Y << Z;
						break;
					case OP_SR: // shift right s($X)<-floor(s($Y)/2^u($Z))
						*reg_X = s(*reg_Y) >> *reg_Z;
						break;
					case OP_SRI:
						*reg_X = s(*reg_Y) >> Z;
						break;
					case OP_SRU: // u($X)<-floor(u($Y)/2^u($Z))
						*reg_X = *reg_Y >> *reg_Z;
						break;
					case OP_SRUI:
						*reg_X = *reg_Y >> Z;
						break;
					case OP_BN: // branch if negative: if s($X) < 0, set @<-RA
						break;
					case OP_BNB:
						break;
					case OP_BZ:	// branch if zero: if $X, set @<-RA
						break;
					case OP_BZB:
						break;
					case OP_BP: // branch if positive: if s($X)>0, set@<-RA
						break;
					case OP_BPB:
						break;
					case OP_BOD: // branch if odd: if s($X) mod 2 = 1, set @<-RA
						break;
					case OP_BODB:
						break;
					case OP_BNN: // branch if nonnegative: if s($X)>=0, set @<-RA
						break;
					case OP_BNNB:
						break;
					case OP_BNZ: // branch if nonzero: if $X!=0, set@<-RA
						break;
					case OP_BNZB:
						break;
					case OP_BNP: // branch if nonpositive: if s($X)<=0, set @<-RA
						break;
					case OP_BNPB:
						break;
					case OP_BEV: // branch if even: if s($X) mod 2 = 0, set @<-RA
						break;
					case OP_BEVB:
						break;
					case OP_PBN: // probable branch if negative: if s($X)<0, set @<-RA
						break;
					case OP_PBNB:
						break;
					case OP_PBZ: // probable branch if zero: if $X=0, set @<-RA
						break;
					case OP_PBZB:
						break;
					case OP_PBP: // probable branch if positive: if s($X) > 0, set @<-RA
						break;
					case OP_PBPB:
						break;
					case OP_PBOD: // probable branch if odd: if s($X) mod 2 = 1, set @<-RA
						break;
					case OP_PBODB:
						break;
					case OP_PBNN: // probable branch if nonnegative: if s($X) >= 0, set @<-RA
						break;
					case OP_PBNNB:
						break;
					case OP_PBNZ: // probable if nonzero: if $X != 0, set @<-RA
						break;
					case OP_PBNZB:
						break;
					case OP_PBNP: // probable branch if nonpositive: if s($X) <= 0, set @<-RA
						break;
					case OP_PBNPB:
						break;
					case OP_PBEV: // probable branch if even: if s($X) mod 2 = 0, set @<-RA
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
					case OP_LDBI: 
					case OP_LDBU: 
					case OP_LDBUI:
					case OP_LDW: // load wyde s($X)<-s(M2[A])
					case OP_LDWI:
					case OP_LDWU:
					case OP_LDWUI:
					case OP_LDT: // load tetra s($X)<-s(M4[A])
					case OP_LDTI:
					case OP_LDTU:
					case OP_LDTUI:
					case OP_LDO: // load octa s($X)<-s(M8[A])
					case OP_LDOI:
					case OP_LDOU:
					case OP_LDOUI:
					{
						byte -= OP_LDB;
						
						if(byte % 2 == 1)
							addition64bit(*reg_Y, *reg_Z, A);
						else
							addition64bit(*reg_Y, Z, A);

						if(byte % 4 == 2 || byte % 4 == 3)
						{
							if(byte / 4 == 0)
								*reg_X = getByteFromMem(vm, A);
							else if(byte / 4 == 1)
								*reg_X = getWydeFromMem(vm, A);	
							else if(byte / 4 == 2)
								*reg_X = getTetraFromMem(vm, A);
							else
								*reg_X = getOctaFromMem(vm, A);
						}
						else
						{
							if(byte / 4 == 0)
								*reg_X = s(getByteFromMem(vm, A));
							else if(byte / 4 == 1)
								*reg_X = s(getWydeFromMem(vm, A));	
							else if(byte / 4 == 2)
								*reg_X = s(getTetraFromMem(vm, A));
							else
								*reg_X = s(getOctaFromMem(vm, A));
						}

						break;
					}
					case OP_LDSF: // load short float: f($X)<-f(M4[A]_
						break;
					case OP_LSDFI:
						break;
					case OP_LDHT: // load high tetra
					case OP_LDHTI:
						if(byte % 2 == 1)
							addition64bit(*reg_Y, *reg_Z, A);
						else
							addition64bit(*reg_Y, Z, A);
						
						*reg_X = getTetraFromMem(vm, A) << 32;
						break;
					case OP_CSWAP: // compare and swap octabytes: if u(M8[A] = u(rP), where rP is the special prediction reigster,
												 // set u(M8[A]<-u($X) and u($X)<-1. Otherwise set u(rP)<-u(M8[A]) and u($X)<-0.
												 // This is an atomic operation, useful when independent computers share a common memory.
						break;
					case OP_CSWAPI:
						break;
					case OP_LDUNC: //load octa unchached: s($X)<-s(M8[A]).
						break;
					case OP_LDUNCI:
						break;
					case OP_LDVTS: // load virtual trasnlation status: for the operating system only.
						break;
					case OP_LDVTSI:
						break;
					case OP_PRELD: // preload data: says that all of the bytes M[A] through M[A+X] will probably be loaded or stored in the near future.
						break;
					case OP_PRELDI:
						break;
					case OP_PREGO: // prefetch to go: says that many of the bytes M[A] through M[A+X] will probably be used as instructions in the near future.
						break;
					case OP_PREGOI:
						break;
					case OP_GO: // go: u($X)<-@+4, then @<-a
						break;
					case OP_GOI:
						break;
					case OP_STB: // store byte s(M1[A])<-s($X)
					case OP_STBI:
					case OP_STBU: // u(M1[A])<-u($X) mod 2^8
					case OP_STBUI:
					case OP_STW: // store wyde s(M2[A])<-s($X)
					case OP_STWI:
					case OP_STWU: // u(M2[A])<-u($X) mod 2^16
					case OP_STWUI:
					case OP_STT: // store tetra s(M4[A])<-s($X)
					case OP_STTI:
					case OP_STTU: // u(M4[A])<-u($X) mod 2^32
					case OP_STTUI:
					case OP_STO: // store octo s(M8[A])<-s($X)
					case OP_STOI:
					case OP_STOU: // u(M8[A])<-u($X) mod 2^64
					case OP_STOUI:
						byte -= OP_STB;
						
						if(byte % 2 == 1)
							addition64bit(*reg_Y, *reg_Z, A);
						else
							addition64bit(*reg_Y, Z, A);

						if(byte / 4 == 0)
						{
							if(byte / 2 % 2 == 1)
							{
								addByteToMem(vm, A, *reg_X);
							}
							else
							{
								addByteToMem(vm, A, s(*reg_X));
							}
						}
						else if(byte / 4 == 1)
						{
							if(byte / 2 % 2 == 1)
							{
								addWydeToMem(vm, A, *reg_X);
							}
							else
							{
								addWydeToMem(vm, A, s(*reg_X));
							}
						}
						else if(byte / 4 == 2)
						{
							if(byte / 2 % 2 == 1)
							{
								addTetraToMem(vm, A, *reg_X);
							}
							else
							{
								addTetraToMem(vm, A, s(*reg_X));
							}
						}
						else
						{
							if(byte / 2 % 2 == 1)
							{
								addOctaToMem(vm, A, *reg_X);
							}
							else
							{
								addOctaToMem(vm, A, s(*reg_X));
							}
						}

						break;
					case OP_STSF: // store short float: f(M4[A])<-f($X)
						break;
					case OP_STSFI:
						break;
					case OP_STHT:
					case OP_STHTI:
						if(byte % 2 == 1)
							addition64bit(*reg_Y, *reg_Z, A);
						else
							addition64bit(*reg_Y, Z, A);
						
						addTetraToMem(vm, A, *reg_X >> 32);
						break;
					case OP_STCO: // store constant octabyte u(M8[A])<-X
					case OP_STCOI:
						if(byte % 2 == 1)
							addition64bit(*reg_Y, *reg_Z, A);
						else
							addition64bit(*reg_Y, Z, A);
	
						addOctaToMem(vm, A, X);
						break;
					case OP_STUNC: // store octa uncached: s(M8[A)<-s($X)
						break;
					case OP_STUNCI:
						break;
					case OP_SYNCD: // synchronize data: says that all of the bytes M[A] through M[A+X] must be brought up to date in the physical memory, so that other computers and input/output devices can read them.
						break;
					case OP_SYNCDI:
						break;
					case OP_PREST: // prestore data: says that all of the bytes M[A] through M[A+X] will definitely be written(stored) before they are next read( loaded).
						break;
					case OP_PRESTI:
						break;
					case OP_SYNCID: // synchronize instructions and data: says that all of the bytes M[A] through M[A+X] must be fetched again before being interpreted as instructions.
						break;
					case OP_SYNCIDI:
						break;
					case OP_PUSHGO: // push registers and go: push(X) and set rJ<-@ + 4, then set @<-A
						break;
					case OP_PUSHGOI:
						break;
/* SET */	case OP_OR: // bitwise or: v($X)<-v($Y) | v($Z)
/* $X $Y */	
/* <=> */
/* OR $X $Y 0 */
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
					case OP_SETH: // set high wyde: u($X)<-YZ x 2^48
						break;
					case OP_SETMH: // set medium high wyde: u($X)<-YZ x 2^32
						break;
					case OP_SETML: // set medium low wide: u($X)<-YZ x 2^16
						break;
/*SET*/		case OP_SETL: // set low wyde: u($X)<-YZ
						break;
					case OP_INCH: // increase by high wyde: u($X)<-(u($X) + YZ) mod 2^64
						break;
					case OP_INCHMH: // increase by medium high wyde: u($X)<-(u($X) + YZ x 2^32) mod 2^64 
						break;
					case OP_INCML: // increase by medium low wyde: u($X)<-(u($X) + YZ x 2^16) mod 2^64
						break;
					case OP_INCL: // increase by low wyde: u($X)<-(u($X) + YZ) mod 2^64
						break;
					case OP_ORH: // bitwise or with high wyde: v($X)<-v($X)|v(YZ<<48)
						break;
					case OP_ORMH: // bitwise or with medium high wyde: v($X)<-v($X)|v(YZ<<32)
						break;
					case OP_ORML: // bitwise or with medium low wyde: v($X)<-v($X)|v(YZ<<16)
						break;
					case OP_ORL: // bitwise or with low wyde: v($X)<-v($X)|v(YZ)
						break;
					case OP_ANDH: // bitwise and-not high wyde: v($X)<-v($X)&!v(YZ<<48)
						break;
					case OP_ANDNMH: // bitwise and-not medium high wyde: v($X)<-v($X)&!v(YZ<<32)
						break;
					case OP_ANDML: // bitwise and-not medium low wyde: v($X)<-v($X)&!v(YZ<<16)
						break;
					case OP_ANDNL: // bitwise and-not low wyde: v($X)<-v($X)&!v(YZ)
						break;
					case OP_JMP: // jump: @<-RA (a three byte relative address) <=> @+4xXYZ (can also be negative)
						break;
					case OP_JMPB:
						break;
					case OP_PUSHJ: // push registers and jump: push(X) and set rJ<-@ + 4, then set @<-RA
						break;
					case OP_PUSHJB:
						break;
					case OP_GETA: // get address: u($X)<-RA
						break;
					case OP_GETAB:
						break;
					case OP_PUT: // put into special register: u(g[X])<-u($Z), where 0 <= X < 32
						break;
					case OP_PUTI:
						break;
					case OP_POP: // pop registers and return: pop(X), then @<-rJ + 4xYZ
						break;
					case OP_RESUME: // resume after interrupt
						break;
					case OP_SAVE: // save process state: u($X)<-context
						break;
					case OP_UNSAVE: // resstore process state: context<-u($Z)
						break;
					case OP_SYNC: // synchronize: restricts parallel activities so that different processors can cooperate reliably;
						break;
					case OP_SWYM: // symphatize with your machinery: nop
						break;
					case OP_GET: // get from special register: u($X)<-u(g[Z]), where 0 <= Z < 32
						break;
					case OP_TRIP: //this command forces a trip to the handler at location #00
						break;
		}
	}
}
