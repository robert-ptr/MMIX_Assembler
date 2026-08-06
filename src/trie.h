#ifndef MMIX_TRIE
#define MMIX_TRIE

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
    bool is_word;
};

TrieNode* get_node(void);
void free_trie(TrieNode* root);
void insert_node(TrieNode* node, char* word, bool value);
void create_trie(TrieNode* node, char** words, int32_t n);
bool find_word(TrieNode* node, char* word);
uint8_t char_to_index(char c);
char index_to_char(uint8_t i);

#endif
