#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

TrieNode* getNode(void)
{
	TrieNode* new_node = (TrieNode*)malloc(sizeof(TrieNode));
	new_node->isWord = -1;

	for(int32_t i = 0; i < ALPHABET_SIZE; i++)
		new_node->nodes[i] = NULL;

	return new_node;
}

static int32_t alphabetIndex(char c)
{
	if(c >= '0' && c <= '9')
		return 26 + (c - '0');
	else if(c >= 'a' && c <= 'z')
		return c - 'a';
	else
		return -1;
}

void insertNode(TrieNode* node, char* word, int32_t value)
{
		TrieNode* copy = node;
		int32_t n = strlen(word);
		for(int32_t i = 0; i < n; i++)
		{
			if(alphabetIndex(word[i]) == -1)
				return;
		}
		for(int32_t i = 0; i < n; i++)
		{
			int32_t index = alphabetIndex(word[i]);
			if(copy->nodes[index] == NULL)
			{
				copy->nodes[index] = getNode();
			}
			copy = copy->nodes[index];
			
		}

		copy->isWord = value; // true is -2, false is -1

}

void createTrie(TrieNode* node, char** words, int32_t n)
{
	for(int32_t i = 0; i < n; i++)
	{
		insertNode(node, words[i], -2);
	}
}

int32_t parseNumber(char* buffer, int32_t* index)
{
	int32_t number = 0;
	while(buffer[*index] != '\0' && buffer[*index] != '\n')
	{
		number = number* 10 + (buffer[*index] - '0');
		(*index)++;
	}

	return number;
}

void createInstructionTrie(TrieNode* node)
{
	for(int32_t i = 0; i < 256; i++)
	{
		insertNode(node, instr[i].name, instr[i].op_code);
	}
}

int32_t findWord(TrieNode* node, char* word)
{
	for(int32_t i = 0; word[i] != '\0'; i++)
	{
		int32_t index = alphabetIndex(word[i]);
		if(index == -1)
			return -1;
		if(node->nodes[index] == NULL)
			return -1;
		node = node->nodes[index];
	}

	return node->isWord;
}
