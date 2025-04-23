#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

Instruction instructions[258] = 
{
    {   .name = "trap",     .description = 0xDF00, }, // also accepts a single argument that may or may not be an immediate
    {   .name = "fcmp",     .description = 0x8001, },
    {   .name = "fun",      .description = 0x8002, },
    {   .name = "feql",     .description = 0x8003, },
    {   .name = "fadd",     .description = 0x8004, },
    {   .name = "fix",      .description = 0xC805, },
    {   .name = "fsub",     .description = 0x8006, },
    {   .name = "fixu",     .description = 0x8007, },
    {   .name = "flot",     .description = 0xA808, },
    {   .name = "flotu",    .description = 0xA80A, },
    {   .name = "sflot",    .description = 0xE80C, },
    {   .name = "sflotu",   .description = 0xE80E, },
    {   .name = "fmul",     .description = 0x8010, },
    {   .name = "fcmpe",    .description = 0x8011, },
    {   .name = "fune",     .description = 0x8012, },
    {   .name = "feqle",    .description = 0x8013, },
    {   .name = "fdiv",     .description = 0x8014, },
    {   .name = "fsqrt",    .description = 0xD015, },
    {   .name = "frem",     .description = 0x8016, },
    {   .name = "fint",     .description = 0xC017, },
    {   .name = "mul",      .description = 0xA418, },
    {   .name = "mulu",     .description = 0xA41A, },
    {   .name = "div",      .description = 0xA41C, },
    {   .name = "divu",     .description = 0xA41E, },
    {   .name = "add",      .description = 0xA420, },
    {   .name = "addu",     .description = 0xA422, },
    {   .name = "sub",      .description = 0xA424, },
    {   .name = "subu",     .description = 0xA426, },
    {   .name = "2addu",    .description = 0xA428, },
    {   .name = "4addu",    .description = 0xA42A, },
    {   .name = "8addu",    .description = 0xA42C, },
    {   .name = "16addu",   .description = 0xA42E, },
    {   .name = "cmp",      .description = 0xA430, },
    {   .name = "cmpu",     .description = 0xA432, },
    {   .name = "neg",      .description = 0xED34, },
    {   .name = "negu",     .description = 0xED36, },
    {   .name = "sl",       .description = 0xA438, },
    {   .name = "slu",      .description = 0xA43A, },
    {   .name = "sr",       .description = 0xA43C, }, 
    {   .name = "sru",      .description = 0xA43E, },
    {   .name = "bn",       .description = 0x4140, },
    {   .name = "bz",       .description = 0x4142, },
    {   .name = "bp",       .description = 0x4144, },
    {   .name = "bod",      .description = 0x4146, },
    {   .name = "bnn",      .description = 0x4148, },
    {   .name = "bnz",      .description = 0x414A, },
    {   .name = "bnp",      .description = 0x414C, },
    {   .name = "bev",      .description = 0x414E, },
    {   .name = "pbn",      .description = 0x4150, },
    {   .name = "pbz",      .description = 0x4152, },
    {   .name = "pbp",      .description = 0x4154, },
    {   .name = "pbod",     .description = 0x4156, },
    {   .name = "pbnn",     .description = 0x4158, },
    {   .name = "pbnz",     .description = 0x415A, },
    {   .name = "pbnp",     .description = 0x415C, },
    {   .name = "pbev",     .description = 0x415E, },
    {   .name = "csn",      .description = 0xA460, },
    {   .name = "csz",      .description = 0xA462, },
    {   .name = "csp",      .description = 0xA464, },
    {   .name = "csod",     .description = 0xA466, },
    {   .name = "csnn",     .description = 0xA468, },
    {   .name = "csnz",     .description = 0xA46A, },
    {   .name = "csnp",     .description = 0xA46C, },
    {   .name = "csev",     .description = 0xA46E, }, 
    {   .name = "zsn",      .description = 0xA470, },
    {   .name = "zsz",      .description = 0xA472, },   
    {   .name = "zsp",      .description = 0xA474, },
    {   .name = "zsod",     .description = 0xA476, },
    {   .name = "zsnn",     .description = 0xA478, },
    {   .name = "zsnz",     .description = 0xA47A, },
    {   .name = "zsnp",     .description = 0xA47C, },
    {   .name = "zsev",     .description = 0xA47E, },
    {   .name = "ldb",      .description = 0xA480, },
    {   .name = "ldbu",     .description = 0xA482, },
    {   .name = "ldw",      .description = 0xA484, },
    {   .name = "ldwu",     .description = 0xA486, },
    {   .name = "ldt",      .description = 0xA488, },
    {   .name = "ldtu",     .description = 0xA48A, },
    {   .name = "ldo",      .description = 0xA48C, },
    {   .name = "ldou",     .description = 0xA48E, },
    {   .name = "ldsf",     .description = 0xE590, },
    {   .name = "ldht",     .description = 0xE592, },
    {   .name = "cswap",    .description = 0xA494, },
    {   .name = "ldunc",    .description = 0xA496, },
    {   .name = "ldvts",    .description = 0xA498, },
    {   .name = "preld",    .description = 0xB49A, }, 
    {   .name = "prego",    .description = 0xB49C, },
    {   .name = "go",       .description = 0xA49E, },
    {   .name = "stb",      .description = 0xA4A0, },
    {   .name = "stbu",     .description = 0xA4A2, },
    {   .name = "stw",      .description = 0xA4A4, },  
    {   .name = "stwu",     .description = 0xA4A6, },
    {   .name = "stt",      .description = 0xA4A8, },
    {   .name = "sttu",     .description = 0xA4AA, },
    {   .name = "sto",      .description = 0xA4AC, }, 
    {   .name = "stou",     .description = 0xA4AE, },
    {   .name = "stsf",     .description = 0xE5B0, },
    {   .name = "stht",     .description = 0xA4B2, },
    {   .name = "stco",     .description = 0xB4B4, },
    {   .name = "stunc",    .description = 0xA4B6, },
    {   .name = "syncd",    .description = 0xB4B8, },
    {   .name = "prest",    .description = 0xB4BA, },
    {   .name = "syncid",   .description = 0xB4BC, },
    {   .name = "pushgo",   .description = 0xA4BE, },
    {   .name = "or",       .description = 0xA4C0, },
    {   .name = "orn",      .description = 0xA4C2, },
    {   .name = "nor",      .description = 0xA4C4, },
    {   .name = "xor",      .description = 0xA4C6, },
    {   .name = "and",      .description = 0xA4C8, },
    {   .name = "andn",     .description = 0xA4CA, },
    {   .name = "nand",     .description = 0xA4CC, },
    {   .name = "nxor",     .description = 0xA4CE, },
    {   .name = "bdif",     .description = 0xA4D0, },
    {   .name = "wdif",     .description = 0xA4D2, },
    {   .name = "tdif",     .description = 0xA4D4, },
    {   .name = "odif",     .description = 0xA4D6, },
    {   .name = "mux",      .description = 0xA4D8, },
    {   .name = "sadd",     .description = 0xA4DA, },
    {   .name = "mor",      .description = 0xA4DC, },
    {   .name = "mxor",     .description = 0xA4DE, },
    {   .name = "seth",     .description = 0x41E0, },
    {   .name = "setmh",    .description = 0x41E1, },
    {   .name = "setml",    .description = 0x41E2, },
    {   .name = "setl",     .description = 0x41E3, },
    {   .name = "inch",     .description = 0x41E4, },
    {   .name = "incmh",    .description = 0x41E5, },
    {   .name = "incml",    .description = 0x41E6, }, 
    {   .name = "incl",     .description = 0x41E7, },
    {   .name = "orh",      .description = 0x41E8, },
    {   .name = "ormh",     .description = 0x41E9, },
    {   .name = "orml",     .description = 0x41EA, },
    {   .name = "orl",      .description = 0x41EB, },
    {   .name = "andnh",    .description = 0x41EC, },
    {   .name = "andnmh",   .description = 0x41ED, },
    {   .name = "andnml",   .description = 0x41EE, },
    {   .name = "andnl",    .description = 0x41EF, },
    {   .name = "jmp",      .description = 0x00F0, }, // takes an immediate as argument 
    {   .name = "pushj",    .description = 0x41F2, }, 
    {   .name = "geta",     .description = 0x41F4, }, 
    {   .name = "put",      .description = 0x63F6, }, 
    {   .name = "pop",      .description = 0x43F8, }, 
    {   .name = "resume",   .description = 0x00F9, }, // takes an immediate as argument 
    {   .name = "save",     .description = 0x41FA, }, 
    {   .name = "unsave",   .description = 0x00FB, }, // takes a register as an argument  
    {   .name = "sync",     .description = 0x00FC, }, // takes an immediate as argument that must be 0, 1, 2, or 3
    {   .name = "swym",     .description = 0xDFFD, }, 
    {   .name = "get",      .description = 0x41FE, }, 
    {   .name = "trip",     .description = 0xDFFF, }, // same as trap 
    {   .name = "lda",      .description = 0x9A22, }, 
    {   .name = "set",      .description = 0x43E3, }, // SET can be an abbrevation for SETL or for OR 
                                                    // so SET $X, $Y is equivalent to OR $X, $Y, 0
};

