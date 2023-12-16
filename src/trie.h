#include <stdbool.h>

#define ALPHABET_SIZE 26

typedef struct TrieNode TrieNode;

struct TrieNode 
{
	TrieNode* nodes[ALPHABET_SIZE];
	bool isWord;
};

void createTrie(TrieNode** node, char** words, int n);

bool findWord(TrieNode* node, char* word);
