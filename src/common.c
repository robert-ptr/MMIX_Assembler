#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

Instruction instructions[256] = 
{
	{	.name = "TRAP", 	.arity = { 3, 2, 1}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 1, 1},
							{ 1, 1, 1},
							{ 1, 1, 1}}},
	{	.name = "FCMP", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FUN", 		.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FEQL", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FADD", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FIX", 		.arity = { 3, 2, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FSUB", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FIXU", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FLOT", 	.arity = { 3, 2, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FLOTI", 	.arity = { 3, 2, 0}, .has_alt_ver = false, // ? is this correct
		.is_constant = { 	{ 0, 1, 1},
							{ 0, 1, 0},
							{ 0, 0, 0}}},
	{	.name = "FLOTU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FLOTUI", .arity = { 3, 2, 0}, .has_alt_ver = false, // ?
		.is_constant = { 	{ 0, 1, 1},
							{ 0, 1, 0},
							{ 0, 0, 0}}},
	{	.name = "SFLOT", 	.arity = { 3, 2, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SFLOTI", .arity = { 3, 2, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 1},
							{ 0, 1, 0},
							{ 0, 0, 0}}},
	{	.name = "SFLOTU", .arity = { 3, 2, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SFLOTUI",.arity = { 3, 2, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 1},
							{ 0, 1, 0},
							{ 0, 0, 0}}},
	{	.name = "FMUL", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FCMPE", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FUNE", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FEQLE", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FDIV", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FSQRT", 	.arity = { 3, 2, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FREM",   .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "FINT",   .arity = { 3, 2, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "MUL", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "MULI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "MULU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "MULUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "DIV", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "DIVI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "DIVU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "DIVUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ADD", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ADDI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ADDU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ADDUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SUB", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SUBI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SUBU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SUBUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "2ADDU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "2ADDUI", .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "4ADDU",  .arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "4ADDUI", .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "8ADDU",  .arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "8ADDUI", .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{ .name = "16ADDU", .arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
						    { 0, 0, 0}}},
	{	.name = "16ADDUI",.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CMP", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CMPI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CMPU",   .arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CMPUI",  .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "NEG", 	  .arity = { 3, 2, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "NEGI",   .arity = { 3, 2, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 1},
							{ 0, 1, 0},
							{ 0, 0, 0}}},
	{	.name = "NEGU", 	.arity = { 3, 2, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "NEGUI", 	.arity = { 3, 2, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 1},
							{ 0, 1, 0},
							{ 0, 0, 0}}},
	{	.name = "SL", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SLI", 		.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SLU", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SLUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SR", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SRI", 		.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
						    { 0, 0, 0}}},
	{	.name = "SRU", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SRUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BN", 		.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BNB", 		.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BZ", 		.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BZB", 		.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BP", 		.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BPB", 		.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BOD", 		.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BODB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BNN", 		.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
						    { 0, 0, 0}}},
	{	.name = "BNNB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BNZ", 		.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BNZB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BNP", 		.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BNPB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BEV", 		.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BEVB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBN", 		.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBNB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBZ", 		.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBZB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBP", 		.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBPB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBOD", 	.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBODB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBNN", 	.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBNNB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBNZ", 	.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBNZB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBNP", 	.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBNPB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBEV", 	.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PBEVB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSN", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSNI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSZ", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSZI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSP", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSPI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSOD", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSODI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSNN", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSNNI",  .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSNZ", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSNZI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSNP", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
						    { 0, 0, 0}}},
	{	.name = "CSNPI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{ .name = "CSEV", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSEVI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSN", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSNI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSZ", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSZI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSP", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSPI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSOD", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSODI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSNN", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSNNI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSNZ", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSNZI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSNP", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSNPI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSEV", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ZSEVI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDB", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDBI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDBU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDBUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDW", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDWI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDWU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDWUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDT", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDTI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDTU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDTUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDO", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDOI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDOU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDOUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDSF", 	.arity = { 3, 2, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 1, 0},
							{ 0, 0, 0}}},
	{	.name = "LDSFI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDHT", 	.arity = { 3, 2, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 1, 0},
							{ 0, 0, 0}}},
	{	.name = "LDHTI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSWAP", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "CSWAPI", .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDUNC", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDUNCI", .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDVTS", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "LDVTSI", .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PRELD", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 1, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PRELDI", .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PREGO", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 1, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PREGOI", .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "GO", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "GOI", 		.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STB", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STBI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STBU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STBUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STW", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STWI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STWU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STWUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STT", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STTI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STTU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
						    { 0, 0, 0}}},
	{	.name = "STTUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STO", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STOI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{ .name = "STOU", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STOUI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STSF", 	.arity = { 3, 2, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 1, 0},
							{ 0, 0, 0}}},
	{	.name = "STSFI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STHT", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STHTI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STCO", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 1, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STCOI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STUNC", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "STUNCI", .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SYNCD",  .arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 1, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SYNCDI", .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PREST",  .arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 1, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PRESTI", .arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SYNCID", .arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 1, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SYNCIDI",.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PUSHGO", .arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PUSHGOI",.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "OR", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ORI", 		.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ORN", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ORNI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "NOR", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "NORI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "XOR", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "XORI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "AND", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ANDI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ANDN", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
			    			{ 0, 0, 0}}},
	{	.name = "ANDNI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "NAND", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "NANDI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "NXOR", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "NXORI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BDIF", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "BDIFI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "WDIF", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "WDIFI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
		    				{ 0, 0, 0}}},
	{	.name = "TDIF", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "TDIFI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ODIF", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ODIFI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "MUX", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "MUXI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SADD", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SADDI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "MOR", 		.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "MORI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "MXOR", 	.arity = { 3, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "MXORI", 	.arity = { 3, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 1},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SETH", 	.arity = { 2, 0 ,0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SETMH", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SETML", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
				    		{ 0, 0, 0}}},
	{	.name = "SETL", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "INCH", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "INCMH", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
			    			{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "INCML", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
			    			{ 0, 0, 0}}},
	{   .name = "INCL", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ORH", 		.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ORMH", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ORML", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
		    				{ 0, 0, 0}}},
	{	.name = "ORL", 		.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ANDNH", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ANDNMH", .arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ANDNML", .arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "ANDNL", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "JMP", 		.arity = { 1, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 1, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "JMPB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PUSHJ", 	.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PUSHJB", .arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
			    			{ 0, 0, 0}}},
	{	.name = "GETA", 	.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "GETAB", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PUT", 		.arity = { 2, 0, 0}, .has_alt_ver = true,
		.is_constant = { 	{ 1, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "PUTI", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "POP", 		.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "RESUME", .arity = { 1, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SAVE", 	.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}}, // ?
	{	.name = "UNSAVE", .arity = { 1, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SYNC", 	.arity = { 1, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 0, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "SWYM", 	.arity = {3, 2, 1}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 1, 1},
							{ 1, 1, 0},
							{ 1, 0, 0}}}, // NOP
	{	.name = "GET", 		.arity = { 2, 0, 0}, .has_alt_ver = false,
		.is_constant = { 	{ 0, 1, 0},
							{ 0, 0, 0},
							{ 0, 0, 0}}},
	{	.name = "TRIP", 	.arity = {3, 2, 1}, .has_alt_ver = false,
		.is_constant = { 	{ 1, 1, 1},
							{ 1, 1, 0},
							{ 1, 0, 0}}}
};

static const char to_lowercase[] = {
    ['A'] = 'a', ['B'] = 'b', ['C'] = 'c', ['D'] = 'd', ['E'] = 'e', ['F'] = 'f', 
    ['G'] = 'g', ['H'] = 'h', ['I'] = 'i', ['J'] = 'j', ['K'] = 'k', ['L'] = 'l', 
    ['M'] = 'm', ['N'] = 'n', ['O'] = 'o', ['P'] = 'p', ['Q'] = 'q', ['R'] = 'r',
    ['S'] = 's', ['T'] = 't', ['U'] = 'u', ['V'] = 'v', ['W'] = 'w', ['X'] = 'x',
    ['Y'] = 'y', ['Z'] = 'z',
    ['a'] = 'a', ['b'] = 'b', ['c'] = 'c', ['d'] = 'd', ['e'] = 'e', ['f'] = 'f', 
    ['g'] = 'g', ['h'] = 'h', ['i'] = 'i', ['j'] = 'j', ['k'] = 'k', ['l'] = 'l', 
    ['m'] = 'm', ['n'] = 'n', ['o'] = 'o', ['p'] = 'p', ['q'] = 'q', ['r'] = 'r',
    ['s'] = 's', ['t'] = 't', ['u'] = 'u', ['v'] = 'v', ['w'] = 'w', ['x'] = 'x',
    ['y'] = 'y', ['z'] = 'z',
    ['0'] = '0', ['1'] = '1', ['2'] = '2', ['3'] = '3', ['4'] = '4', ['5'] = '5', ['6'] = '6', ['7'] = '7', ['8'] = '8', ['9'] = '9'
};

char* getString(char* buffer, int32_t length, int32_t buf_index)
{
	if(length <= 1)
		return NULL;
	
	int32_t i = 0;
	char* new_string = (char*)malloc(length * sizeof(char));
	new_string[length - 1] = '\0';
	for(; (i + 1 - length) != 0; i++)
	{
		new_string[i]	= buffer[buf_index - length + 1 + i];	
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

void charToLowercase(char* c)
{
    *c = to_lowercase[*c];
}

void stringToLowercase(char** s)
{
	for(int32_t i = 0; i < (*s)[i] != '\0'; i++)
	{
		charToLowercase(&(*s)[i]);
	}
}

char* intToHexString(uint64_t n)
{
	uint64_t c = n;
	uint64_t size = 0;
	while(c > 0)
	{
		c /= 16;
		size++;
	}

	char* str = (char*)malloc(19 * sizeof(char));
	str[0] = '0';
	str[1] = 'x';
	for(int i = 2; i < 18; i++)
	{
		str[i] = '0';
	}
	
	for(int i = 17; i > 1; i--, n /= 16)
	{
		c = n % 16;
		switch(c)
		{
			case 0:
				str[i] = '0';
				break;
			case 1:
				str[i] = '1';
				break;
			case 2:
				str[i] = '2';
				break;
			case 3:
				str[i] = '3';
				break;
			case 4:
				str[i] = '4';
				break;
			case 5:
				str[i] = '5';
				break;
			case 6:
				str[i] = '6';
				break;
			case 7:
				str[i] = '7';
				break;
			case 8:
				str[i] = '8';
				break;
			case 9:
				str[i] = '9';
				break;
			case 10:
				str[i] = 'A';
				break;
			case 11:
				str[i] = 'B';
				break;
			case 12:
				str[i] = 'C';
				break;
			case 13:
				str[i] = 'D';
				break;
			case 14:
				str[i] = 'E';
				break;
			case 15:
				str[i] = 'F';
				break;
		}
	}

	return str;
}

char* intToBinaryString(uint64_t n)
{
	char* str = (char*)malloc(65 * sizeof(char));

	for(int i = 0; i < 64; i++)
	{
		str[i] = '0';
	}

	for(int i = 63; i >= 0 && n > 0; i--)
	{
		if(n & 1)
			str[i] = '1';
		else
			str[i] = '0';

		n >>= 1;
	}

    return str;
}

uint64_t fromHexadecimal(const char* str)
{
	uint64_t result = 0;
	int32_t n = strlen(str);
	char* new_str = (char*)malloc((n + 1) * sizeof(char));
	stringToLowercase(&new_str);	
	for(int i = 0; i < n; i++)
	{
		switch(new_str[i])
		{
			case '0':
				result *= 16;
				break;
			case '1':
				result = result * 16 + 1;
				break;
			case '2':
				result = result * 16 + 2;
				break;
			case '3':
				result = result * 16 + 3;
				break;
			case '4':
				result = result * 16 + 4;
				break;
			case '5':
				result = result * 16 + 5;
				break;
			case '6':
				result = result * 16 + 6;
				break;
			case '7':
				result = result * 16 + 7;
				break;
			case '8':
				result = result * 16 + 8;
				break;
			case '9':
				result = result * 16 + 9;
				break;
			case 'a':
				result = result * 16 + 10;
				break;
			case 'b':
				result = result * 16 + 11;
				break;
			case 'c':
				result = result * 16 + 12;
				break;
			case 'd':
				result = result * 16 + 13;
				break;
			case 'e':
				result = result * 16 + 14;
				break;
			case 'f':
				result = result * 16 + 15;
				break;
			default:
				fprintf(stderr, "Invalid hex digit!");
				break;
		}
	}

	free(new_str);
}
