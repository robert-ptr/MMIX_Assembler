#include <stdbool.h>

#define ALPHABET_SIZE 36

typedef struct TrieNode TrieNode;

struct TrieNode
{
	TrieNode* nodes[ALPHABET_SIZE];
	bool isWord;
};

TrieNode* getNode(void);
void insertNode(TrieNode* node, char* word);
void createTrie(TrieNode* node, char** words, int n);
bool findWord(TrieNode* node, char* word);
