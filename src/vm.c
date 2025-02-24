#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "vm.h"

#define GROW_SET(x) ((x) == 0 ? 8 : ((x) * 2))

uint8_t branch_time = 0;

RunningTime times[256] ={   {.oopsies=5 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 0x 
                            {.oopsies=4 , .mems=0}, {.oopsies=4 , .mems=0}, {.oopsies=4 , .mems=0 }, {.oopsies=4 , .mems=0 }, // 0x
                            {.oopsies=4 , .mems=0}, {.oopsies=4 , .mems=0}, {.oopsies=4 , .mems=0 }, {.oopsies=4 , .mems=0 }, // 0x
                            {.oopsies=4 , .mems=0}, {.oopsies=4 , .mems=0}, {.oopsies=4 , .mems=0 }, {.oopsies=4 , .mems=0 }, // 0x
                            {.oopsies=4 , .mems=0}, {.oopsies=4 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=4 , .mems=0 }, // 1x
                            {.oopsies=40, .mems=0}, {.oopsies=40, .mems=0}, {.oopsies=4 , .mems=0 }, {.oopsies=4 , .mems=0 }, // 1x
                            {.oopsies=10, .mems=0}, {.oopsies=10, .mems=0}, {.oopsies=10, .mems=0 }, {.oopsies=10, .mems=0 }, // 1x 
                            {.oopsies=60, .mems=0}, {.oopsies=60, .mems=0}, {.oopsies=60, .mems=0 }, {.oopsies=60, .mems=0 }, // 1x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 2x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 2x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 2x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 2x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 3x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 3x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 3x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 3x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 4x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 4x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 4x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 4x
                            {.oopsies=3 , .mems=0}, {.oopsies=3 , .mems=0}, {.oopsies=3 , .mems=0 }, {.oopsies=3 , .mems=0 }, // 5x
                            {.oopsies=3 , .mems=0}, {.oopsies=3 , .mems=0}, {.oopsies=3 , .mems=0 }, {.oopsies=3 , .mems=0 }, // 5x
                            {.oopsies=3 , .mems=0}, {.oopsies=3 , .mems=0}, {.oopsies=3 , .mems=0 }, {.oopsies=3 , .mems=0 }, // 5x
                            {.oopsies=3 , .mems=0}, {.oopsies=3 , .mems=0}, {.oopsies=3 , .mems=0 }, {.oopsies=3 , .mems=0},  // 5x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 6x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 6x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 6x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 6x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 7x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 7x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 7x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // 7x
                            {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=1 }, // 8x
                            {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=1 }, // 8x
                            {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=1 }, // 8x
                            {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=1 }, // 8x
                            {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=1 }, // 9x
                            {.oopsies=2 , .mems=2}, {.oopsies=2 , .mems=2}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=1 }, // 9x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=0 }, // 9x
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=3 , .mems=0 }, {.oopsies=3 , .mems=0 }, // 9x
                            {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=1 }, // Ax
                            {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=1 }, // Ax
                            {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=1 }, // Ax
                            {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=1 }, // Ax
                            {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=1 }, // Bx
                            {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1}, {.oopsies=1 , .mems=1 }, {.oopsies=1 , .mems=1 }, // Bx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Bx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=3 , .mems=0 }, {.oopsies=3 , .mems=0 }, // Bx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Cx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Cx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Cx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Cx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Dx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Dx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Dx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Dx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Ex
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Ex
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Ex
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Ex
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Fx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=1 , .mems=0 }, // Fx
                            {.oopsies=3 , .mems=0}, {.oopsies=5 , .mems=0}, {.oopsies=1 , .mems=20}, {.oopsies=1 , .mems=20}, // Fx
                            {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0}, {.oopsies=1 , .mems=0 }, {.oopsies=5 , .mems=0 }};// Fx

static void addition64bit(uint64_t a, uint64_t b, uint64_t* result) // done to avoid overflow
{
    uint32_t p1 = a >> 32;
    uint32_t p2 = a & 0xFFFFFFFF;
    uint32_t p3 = b >> 32;
    uint32_t p4 = b & 0xFFFFFFFF;

    uint64_t _result1 = p1 + p3; // max 33 bits
    uint64_t _result2 = (p2 + p4 + _result1 >> 32) & 0xFFFFFFFF; // max 33 bits
    _result1 &= 0xFFFFFFFF;
    *result = _result1 | (_result2 << 32);
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
        *result1 = _result1 + _result2 + _result3 + _result4;
    }
    else
    {
        uint64_t rest = _result1 >> 32 + _result2 & 0xFFFFFFFF + _result3 & 0xFFFFFFFF;
        *result1 = _result1 + rest & 0xFFFFFFFF;
        *result2 = _result4 + _result2 >> 32 + _result2 >> 32 + rest >> 32;
    }
}

void initVM(VM* vm)
{
    vm->memory = (Table*)malloc(sizeof(Table));
    initTable(vm->memory);
}

void freeVM(VM* vm)
{
    freeTable(vm->memory);
}

static bool isAtEnd(VM* vm)
{
    return vm->ip == NULL;
}

static Byte currentByte(VM* vm)
{
    return *vm->ip; 
}

static Byte getByte(VM* vm)
{
    return *(vm->ip++);
}

static MMIX_Register* getReg(VM* vm, uint8_t index)
{
    return &vm->general_registers[index];
}

static MMIX_Register* getSpecialReg(VM* vm, uint8_t index)
{
    return &vm->special_registers[index];
}

