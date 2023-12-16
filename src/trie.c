#include <stdlib.h>
#include <string.h>
#include "trie.h"

TrieNode* getNode(void)
{
	TrieNode* new_node = (TrieNode*)malloc(sizeof(TrieNode));
	new_node->isWord = false;

	for(int i = 0; i < ALPHABET_SIZE; i++)
		new_node->nodes[i] = NULL;

	return new_node;
}

static int alphabetIndex(char c)
{
	if(c >= '0' && c <= '9')
		return 26 + (c - '0');
	else if(c >= 'a' && c <= 'z')
		return c - 'a';
	else
		return -1;
}

void insertNode(TrieNode* node, char* word)
{
		TrieNode* copy = node;
		int n = strlen(word);
		for(int i = 0; i < n; i++)
		{
			if(alphabetIndex(word[i]) == -1)
				return;
		}
		for(int i = 0; i < n; i++)
		{
			int index = alphabetIndex(word[i]);
			if(copy->nodes[index] == NULL)
			{
				copy->nodes[index] = getNode();
			}
			copy = copy->nodes[index];
			
		}

		copy->isWord = true;

}

void createTrie(TrieNode* node, char** words, int n)
{
	for(int i = 0; i < n; i++)
	{
		insertNode(node, words[i]);
	}
}

bool findWord(TrieNode* node, char* word)
{
	for(int i = 0; word[i] != '\0'; i++)
	{
		int index = alphabetIndex(word[i]);
		if(index == -1)
			return false;
		if(node->nodes[index] == NULL)
			return false;
		node = node->nodes[index];
	}
	if(node->isWord)
		return true;

	return false;
}
