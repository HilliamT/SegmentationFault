#ifndef ARM11_13_TRIE_H
#define ARM11_13_TRIE_H

#include "../../util/map.h"

typedef struct TrieNode
{
    char *word;
    Map *children;
} TrieNode;

TrieNode *init_trie_node();
TrieNode *get_child(TrieNode *node, char *key);
void insert_word(TrieNode *node, char *word);
void insert_word_file(TrieNode *node, char *file_path);
TrieNode *build_word_bank();

#endif //ARM11_13_TRIE_H