static int64_t s(uint64_t unsig)
{
    int64_t sig;
    // convert from unsigned to signed
    sig = unsig - (unsig >> 63 << 63); // hope this works the way I think it works
    return sig;
}

static void addByteToMem(VM* vm, uint64_t address, Byte byte)
{
    uint8_t offset = address % 8;
    address -= offset;

    char* key = intToHexString(address, 64);
    
    EntryValue value;
    value.type = TYPE_INT;

    size_t n = strlen(key);

    findInTable(vm->memory, key, n, &value);
    value.as_int = value.as_int | (0xFF << (8 * offset)) & byte << (8 * offset);

    addToTable_uint64_t(vm->memory, key, n, value.as_int);
}

static void addWydeToMem(VM* vm, uint64_t address, Wyde wyde)
{
    uint8_t offset = address % 4;
    address -= offset;
    char* key = intToHexString(address, 64);
    
    EntryValue value;
    value.type = TYPE_INT;

    size_t n = strlen(key);

    findInTable(vm->memory, key, n, &value);
    value.as_int = value.as_int | (0xFFFF << (16 * offset)) & wyde << (16 * offset);

    addToTable_uint64_t(vm->memory, key, n, value.as_int);
}

static void addTetraToMem(VM* vm, uint64_t address, Tetra tetra)
{
    uint8_t offset = address % 2;
    address -= offset;
    
    char* key = intToHexString(address, 64);
    
    EntryValue value;
    value.type = TYPE_INT;

    size_t n = strlen(key);

    findInTable(vm->memory, key, n, &value);
    value.as_int = value.as_int | (0xFFFFFFFF << (32 * offset)) & tetra << (32 * offset);

    addToTable_uint64_t(vm->memory, key, n, value.as_int);
}

static void addOctaToMem(VM* vm, uint64_t address, Octa octa)
{
    char* key = intToHexString(address, 64);

    addToTable_uint64_t(vm->memory, key, strlen(key), octa);
}

static Byte getByteFromMem(VM* vm, uint64_t address)
{
    EntryValue value;
    value.type = TYPE_INT;

    uint8_t offset = address % 8;
    address -= offset;
    char* key = intToHexString(address, 64);

    if(findInTable(vm->memory, key, strlen(key), &value))
    {
        return value.as_int << (8 * (7 - offset)) >> 56;
    }
    
    return 0;
}

static Wyde getWydeFromMem(VM* vm, uint64_t address)
{
    EntryValue value;
    value.type = TYPE_INT;

    uint8_t offset = address % 4;
    address -= offset;
    char* key = intToHexString(address, 64);

    if(findInTable(vm->memory, key, strlen(key), &value))
    {
        return value.as_int << (16 * (3 - offset)) >> 48;
    }
    
    return 0;
}

static Tetra getTetraFromMem(VM* vm, uint64_t address)
{
    EntryValue value;
    value.type = TYPE_INT;

    uint8_t offset = address % 2;
    address -= offset;
    char* key = intToHexString(address, 64);

    if(findInTable(vm->memory, key, strlen(key), &value))
    {
        return value.as_int << (32 * (1 - offset)) >> 32;
    }
    
    return 0;
}

static Octa getOctaFromMem(VM* vm, uint64_t address)
{   
    EntryValue value;
    value.type = TYPE_INT;

    char* key = intToHexString(address, 64);

    if(findInTable(vm->memory, key, strlen(key), &value))
    {
        return value.as_int;
    }
    
    return 0;
}

