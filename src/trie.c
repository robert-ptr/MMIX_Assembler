#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"
#include "common.h"
#include "scanner.h"

TrieNode* getNode(void)
{
	TrieNode* new_node = (TrieNode*)malloc(sizeof(TrieNode));
	new_node->isWord = -1;

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

void insertNode(TrieNode* node, char* word, int value)
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

		copy->isWord = value; // true is -2, false is -1

}

void createTrie(TrieNode* node, char** words, int n)
{
	for(int i = 0; i < n; i++)
	{
		insertNode(node, words[i], -2);
	}
}

int parseNumber(char* buffer, int* index)
{
	int number = 0;
	while(buffer[*index] != '\0' && buffer[*index] != '\n')
	{
		number = number* 10 + (buffer[*index] - '0');
		(*index)++;
	}

	return number;
}

void createInstructionTrie(TrieNode* node)
{
	char* buffer = readFile("instructions2.txt");
	
	int index = 0;
	int n = 0;
	char** words = (char**)malloc(256 * sizeof(char*));
	int* values = (int*)malloc(256 * sizeof(int));

	while(buffer[index] != '\0')
	{
		int start = index;
		while(buffer[index] != ' ')
		{
			index++;
		}

		int length = index - start;
		char* word = (char*)malloc((length + 1) * sizeof(char));

		for(int i = 0; i < length; i++)
		{
			word[i] = buffer[start + i];
		}

		word[length] = '\0';
		toLowercase(&word);
		words[n] = word;
		index++;

		values[n++] = parseNumber(buffer, &index);

		if(buffer[index] == '\n')
			index++;
	}

	for(int i = 0; i < n; i++)
	{
		insertNode(node, words[i], values[i]);
	}
}

int findWord(TrieNode* node, char* word)
{
	for(int i = 0; word[i] != '\0'; i++)
	{
		int index = alphabetIndex(word[i]);
		if(index == -1)
			return -1;
		if(node->nodes[index] == NULL)
			return -1;
		node = node->nodes[index];
	}

	return node->isWord;
}

/* for testing
int main()
{
	TrieNode* root = getNode();
	createInstructionTrie(root);
	printf("%d", findWord(root, "trap"));
}
*/
