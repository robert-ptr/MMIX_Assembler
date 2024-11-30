#ifndef MMIX_PROJ_TRIE
#define MMIX_PROJ_TRIE

#include <stdbool.h>
#include <stdint.h>
#include "common.h"

#define ALPHABET_SIZE 36

typedef struct TrieNode TrieNode;

struct TrieNode
{
	TrieNode* nodes[ALPHABET_SIZE];
	bool isWord;
};

TrieNode* getNode(void);
void freeTrie(TrieNode* root);
void insertNode(TrieNode* node, char* word, bool value);
void createTrie(TrieNode* node, char** words, int32_t n);
int32_t findWord(TrieNode* node, char* word);

#endif
