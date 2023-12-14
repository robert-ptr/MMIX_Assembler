#include <stdlib.h>
#include <stdbool.h>

#define ALPHABET_SIZE 26

typedef struct
{
	TrieNode* nodes[ALPHABET_SIZE];
	bool isWord;
} TrieNode;

void createTrie(TrieNode** node, char** words, int n);

bool findWord(TrieNode* node, char* word);
