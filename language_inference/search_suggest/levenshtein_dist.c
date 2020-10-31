#include "levenshtein_dist.h"
#include "trie.h"
#include <string.h>
#include <stdlib.h>
#include "../../util/linked_list.h"
#include <stdio.h>

/**
 * This algorithm is based off the Levenshtein Distance metric, the relationship between two
 * words as the number of single characters that need to be changed for one word to be the
 * same as the other.
 *
 * LEVENSHTEIN DISTANCE:
 *
 * "programming" is 1 LD from "pnogramming" [r -> n]
 * "Will" is 2 LDs away from "mile" [W -> m, l -> e]
 *
 * This is incredibly helpful for catching typos or offering search suggestions from a bank
 * of technical words.
 *
 * HOW TO USE:
 *
 * 1 function:
 * get_closest_query(SENTENCE QUERY, MAX LEVENSHTEIN DISTANCE, MIN WORD SIZE TO PERFORM LD ALGO ON)
 *
 * FOR EXAMPLE:
 *
 * int main() {
 *  printf("BEST QUERY: %s\n", get_closest_query("yes segmentution fuult", 3, 5));
 * }
 *
 * Will print
 * "BEST QUERY:  yes segmentation fault"
 */

/**
 * Used to hold contextual information for Levenshtein Distance algorithm calculations
 */
typedef struct SearchContext
{
  int *current_row;
  char *word;
  int *max_cost;
  LinkedList *results;
} SearchContext;

SearchContext *init_search_context(int *current_row, char *word, int max_cost, LinkedList *results)
{
  SearchContext *searchContext = malloc(sizeof(SearchContext));
  searchContext->current_row = current_row;
  searchContext->word = word;

  searchContext->max_cost = malloc(sizeof(int));
  *(searchContext->max_cost) = max_cost;

  searchContext->results = results;
  return searchContext;
}

typedef struct LevenPair
{
  char *word;
  int *ldist;
} LevenPair;

LevenPair *init_leven_pair(char *word, int ldist)
{
  LevenPair *levenPair = malloc(sizeof(LevenPair));
  levenPair->word = strdup(word);
  levenPair->ldist = malloc(sizeof(int));
  *(levenPair->ldist) = ldist;
  return levenPair;
}

LevenPair *join_leven_pairs(LevenPair *pair1, LevenPair *pair2)
{
  char *sentence = malloc((strlen(pair1->word) + strlen(pair2->word) + 2) * sizeof(char));
  sentence[0] = '\0';
  if (strlen(pair1->word) != 0)
  {
    strcat(sentence, pair1->word);
    strcat(sentence, " ");
  }
  strcat(sentence, pair2->word);
  LevenPair *result_pair = init_leven_pair(sentence, *(pair1->ldist) + *(pair2->ldist));
  free(sentence);

  return result_pair;
}

int min_int_arr(int *arr, int size)
{
  int min = 0;
  for (int i = 0; i < size; i++)
  {
    if (min > arr[i])
      min = arr[i];
  }
  return min;
}

void search_recursive(char *key, TrieNode *node, SearchContext *context)
{
  int *previous_row = context->current_row;
  char *word = context->word;
  int max_cost = *context->max_cost;
  LinkedList *results = context->results;

  int columns = strlen(word) + 1;
  int *current_row = calloc(columns, sizeof(int));
  current_row[0] = previous_row[0] + 1;

  for (int col = 1; col < columns; col++)
  {

    int insert_cost = current_row[col - 1] + 1;
    int delete_cost = previous_row[col - 1] + 1;
    int replace_cost = previous_row[col - 1];
    replace_cost += (word[col - 1] != key[0]) ? 1 : 0;

    int min = (insert_cost < delete_cost) ? insert_cost : delete_cost;

    current_row[col] = (min < replace_cost) ? min : replace_cost;
  }

  if (current_row[columns - 1] <= max_cost && (node->word != 0))
  {
    linked_list_append(results, init_leven_pair(node->word, current_row[columns - 1]));
  }

  SearchContext *new_context = init_search_context(current_row, word, max_cost, results);

  if (min_int_arr(current_row, columns) <= max_cost)
  {
    map_iterate(node->children, (void (*)(char *, void *, void *)) & search_recursive, new_context);
  }
}

LinkedList *find_close_words(TrieNode *node, char *word, int max_cost)
{
  int *current_row = calloc(sizeof(int), strlen(word) + 1);
  for (int i = 0; i < (strlen(word) + 1); i++)
    current_row[i] = i;

  LinkedList *results = linked_list_make();

  SearchContext *context = init_search_context(current_row, word, max_cost, results);

  map_iterate(node->children, (void (*)(char *, void *, void *)) & search_recursive, context);

  return results;
}

LevenPair *get_closest_word(TrieNode *node, char *word, int max_cost)
{
  LevenPair *best_pair = 0;

  LinkedList *results = find_close_words(node, word, max_cost);

  ListEntry *curr_node = results->head;
  while (curr_node->data != 0)
  {
    LevenPair *pair = (LevenPair *)curr_node->data;

    if (best_pair == 0 || (*best_pair->ldist > *pair->ldist))
    {
      best_pair = pair;
    }

    curr_node = curr_node->next;
  }

  return best_pair;
}

char *get_closest_query(char *sentence, int max_word_cost, int min_word_size)
{
  TrieNode *bank = build_word_bank();

  LevenPair *sentence_pair = init_leven_pair("", 0);

  char *token;
  char *temp_text = strdup(sentence);

  while ((token = strtok_r(temp_text, " ", &temp_text)))
  {

    if (strlen(token) >= min_word_size)
    {
      LevenPair *best_pair = get_closest_word(bank, token, max_word_cost);

      if (best_pair == 0)
      {
        sentence_pair = join_leven_pairs(sentence_pair, init_leven_pair(token, 0));
      }
      else
      {
        sentence_pair = join_leven_pairs(sentence_pair, best_pair);
      }
    }
    else
    {
      sentence_pair = join_leven_pairs(sentence_pair, init_leven_pair(token, 0));
    }
  }

  return sentence_pair->word;
}
