#ifndef MMIX_COMMON
#define MMIX_COMMON

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    char* name;
    uint16_t description; // 2 bits are for arity, first bit marks whether the instruction accepts 3 arguments
                         // the second marks whether the instruction accepts 2 arguments 
                         // the third bit marks whether there is a version of the instruction with immediates 
                         // the next 3 bits mark which arguments can be immediates for the version with 3 arguments 
                         // the next 2 bits mark which arguments can be immedaites for the version with 2 arguments 
                         // if the instruction accepts one single argument, that is handled separately in the parser 
                         // since there are few such instructions and this compromise allows us to use 8 bits instead of 10 + 6 for padding
                         // and also makes the instruction table more easily readable
                         // + 1 byte for the opcode
} Instruction;

extern Instruction instructions[258]; 
char* get_string(char* buffer, int32_t length, int32_t buf_index);
char* read_file(const char* path);
FILE* create_file(const char* path);

void string_to_lowercase(char* s);
char* int_to_hex_string(uint64_t n, uint8_t bits);
char* int_to_binary_string(uint64_t n, uint8_t bits);
int64_t parse_hex_number(const char* str);
int32_t parse_number(char* buffer);

#endif