void execute(VM* vm)
{
    for(;;)
    {
        if(isAtEnd(vm))
            return;

        Byte X,Y,Z;
        Byte byte = getByte(vm);
        X = getByte(vm);
        Y = getByte(vm);
        Z = getByte(vm);
        
        uint64_t reg_X = getReg(vm, X);
        uint64_t reg_Y = getReg(vm, Y);
        uint64_t reg_Z = getReg(vm, Z);
        Octa A;
        Octa result1;
        Octa result2;

        switch(byte)
        {
                    case OP_TRAP:   // this command is analogous to TRIP, but it forces a trap to the operating system.
                                                    // 5 oops
                        break;
                    case OP_FCMP:   // floating compare: s($X)<-[f($Y) > f($Z)] - [f($Y) < f($Z)]
                                                    // 1 oops
                        break;
                    case OP_FUN:        // floating unordered: s($X)<-[f($Y) || f($Z)]?
                                                    // 1 oops
                        break;
                    case OP_FEQL:   // floating equal to: s($X)<-[f($Y)=f($Z)]
                                                    // 1 oops
                        break;
                    case OP_FADD:   // floating add: f($X)<-f($Y)+f($Z)
                                                    // 4 oops
                        break;
                    case OP_FIX:        // convert floating to fixed: s($X)<-int32_t f($Z)
                                                    // 4 oops
                        break;
                    case OP_FSUB:   // floating subtract: f($X)<-f($Y)-f($Z)
                                                    // 4 oops
                        break;
                    case OP_FIXU:   // convert floating to fixed unsigned: u($X)<-[int32_t f($Z)) mod 2^64
                                                    // 4 oops
                        break;

                    case OP_FLOT:   // convert fixed to floating: f($X)<-s($Z)
                                                    // 4 oops
                        break;
                    case OP_FLOTI:
                        break;
                    case OP_FLOTU:  // convert fixed to floating unsigned: f($X)<-u($Z)
                                                    // 4 oops
                        break;
                    case OP_FLOTUI:
                        break;
                    case OP_SFLOT:  // convert fixed to short float: f($X)<-f(T)<-s($Z)
                                                    // 4 oops
                        break;
                    case OP_SFLOTI:
                        break;
                    case OP_SFLOTU: // convert fixed to short float unsigned: f($X)<-f(T)<-u($Z)
                                                    // 4 oops
                        break;
                    case OP_SFLOTUI:
                        break;
                    
                    case OP_FMUL:   // floating multiply: f($X)<-f($Y)/f($Z)
                                                    // 4 oops
                        break;
                    case OP_FCMPE:  // floating compare with respect to epsilon: s($X)<-[f($Y) > f($Z) (f(rE))] - [f($Y) < f($Z) (f(rE))]
                                                    // 4 oops
                        break;
                    case OP_FUNE:   // floating unordered with respect to epsilon: s($X)<-[f($Y) || f($Z) (f(rE))]
                                                    // 4 oops
                        break;
                    case OP_FEQLE:  // floating equivalent with respect to epsilon: s($X)<-[f($Y) ~= f($Z) (f(rE))]
                                                    // 4 oops
                        break;
                    case OP_FDIV:   // floating divide: f($X)<-f($Y)/f($Z)
                                                    // 40 oops
                        break;
                    case OP_FSQRT:  // floating square root: f($X)<-f($Z)^(1/2)
                                                    // 40 oops
                        break;
                    case OP_FREM:   // floating remainder: f($X)<f($Y) rem f($Z)
                                                    // 4 oops
                        break;
                    case OP_FINT:   // floating integer: f($X)<-int32_t f($Z)
                                                    // 4 oops
                        break;
                    
                        // 10 oops
                    case OP_MUL:        //multiply s($X)<-s($Y)xs($Z)
                        multiplication64bit(*reg_Y, *reg_Z, reg_X, &result2);
                        if(s(*reg_Y) > 0 && s(*reg_Z) < 0 || s(*reg_Z) < 0 && s(*reg_Y) > 0)
                            *reg_X = -*reg_X;
                        break;
                    case OP_MULI:
                        multiplication64bit(*reg_Y, Z, reg_X, &result2);
                        if(s(*reg_Y) > 0 && s(*reg_Z) < 0 || s(*reg_Z) < 0 && s(*reg_Y) > 0)
                            *reg_X = -*reg_X;
                        break;
                    case OP_MULU:   // u(rH $X)<-u($Y)xu($Z)
                        multiplication64bit(*reg_Y, *reg_Z, reg_X, getSpecialReg(vm, rH));
                        break;
                    case OP_MULUI:
                        multiplication64bit(*reg_Y, Z, reg_X, getSpecialReg(vm, rH));
                        break;
                        
                        // 60 oops
                    case OP_DIV:        //divide s($X)<-floor(s($Y)/s($Z))[$Z!=0] and s(rR)<-s($Y)mod s($Z)
                        *reg_X = s(*reg_Y) / s(*reg_Z);
                        *getSpecialReg(vm, rR) = s(*reg_Y) % s(*reg_Z);
                        break;
                    case OP_DIVI:
                        *reg_X = s(*reg_Y) / s(Z);
                        *getSpecialReg(vm, rR) = s(*reg_Y) % s(Z);
                        break;
                    case OP_DIVU:   // u($X)<-floor(u(rD $Y) / u($Z)), u(rR)<-u(rD $Y) mod u($Z), if (u($Z) > u(rD)); otherwise $X<-rD, rR<-$Y
                        *reg_X = *reg_Y / *reg_Z;
                        *getSpecialReg(vm, rR) = *reg_Y % *reg_Z;
                        break;
                    case OP_DIVUI:
                        *reg_X = *reg_Y / Z;
                        *getSpecialReg(vm, rR) = *reg_Y % Z;
                        break;
                        
                        // 1 oops
                    case OP_ADD:        //add s($X)<-s($Y)+s($Z)
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
/* LOC*/            case OP_ADDU:       // u($X)<-(u($Y)+u($Z)) mod 2^64 ;OP_LDA is equivalent to a version of this
                        addition64bit(*reg_Y, *reg_Z, reg_X);
                        break;
                    case OP_ADDUI:
                        addition64bit(*reg_Y, Z, reg_X);
                        break;
                        
                        // 1 oops
                    case OP_SUB:            //subtract s($X)<-S($Y)-S($Z)
                        *reg_X = s(*reg_Y) - s(*reg_Z);
                        break;
                    case OP_SUBI:
                        *reg_X = s(*reg_Y) - s(Z);
                        break;
                    case OP_SUBU:
                        *reg_X = *reg_Y - *reg_Z;
                        break;
                    case OP_SUBUI:
                        *reg_X = *reg_Y - Z;
                        break;                  
                        
                        // 1 oops
                    case OP_2ADDU:      //times 2 and add unsigned u($X)<-(u($Y)x2+u($Z)) mod 2^64
                        multiplication64bit(*reg_Y, 2, &result1, &result2);
                        addition64bit(result1, *reg_Z, reg_X);
                        break;
                    case OP_2ADDUI:
                        multiplication64bit(*reg_Y, 2, &result1, &result2);
                        addition64bit(result1, Z, reg_X);
                        break;
                    case OP_4ADDU:      // times 4 and add unsigned
                        multiplication64bit(*reg_Y, 4, &result1, &result2);
                        addition64bit(result1, *reg_Z, reg_X);
                        break;
                    case OP_4ADDUI:
                        multiplication64bit(*reg_Y, 4, &result1, &result2);
                        addition64bit(result1, Z, reg_X);
                        break;
                    case OP_8ADDU:      // times 8 and add unsigned
                        multiplication64bit(*reg_Y, 8, &result1, &result2);
                        addition64bit(result1, *reg_Z, reg_X);
                        break;
                    case OP_8ADDUI:
                        multiplication64bit(*reg_Y, 8, &result1, &result2);
                        addition64bit(result1, Z, reg_X);
                        break;
                    case OP_16ADDU:     // times 16 and add unsigned
                        multiplication64bit(*reg_Y, 16, &result1, &result2);
                        addition64bit(result1, *reg_Z, reg_X);
                        break;
                    case OP_16ADDUI:
                        multiplication64bit(*reg_Y, 16, &result1, &result2);
                        addition64bit(result1, Z, reg_X);
                        break;

                    case OP_CMP:            // compare s($X)<-[s($Y) > s($Z)] - [s($Y) < s($Z)]
                                                        // 1 oops
                        *reg_X = (s(*reg_Y) > s(*reg_Z)) - (s(*reg_Y) < s(*reg_Z));
                        break;
                    case OP_CMPI:
                        *reg_X = (s(*reg_Y) > s(Z)) - (s(*reg_Y) < s(Z));
                        break;
                    case OP_CMPU:       // s($X)<-[u($Y) > u($Z)] - [u($Y) < u($Z)]
                                                        // 1 oops
                        *reg_X = (*reg_Y > *reg_Z) - (*reg_Y < *reg_Z);
                        break;
                    case OP_CMPUI:
                        *reg_X = (*reg_Y > Z) - (*reg_Y < Z);
                        break;

                    case OP_NEG:            // negate s($X)<-Y-s($Z)
                                                        // 1 oops
                        *reg_X = Y - s(*reg_Z);
                        break;
                    case OP_NEGI:
                        *reg_X = Y - s(Z);
                        break;
                    case OP_NEGU:       // u($X)<-(Y-u($Z))mod 2^64
                                                        // 1 oops
                        if(Y - *reg_Z >= 0)
                            *reg_X = Y - *reg_Z;
                        else
                            *reg_X = (Y - *reg_Z) & 0x7FFFFFFF;
                        break;
                    case OP_NEGUI:
                        if(Y - Z >= 0)
                            *reg_X = Y - Z;
                        else
                            *reg_X = (Y - Z) & 0x7FFFFFFF;
                        break;

                    case OP_SL:             // shift left s($X)<-s($Y)x2^(u($Z))
                                                        // 1 oops
                        *reg_X = s(*reg_Y) << *reg_Z;
                        break;
                    case OP_SLI:
                        *reg_X = s(*reg_Y) << Z;
                        break;
                    case OP_SLU:            // s($X)<-u($Y)x2^(u($Z)) mod 2^64
                                                        // 1 oops
                        *reg_X = *reg_Y << *reg_Z;
                        break;
                    case OP_SLUI:
                        *reg_X = *reg_Y << Z;
                        break;

                    case OP_SR:             // shift right s($X)<-floor(s($Y)/2^u($Z))
                                                        // 1 oops
                        *reg_X = s(*reg_Y) >> *reg_Z;
                        break;
                    case OP_SRI:
                        *reg_X = s(*reg_Y) >> Z;
                        break;
                    case OP_SRU:            // u($X)<-floor(u($Y)/2^u($Z))
                                                        // 1 oops
                        *reg_X = *reg_Y >> *reg_Z;
                        break;
                    case OP_SRUI:
                        *reg_X = *reg_Y >> Z;
                        break;

                    case OP_BN:             // branch if negative: if s($X) < 0, set @<-RA
                                                        // 3 oops if branch is taken, 1 oops if not
                        break;
                    case OP_BNB:
                        break;
                    case OP_BZ:             // branch if zero: if $X, set @<-RA
                                                        // 3 oops if branch is taken, 1 oops if not
                        break;
                    case OP_BZB:
                        break;
                    case OP_BP:             // branch if positive: if s($X)>0, set@<-RA
                                                        // 3 oops if branch is taken, 1 oops if not
                        break;
                    case OP_BPB:
                        break;
                    case OP_BOD:            // branch if odd: if s($X) mod 2 = 1, set @<-RA
                                                        // 3 oops if branch is taken, 1 oops if not
                        break;
                    case OP_BODB:
                        break;
                    case OP_BNN:            // branch if nonnegative: if s($X)>=0, set @<-RA
                                                        // 3 oops if branch is taken, 1 oops if not
                        break;
                    case OP_BNNB:
                        break;
                    case OP_BNZ:            // branch if nonzero: if $X!=0, set@<-RA
                                                        // 3 oops if branch is taken, 1 oops if not
                        break;
                    case OP_BNZB:
                        break;
                    case OP_BNP:            // branch if nonpositive: if s($X)<=0, set @<-RA
                                                        // 3 oops if branch is taken, 1 oops if not
                        break;
                    case OP_BNPB:
                        break;
                    case OP_BEV:            // branch if even: if s($X) mod 2 = 0, set @<-RA
                                                        // 3 oops if branch is taken, 1 oops if not
                        break;
                    case OP_BEVB:
                        break;
                    

                    case OP_PBN:            // probable branch if negative: if s($X)<0, set @<-RA
                                                        // 3 oops if wrong, 1 oops if right
                        break;
                    case OP_PBNB:
                        break;
                    case OP_PBZ:            // probable branch if zero: if $X=0, set @<-RA
                                                        // 3 oops if wrong, 1 oops if right
                        break;
                    case OP_PBZB:
                        break;
                    case OP_PBP:            // probable branch if positive: if s($X) > 0, set @<-RA
                                                        // 3 oops if wrong, 1 oops if right
                        break;
                    case OP_PBPB:
                        break;
                    case OP_PBOD:       // probable branch if odd: if s($X) mod 2 = 1, set @<-RA
                                                        // 3 oops if wrong, 1 oops if right
                        break;
                    case OP_PBODB:
                        break;
                    case OP_PBNN:       // probable branch if nonnegative: if s($X) >= 0, set @<-RA
                                                        // 3 oops if wrong, 1 oops if right
                        break;
                    case OP_PBNNB:
                        break;
                    case OP_PBNZ:       // probable if nonzero: if $X != 0, set @<-RA
                                                        // 3 oops if wrong, 1 oops if right
                        break;
                    case OP_PBNZB:
                        break;
                    case OP_PBNP:       // probable branch if nonpositive: if s($X) <= 0, set @<-RA
                                                        // 3 oops if wrong, 1 oops if right
                        break;
                    case OP_PBNPB:
                        break;
                    case OP_PBEV:       // probable branch if even: if s($X) mod 2 = 0, set @<-RA
                                                        // 3 oops if wrong, 1 oops if right
                        break;
                    case OP_PBEVB:
                        break;
                    

                    case OP_CSN:            // conditional set if negative: if s($Y) < 0, set $X<-$Z
                                                        // 1 oops
                        break;
                    case OP_CSNI:
                        break;
                    case OP_CSZ:            // conditional set if zero: if $Y = 0, set $X<-$Z
                                                        // 1 oops
                        break;
                    case OP_CSZI:
                        break;
                    case OP_CSP:            // conditional set if positive: if s($Y) > 0, set $X<-$Z
                                                        // 1 oops
                        break;
                    case OP_CSPI:
                        break;
                    case OP_CSOD:       // conditional set if odd: if s($Y) mod 2 = 1, set $X<-$Z
                                                        // 1 oops
                        break;
                    case OP_CSODI:
                        break;
                    case OP_CSNN:       // conditional set if nonnegative: if s($Y) >= 0, set $X<-$Z
                                                        // 1 oops
                        break;
                    case OP_CSNNI:
                        break;
                    case OP_CSNZ:       // conditional set if nonzero: if $Y != 0, set $X<-$Z
                                                        // 1 oops
                        break;
                    case OP_CSNZI:
                        break;
                    case OP_CSNP:       // conditional set if nonpositive: if s($Y) <= 0, set $X<-$Z
                                                        // 1 oops
                        break;
                    case OP_CSNPI:
                        break;
                    case OP_CSEV:       // conditional set if even: if s($Y) mod 2 = 0, set $X<-$Z
                                                        // 1 oops
                        break;
                    case OP_CSEVI:
                        break;
                    

                    case OP_ZSN:            // zero or set if negative: $X<-$Z[s($Y) < 0]
                                                        // 1 oops
                        break;
                    case OP_ZSNI:
                        break; 
                    case OP_ZSZ:            // zero or set if zero: $X<-$Z[$Y = 0]
                                                        // 1 oops
                        break;
                    case OP_ZSZI:
                        break;
                    case OP_ZSP:            // zero or set if positive: $X<-$Z[s($Y) > 0]
                                                        // 1 oops
                        break;
                    case OP_ZSPI:
                        break;
                    case OP_ZSOD:       // zero or set if odd: $X<-$Z[s($Y) mod 2 = 1]
                                                        // 1 oops
                        break;
                    case OP_ZSODI:
                        break;
                    case OP_ZSNN:       // zero or set if nonnegative: $X<-$Z[s($Y) >= 0]
                                                        // 1 oops
                        break;
                    case OP_ZSNNI:
                        break;
                    case OP_ZSNZ:       // zero or set if nonzero: $X<-$Z[$Y!=0]
                                                        // 1 oops
                        break;
                    case OP_ZSNZI:
                        break;
                    case OP_ZSNP:       // zero or set if nonpositive: $X<-$Z[s($Y) <= 0]
                                                        // 1 oops
                        break;
                    case OP_ZSNPI:
                        break;
                    case OP_ZSEV:       // zero or set if even: $X<-$Z[s($Y) mod 2 = 0]
                                                        // 1 oops
                        break;
                    case OP_ZSEVI:
                        break;
                    

                    case OP_LDB:            // load byte s($X)<-s(M1[A])
                                                        // 1 mem + 1 oops
                        addition64bit(*reg_Y, *reg_Z, &A);
                        *reg_X = s(getByteFromMem(vm, A));
                        break;
                    case OP_LDBI: 
                        addition64bit(*reg_Y, Z, &A);
                        *reg_X = s(getByteFromMem(vm, A));
                        break;
                    case OP_LDBU: 
                        addition64bit(*reg_Y, *reg_Z, &A);
                        *reg_X = getByteFromMem(vm, A);
                        break;
                    case OP_LDBUI:
                        addition64bit(*reg_Y, Z, &A);
                        *reg_X = getByteFromMem(vm, A);
                        break;
                    case OP_LDW:            // load wyde s($X)<-s(M2[A])
                                                        // 1 mem + 1 oops
                        addition64bit(*reg_Y, *reg_Z, &A);
                        *reg_X = s(getWydeFromMem(vm, A));
                        break;
                    case OP_LDWI:
                        addition64bit(*reg_Y, Z, &A);
                        *reg_X = s(getWydeFromMem(vm, A));
                        break;
                    case OP_LDWU:
                        addition64bit(*reg_Y, *reg_Z, &A);
                        *reg_X = getWydeFromMem(vm, A);
                        break;
                    case OP_LDWUI:
                        addition64bit(*reg_Y, Z, &A);
                        *reg_X = getWydeFromMem(vm, A);
                        break;
                    case OP_LDT:            // load tetra s($X)<-s(M4[A])
                                                        // 1 mem + 1 oops
                        addition64bit(*reg_Y, *reg_Z, &A);
                        *reg_X = s(getTetraFromMem(vm, A));
                        break;
                    case OP_LDTI:
                        addition64bit(*reg_Y, Z, &A);
                        *reg_X = s(getTetraFromMem(vm, A));
                        break;
                    case OP_LDTU:
                        addition64bit(*reg_Y, *reg_Z, &A);
                        *reg_X = getTetraFromMem(vm, A);
                        break;
                    case OP_LDTUI:
                        addition64bit(*reg_Y, Z, &A);
                        *reg_X = getTetraFromMem(vm, A);
                        break;
                    case OP_LDO:            // load octa s($X)<-s(M8[A])
                                                        // 1 mem + 1 oops
                        addition64bit(*reg_Y, *reg_Z, &A);
                        *reg_X = s(getOctaFromMem(vm, A));
                        break;
                    case OP_LDOI:
                        addition64bit(*reg_Y, Z, &A);
                        *reg_X = s(getOctaFromMem(vm, A));
                        break;
                    case OP_LDOU:
                        addition64bit(*reg_Y, *reg_Z, &A);
                        *reg_X = getOctaFromMem(vm, A);
                        break;
                    case OP_LDOUI:
                        addition64bit(*reg_Y, Z, &A);
                        *reg_X = getOctaFromMem(vm, A);
                        break;
                    case OP_LDSF:       // load short float: f($X)<-f(M4[A]_
                                                        // 1 mem + 1 oops
                        break;
                    case OP_LDSFI:
                        break;
                    case OP_LDHT:       // load high tetra
                                                        // 1 mem + 1 oops
                    case OP_LDHTI:
                        if(byte % 2 == 1)
                            addition64bit(*reg_Y, *reg_Z, &A);
                        else
                            addition64bit(*reg_Y, Z, &A);
                        
                        *reg_X = (Octa)getTetraFromMem(vm, A) << 32;
                        break;
                    

                    case OP_CSWAP:      // compare and swap octabytes: if u(M8[A] = u(rP), where rP is the special prediction reigster,
                                                        // set u(M8[A]<-u($X) and u($X)<-1. Otherwise set u(rP)<-u(M8[A]) and u($X)<-0.
                                                        // This is an atomic operation, useful when independent computers share a common memory.
                                                        // 2 mem + 2 oops
                        break;
                    case OP_CSWAPI:
                        break;
                    case OP_LDUNC:      // load octa unchached: s($X)<-s(M8[A]).
                                                        // 1 mem + 1 oops
                        break;
                    case OP_LDUNCI:
                        break;
                    case OP_LDVTS:      // load virtual trasnlation status: for the operating system only.
                                                        // 1 oops
                        break;
                    case OP_LDVTSI:
                        break;
                    case OP_PRELD:      // preload data: says that all of the bytes M[A] through M[A+X] will probably be loaded or stored in the near future.
                                                        // 1 oops
                        break;
                    case OP_PRELDI:
                        break;
                    case OP_PREGO:      // prefetch to go: says that many of the bytes M[A] through M[A+X] will probably be used as instructions in the near future.
                                                        // 1 oops
                        break;
                    case OP_PREGOI:
                        break;
                    

                    case OP_GO:             // go: u($X)<-@+4, then @<-a
                                                        // 3 oops
                        break;
                    case OP_GOI:
                        break;
                    

                    case OP_STB:            // store byte s(M1[A])<-s($X)
                                                        // 1 mem + 1 oops
                        addition64bit(*reg_Y, *reg_Z, &A);
                        addByteToMem(vm, A, s(*reg_X));
                        break;
                    case OP_STBI:
                        addition64bit(*reg_Y, Z, &A);
                        addByteToMem(vm, A, s(*reg_X));
                        break;
                    case OP_STBU:       // u(M1[A])<-u($X) mod 2^8
                                                        // 1 mem + 1 oops
                        addition64bit(*reg_Y, *reg_Z, &A);
                        addByteToMem(vm, A, *reg_X);
                        break;
                    case OP_STBUI:
                        addition64bit(*reg_Y, Z, &A);
                        addByteToMem(vm, A, *reg_X);
                        break;
                    case OP_STW:            // store wyde s(M2[A])<-s($X)
                                                        // 1 mem + 1 oops
                        addition64bit(*reg_Y, *reg_Z, &A);
                        addWydeToMem(vm, A, s(*reg_X));
                        break;
                    case OP_STWI:
                        addition64bit(*reg_Y, Z, &A);
                        addWydeToMem(vm, A, s(*reg_X));
                        break;
                    case OP_STWU:       // u(M2[A])<-u($X) mod 2^16
                                                        // 1 mem + 1 oops
                        addition64bit(*reg_Y, *reg_Z, &A);
                        addWydeToMem(vm, A, *reg_X);
                        break;
                    case OP_STWUI:
                        addition64bit(*reg_Y, Z, &A);
                        addWydeToMem(vm, A, *reg_X);
                        break;
                    case OP_STT:            // store tetra s(M4[A])<-s($X)
                                                        // 1 mem + 1 oops
                        addition64bit(*reg_Y, *reg_Z, &A);
                        addTetraToMem(vm, A, s(*reg_X));
                        break;
                    case OP_STTI:
                        addition64bit(*reg_Y, Z, &A);
                        addTetraToMem(vm, A, s(*reg_X));
                        break;
                    case OP_STTU:           // u(M4[A])<-u($X) mod 2^32
                                                        // 1 mem + 1 oops
                        addition64bit(*reg_Y, *reg_Z, &A);
                        addTetraToMem(vm, A, *reg_X);
                        break;
                    case OP_STTUI:
                        addition64bit(*reg_Y, Z, &A);
                        addTetraToMem(vm, A, *reg_X);
                        break;
                    case OP_STO:            // store octo s(M8[A])<-s($X)
                                                        // 1 mem + 1 oops
                        addition64bit(*reg_Y, *reg_Z, &A);
                        addOctaToMem(vm, A, s(*reg_X));
                        break;
                    case OP_STOI:
                        addition64bit(*reg_Y, Z, &A);
                        addOctaToMem(vm, A, s(*reg_X));
                        break;
                    case OP_STOU:       // u(M8[A])<-u($X) mod 2^64
                                                        // 1 mem + 1 oops
                        addition64bit(*reg_Y, *reg_Z, &A);
                        addOctaToMem(vm, A, *reg_X);
                        break;
                    case OP_STOUI:
                        addition64bit(*reg_Y, *reg_Z, &A);
                        addOctaToMem(vm, A, *reg_X);
                        break;
                    case OP_STSF:           // store short float: f(M4[A])<-f($X)
                                                            // 1 mem + 1 oops
                        break;
                    case OP_STSFI:
                        break;
                    case OP_STHT:
                    case OP_STHTI:
                        if(byte % 2 == 1)
                            addition64bit(*reg_Y, *reg_Z, &A);
                        else
                            addition64bit(*reg_Y, Z, &A);
                        
                        addTetraToMem(vm, A, *reg_X >> 32);
                        break;
                    case OP_STCO:       // store constant octabyte u(M8[A])<-X
                                                        // 1 mem + 1 oops
                    case OP_STCOI:
                        if(byte % 2 == 1)
                            addition64bit(*reg_Y, *reg_Z, &A);
                        else
                            addition64bit(*reg_Y, Z, &A);
    
                        addOctaToMem(vm, A, X);
                        break;
                    case OP_STUNC:  // store octa uncached: s(M8[A)<-s($X)
                                                    // 1 mem + 1 oops
                        break;
                    case OP_STUNCI:
                        break;
                    

                    case OP_SYNCD:  // synchronize data: says that all of the bytes M[A] through M[A+X] must be brought up to date in the physical memory, so that other computers and input/output devices can read them.
                                                    // 1 oops
                        break;
                    case OP_SYNCDI:
                        break;
                    case OP_PREST:  // prestore data: says that all of the bytes M[A] through M[A+X] will definitely be written(stored) before they are next read( loaded).
                                                    // 1 oops
                        break;
                    case OP_PRESTI:
                        break;
                    

                    case OP_SYNCID: // synchronize instructions and data: says that all of the bytes M[A] through M[A+X] must be fetched again before being interpreted as instructions.
                                                    // 1 oops
                        break;
                    case OP_SYNCIDI:
                        break;
                    

                    case OP_PUSHGO: // push registers and go: push(X) and set rJ<-@ + 4, then set @<-A
                                                    // 3 oops
                        break;
                    case OP_PUSHGOI:
                        break;


/* SET */           case OP_OR:         // bitwise or: v($X)<-v($Y) | v($Z)
/* $X $Y */                             // 1 oops
/* <=> */
/* OR $X $Y 0 */
                        break;
                    case OP_ORI:
                        break;


                    case OP_ORN:        // bitwise or-not: v($X)<-v($Y) | !v($Z)
                                                    // 1 oops
                        break;
                    case OP_ORNI:
                        break;


                    case OP_NOR:        // bitwise not-or: !v($X)<-v($Y) | v($Z)
                                                    // 1 oops
                        break;
                    case OP_NORI:
                        break;


                    case OP_XOR:        // bitwise exclusive-or: v($X)<-v($Y) ^ v($Z)
                                                    // 1 oops
                        break;
                    case OP_XORI:
                        break;


                    case OP_AND:        // bitwise and: v($X)<-v($Y) & v($Z)
                                                    // 1 oops
                        break;
                    case OP_ANDI:
                        break;
                    case OP_ANDN:   // bitwise and-not: v($X)<-v($Y) & !v($Z)
                                                    // 1 oops
                        break;
                    case OP_ANDNI:
                        break;
                    case OP_NAND:   // bitwise not-and: !v($X)<-v($Y) & v($Z)
                                                    // 1 oops
                        break;
                    case OP_NANDI:
                        break;


                    case OP_NXOR:   // bitwise not-exclusive-or: !v($X)<-v($Y) ^ v($Z)
                                                    // 1 oops
                        break;
                    case OP_NXORI:
                        break;
                        // y .-. z = max(0, y - z) (saturating subtraction)


                    case OP_BDIF:   // byte difference: b($X)<-b($Y) .-. b($Z)
                                                    // 1 oops
                        break;
                    case OP_BDIFI:
                        break;
                    case OP_WDIF:   // wyde difference: w($X)<-w($Y) .-. w($Z)
                                                    // 1 oops
                        break;
                    case OP_WDIFI:
                        break;
                    case OP_TDIF:   // tetra difference: t($X)<-t($Y) .-. t($Z)
                                                    // 1 oops
                        break;
                    case OP_TDIFI:
                        break;
                    case OP_ODIF:   // octa difference: u($X)<-u($Y) .-. u($Z)
                                                    // 1 oops
                        break;
                    case OP_ODIFI:
                        break;


                    case OP_MUX:        // bitwise multiplex (combines two bit vectors by looking at the special multiples mask regiser rM: v($X)<-(v($Y) & v(rM)) | (v($Z) & !v(rM))
                                                    // 1 oops
                        break;
                    case OP_MUXI:
                        break;
                    case OP_SADD:   // sideways add(counts the number of bit positions in which register $Y has a 1 while register $Z has a 0: s($X)<-s(sum(v($Y) & !v($Z)))
                                                    // 1 oops
                        break;
                    case OP_SADDI:
                        break;
                    case OP_MOR:        // multiple or: m^T($X)<-m^T($Y) |(on matrices) m^T($Z) <=> m($X)<-m($Z) |(on matrices) m($Y)
                                                    // 1 oops
                        break;
                    case OP_MORI:
                        break;
                    case OP_MXOR:   // multipe exclusive-or (same as MOR but with  ^(on matrices)
                                                    // 1 oops
                        break;
                    case OP_MXORI:
                        break;


                    case OP_SETH:   // set high wyde: u($X)<-YZ x 2^48
                                                    // 1 oops
                        break;
                    case OP_SETMH:  // set medium high wyde: u($X)<-YZ x 2^32
                                                    // 1 oops
                        break;
                    case OP_SETML:  // set medium low wide: u($X)<-YZ x 2^16
                                                    // 1 oops
                        break;
/*SET*/             case OP_SETL:   // set low wyde: u($X)<-YZ
                                                    // 1 oops
                        break;

                    case OP_INCH:   // increase by high wyde: u($X)<-(u($X) + YZ) mod 2^64
                                                    // 1 oops
                        break;
                    case OP_INCHMH: // increase by medium high wyde: u($X)<-(u($X) + YZ x 2^32) mod 2^64 
                                                    // 1 oops
                        break;
                    case OP_INCML:  // increase by medium low wyde: u($X)<-(u($X) + YZ x 2^16) mod 2^64
                                                    // 1 oops
                        break;
                    case OP_INCL:   // increase by low wyde: u($X)<-(u($X) + YZ) mod 2^64
                                                    // 1 oops
                        break;


                    case OP_ORH:        // bitwise or with high wyde: v($X)<-v($X)|v(YZ<<48)
                                                    // 1 oops
                        break;
                    case OP_ORMH:   // bitwise or with medium high wyde: v($X)<-v($X)|v(YZ<<32)
                                                    // 1 oops
                        break;
                    case OP_ORML:   // bitwise or with medium low wyde: v($X)<-v($X)|v(YZ<<16)
                                                    // 1 oops
                        break;
                    case OP_ORL:        // bitwise or with low wyde: v($X)<-v($X)|v(YZ)
                                                    // 1 oops
                        break;

                    case OP_ANDH:   // bitwise and-not high wyde: v($X)<-v($X)&!v(YZ<<48)
                                                    // 1 oops
                        break;
                    case OP_ANDNMH: // bitwise and-not medium high wyde: v($X)<-v($X)&!v(YZ<<32)
                                                    // 1 oops
                        break;
                    case OP_ANDML:  // bitwise and-not medium low wyde: v($X)<-v($X)&!v(YZ<<16)
                                                    // 1 oops
                        break;
                    case OP_ANDNL:  // bitwise and-not low wyde: v($X)<-v($X)&!v(YZ)
                                                    // 1 oops
                        break;

                    case OP_JMP:        // jump: @<-RA (a three byte relative address) <=> @+4xXYZ (can also be negative)
                                                    // 1 oops
                        break;
                    case OP_JMPB:
                        break;
    
                    case OP_PUSHJ:  // push registers and jump: push(X) and set rJ<-@ + 4, then set @<-RA
                                                    // 1 oops
                        break;
                    case OP_PUSHJB:
                        break;

                    case OP_GETA:   // get address: u($X)<-RA
                                                    // 1 oops
                        break;
                    case OP_GETAB:
                        break;

                    case OP_PUT:        // put into special register: u(g[X])<-u($Z), where 0 <= X < 32
                                                    // 1 oops
                        break;
                    case OP_PUTI:
                        break;

                    case OP_POP:        // pop registers and return: pop(X), then @<-rJ + 4xYZ
                                                    // 3 oops
                        break;
                    case OP_RESUME: // resume after interrupt
                                                    // 5 oops
                        break;
                    case OP_SAVE:   // save process state: u($X)<-context
                                                    // 20 mems + 1 oops
                        break;
                    case OP_UNSAVE: // resstore process state: context<-u($Z)
                                                    // 20 mems + 1 oops
                        break;
                    case OP_SYNC:   // synchronize: restricts parallel activities so that different processors can cooperate reliably;
                                                    // 1 oops
                        break;
                    case OP_SWYM:   // symphatize with your machinery: nop
                                                    // 1 oops 
                        break;
                    case OP_GET:        // get from special register: u($X)<-u(g[Z]), where 0 <= Z < 32
                                                    // 1 oops
                        break;
                    case OP_TRIP:   //this command forces a trip to the handler at location #00
                                                    // 5 oops
                        break;
        }
    }
}
