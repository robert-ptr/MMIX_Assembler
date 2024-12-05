#ifndef MMIX_PROJ_TRIE
#define MMIX_PROJ_TRIE

#include <stdbool.h>
#include <stdint.h>
#include "common.h"

#define ALPHABET_SIZE 36

typedef struct TrieNode TrieNode;

struct TrieNode
{
    uint8_t index;
	TrieNode* nodes[ALPHABET_SIZE];
	TrieNode* parent;
    bool isWord;
};

TrieNode* getNode(void);
void freeTrie(TrieNode* root);
void insertNode(TrieNode* node, char* word, bool value);
void createTrie(TrieNode* node, char** words, int32_t n);
bool findWord(TrieNode* node, char* word);
uint8_t charToIndex(char c);
char indexToChar(uint8_t i);

#endif