int32_t parseNumber(char* buffer)
{
    int32_t number = 0;

    if (sscanf(buffer, "%d", &number) == 1)
        return number;

    return -1;
}

char* getString(char* buffer, int32_t length, int32_t buf_index)
{
    if(length <= 1)
        return NULL;
    
    int32_t i = 0;
    char* new_string = (char*)malloc(length * sizeof(char));
    new_string[length - 1] = '\0';
    for(; (i + 1 - length) != 0; i++)
    {
        new_string[i]   = buffer[buf_index - length + 1 + i];   
    }

    return new_string;
}

char* readFile(const char* path)
{
    FILE* fd = fopen(path, "rb");
    if(fd == NULL)
    {
        fprintf(stderr, "Could not open file \"%s\" for reading.\n", path);
        exit(74);
    }

    fseek(fd, 0L, SEEK_END);
    size_t file_size = ftell(fd);
    rewind(fd);

    char* buffer = (char*)malloc(file_size + 1);
    if(buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, fd);
    if (bytes_read < file_size)
    {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytes_read] = '\0';

    fclose(fd);
    return buffer;
}

FILE* createFile(const char* path)
{
    FILE* fd = fopen(path, "w");
    if (fd == NULL)
    {
        fprintf(stderr, "Could not open file \"%s\" for writing.\n", path);
        exit(74);
    }

    return fd;
}

void stringToLowercase(char* s)
{
    for(int32_t i = 0; s[i] != '\0'; i++)
    {
        s[i] = tolower(s[i]);
    }
}

char* intToHexString(uint64_t n, uint8_t bytes) // bits means the number of used bits in the number, so i dont create a new function for smaller integers
{
    char* str = (char*)malloc(2 * bytes * sizeof(char));

    if (snprintf(str, 2 * bytes + 2, "0x%x", n) != 2 * bytes + 2)
        return NULL;

    return str;
}

char* intToBinaryString(uint64_t n, uint8_t bytes) // bits means the number of used bits in the number, so i dont create a new function for smaller integers
{
    uint8_t len = 8 * bytes;
    char* str = (char*)malloc((len + 1) * sizeof(char));
    str[len] = '\0';

    for(int i = 0; i < len; i++)
    {
        str[i] = '0';
    }

    for(int i = len - 1; i >= 0 && n > 0; i--)
    {
        if(n & 1)
            str[i] = '1';
        else
            str[i] = '0';

        n >>= 1;
    }

    return str;
}

int64_t parseHexNumber(const char* str)
{
    unsigned int number;
    if (sscanf(str, "%x", &number) == 1)
        return number;

    return -1; // failed
}
