void createTrie(TrieNode** node, char** words, int n)
{
	for(int i = 0; i < n; i++)
	{
		int j = 0;
		TrieNode new_node;
		TrieNode** old_node = node;
		while(words[i][j] != '\0')
		{
			char c = words[i][j];
			if((*node)->nodes[c - 'a'] == NULL)
			{
					(*node)->nodes[c - 'a'] = &new_node;
			}
			*node = &new_node;
			j++;	
		}

		(*node)->isWord = true;
		node = old_node;
	}
}

bool findWord(TrieNode* node, char* word)
{
	for(int i = 0; word[i] != '\0'; i++)
	{
		if(node->nodes[word[i]])
			return false;
		node = node->nodes[word[i]];
	}

	return true;
}
