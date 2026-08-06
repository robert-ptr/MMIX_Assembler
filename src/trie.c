#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

TrieNode* get_node(void)
{
    TrieNode* new_node = (TrieNode*)malloc(sizeof(TrieNode));
    new_node->is_word = false;

    for(int32_t i = 0; i < ALPHABET_SIZE; i++)
        new_node->nodes[i] = NULL;
    new_node->index = 0; 
    new_node->parent = NULL;
    
    return new_node;
}

void free_trie(TrieNode* root)
{
    for(int i = 0; i < ALPHABET_SIZE; i++)
    {
        if(root->nodes[i] != NULL)
        {
            free_trie(root->nodes[i]);
        }
    }

    free(root);
}

uint8_t char_to_index(const char c) // the alphabet is 'a...z0...9'
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

char index_to_char(uint8_t i)
{
    if(i >= ALPHABET_SIZE)
    {
        printf("In trie: index_to_char - out of bounds error\n");
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

void insert_node(TrieNode* node, char* word, bool value)
{
        TrieNode* copy = node;
        int32_t n = strlen(word);
        
        for(int32_t i = 0; i < n; i++)
        {
            if(char_to_index(word[i]) == -1)
                return;
        }

        for(int32_t i = 0; i < n; i++)
        {
            int32_t index = char_to_index(word[i]);
            if(copy->nodes[index] == NULL)
            {
                copy->nodes[index] = get_node();
                copy->nodes[index]->index = index; // too much index for my taste
                copy->nodes[index]->parent = copy;
            }
            copy = copy->nodes[index];
        }

        copy->is_word = value; 
}

void create_trie(TrieNode* node, char** words, int32_t n)
{
    for(int32_t i = 0; i < n; i++)
    {
        insert_node(node, words[i], true);
    }
}

bool find_word(TrieNode* node, char* word)
{
    for(int32_t i = 0; word[i] != '\0'; i++)
    {
        int32_t index = char_to_index(word[i]);
        if(index == -1)
            return false;
        if(node->nodes[index] == NULL)
            return false;
        node = node->nodes[index];
    }

    return node->is_word;
}
