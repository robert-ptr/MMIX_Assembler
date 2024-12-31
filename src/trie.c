#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

TrieNode* getNode(void)
{
	TrieNode* new_node = (TrieNode*)malloc(sizeof(TrieNode));
	new_node->isWord = false;

	for(int32_t i = 0; i < ALPHABET_SIZE; i++)
		new_node->nodes[i] = NULL;
    new_node->index = 0; 
    new_node->parent = NULL;
	
    return new_node;
}

void freeTrie(TrieNode* root)
{
    for(int i = 0; i < ALPHABET_SIZE; i++)
    {
        if(root->nodes[i] != NULL)
        {
            freeTrie(root->nodes[i]);
        }
    }

    free(root);
}

uint8_t charToIndex(const char c) // the alphabet is 'a...z0...9'
{
	if(c >= '0' && c <= '9')
		return 26 + (c - '0');
	else if(c >= 'a' && c <= 'z')
		return c - 'a';
    else if(c >= 'A' && c <= 'Z')
        return c - 'A';
	else
		return -1;
}

char indexToChar(uint8_t i)
{
    if(i >= ALPHABET_SIZE)
    {
        printf("In trie: indexToChar - out of bounds error\n");
        return '\0';
    }
    if(i >= 26)
    {
        return ('0' + (i - 26));
    }
    else
    {
        return ('a' + i);
    }
}

void insertNode(TrieNode* node, char* word, bool value)
{
		TrieNode* copy = node;
		int32_t n = strlen(word);
		
		for(int32_t i = 0; i < n; i++)
		{
			if(charToIndex(word[i]) == -1)
				return;
		}

		for(int32_t i = 0; i < n; i++)
		{
			int32_t index = charToIndex(word[i]);
			if(copy->nodes[index] == NULL)
			{
				copy->nodes[index] = getNode();
                copy->nodes[index]->index = index; // too much index for my taste
                copy->nodes[index]->parent = copy;
			}
			copy = copy->nodes[index];
		}

		copy->isWord = value; 
}

void createTrie(TrieNode* node, char** words, int32_t n)
{
	for(int32_t i = 0; i < n; i++)
	{
		insertNode(node, words[i], true);
	}
}

bool findWord(TrieNode* node, char* word)
{
	for(int32_t i = 0; word[i] != '\0'; i++)
	{
		int32_t index = charToIndex(word[i]);
		if(index == -1)
			return false;
		if(node->nodes[index] == NULL)
			return false;
		node = node->nodes[index];
	}

	return node->isWord;
}
