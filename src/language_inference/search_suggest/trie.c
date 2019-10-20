#include "trie.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define MAX_WORD_LEN 20

TrieNode* init_trie_node() {
  TrieNode* trie_node = malloc(sizeof(TrieNode));
  trie_node->word = 0;
  trie_node->children = map_make();
  return trie_node;
}

TrieNode* get_child(TrieNode* node, char* key) {
  return map_get(node->children, key);
}

void set_word(TrieNode* node, char* word) {
  char* word_space = malloc(sizeof(char) * (MAX_WORD_LEN + 1));
  word_space[0] = '\0';
  strcat(word_space, word);
  node->word = word_space;

}

void insert_word(TrieNode* node, char* word) {
  TrieNode* curr_node = node;

  for (int i = 0; i < strlen(word); i++) {

    // stringified our character
    char letter[2] = { 0 };
    letter[0] = word[i];


    if (map_get(curr_node->children, letter) == 0) {
      map_add(curr_node->children, letter, init_trie_node());
    }

    curr_node = get_child(curr_node, letter);
  }

  set_word(curr_node, word);
}

void insert_word_file(TrieNode* node, char* file_path) {
  FILE* file_ptr = fopen(file_path, "r");

  char* line = malloc(sizeof(char) * (MAX_WORD_LEN + 1));

  while (fgets(line, 65537, file_ptr) != NULL) {
    line = strtok(line, "\n");

    insert_word(node, line);
  }
  free(line);
  fclose(file_ptr);
}

/**
 * Prepends "../languages/" to file_name to access language file
 */
static char* get_language_path(char* file_name) {
  char* lang_dur = "./language_inference/languages/";
  char* lang_path = malloc(sizeof(char) * (strlen(file_name) + strlen(lang_dur) + 1));
  lang_path[0] = '\0';
  strcat(lang_path, lang_dur);
  strcat(lang_path, file_name);
  return lang_path;
}

/**
 * Read all ../languages/<files> into our Trie Bank
 */
TrieNode* build_word_bank() {
  TrieNode* trie_bank = init_trie_node();

  DIR* d;
  struct dirent* dir;
  d = opendir("./language_inference/languages/");

  if (d != NULL) {
    while ((dir = readdir(d)) != NULL) {
      // Get each file in the directory, whilst skipping the current (".") and parent ("..").
      if (dir->d_name[0] != '.') {
        // Insert their contents into our trie
        insert_word_file(trie_bank, get_language_path(dir->d_name));
      }
    }
    closedir(d);
  }

  return trie_bank;
}
