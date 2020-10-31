#include "naive_bayes.h"
#include <dirent.h>
#include "../../util/map.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include "large_double/large_double.h"
#include <stdbool.h>

/**
 * HOW TO USE:
 *
 * ===FIRST===:
 * initialise_naiveb_lang_infer(<RELATIVE PATH TO LANGUAGE FOLDER FROM FILE WITH MAIN>);
 *
 * ===THEN===:
 * predict_language(<BODY TEXT>);
 */

#define MAX_WORD_LEN 30
#define CMP_PERCENT_NEEDED 0.05

// Static global variables needed as to minimise file opening and reading operations through caching.
static int no_of_languages = 0;
static char **languages = {0};
static Map *file_lengths = {0};
static int total_samples = 0;
static char *path_to_languages = 0;

/**
 * Set global variables needed for our Naive Bayes inference
 */
void initialise_naiveb_lang_infer(char *relative_path_to_languages)
{
  path_to_languages = strdup(relative_path_to_languages);
  languages = cache_languages();
  file_lengths = cache_file_lengths(languages, no_of_languages);
}

/**
 * Returns the number of languages known
 */
int get_number_of_languages()
{
  return no_of_languages;
}

/**
 * Returns list of known languages
 */
char **get_known_languages()
{
  return languages;
}

/**
 * Returns the number of words within a language's bank of words
 */
int get_lang_sample_size(char *lang_file)
{
  return *((int *)map_get(file_lengths, lang_file));
}

/**
 * Update the number of total banked words
 */
void add_more_samples(int no_of_new_samples)
{
  total_samples += no_of_new_samples;
}

/**
 * Return the list of known language files
 */
char **get_file_names()
{
  return languages;
}

/**
 * Return the number of total banked words
 */
int get_total_samples()
{
  return total_samples;
}

/**
 * Set the value of (GLOBAL) no_of_languages, and returns a list of languages now known
 */
char **cache_languages()
{
  char **list_of_languages = 0;
  int languages = 0;

  // Open a pointer to the languages directory
  DIR *d;
  struct dirent *dir;
  d = opendir(path_to_languages);

  if (d != NULL)
  {
    while ((dir = readdir(d)) != NULL)
    {
      // Get each file in the directory, whilst skipping the current (".") and parent ("..").
      if (dir->d_name[0] != '.')
      {
        // Add it to our list of language text files we can search through
        list_of_languages = realloc(list_of_languages, sizeof(char *) * (languages + 1));
        list_of_languages[languages] = strdup(dir->d_name);
        languages++;
      }
    }
    closedir(d);
  }

  no_of_languages = languages;
  return list_of_languages;
}

/**
 * Prepends path_to_languages to file_name to access language file
 */
static char *get_language_path(char *file_name)
{
  char *lang_path = malloc(sizeof(char) * (strlen(file_name) + strlen(path_to_languages) + 1));
  lang_path[0] = '\0';
  strcat(lang_path, path_to_languages);
  strcat(lang_path, file_name);
  return lang_path;
}

/**
 * Returns the number of lines (words) in a file
 */
int get_file_length(char *file_name)
{

  // Get the relative path to the file and open
  char *lang_path = get_language_path(file_name);

  FILE *file_ptr = fopen(lang_path, "r");
  if (file_ptr == NULL)
  {
    printf("File not found. It is likely that the relative path is not correct.");
    exit(0);
  }

  // Read the number of lines within the file
  char *line = malloc(sizeof(char) * (MAX_WORD_LEN + 1));
  int no_of_lines = 0;

  while (fgets(line, MAX_WORD_LEN + 1, file_ptr) != NULL)
    no_of_lines++;

  fclose(file_ptr);
  free(lang_path);

  free(line);

  return no_of_lines;
}

/**
 * Creates a map for holding the length of each language bank file
 */
Map *cache_file_lengths(char **file_names, int num_of_languages)
{

  // Create a Hashmap containing linking file names to their lengths
  Map *file_to_length = map_make();

  for (int i = 0; i < num_of_languages; i++)
  {
    int *file_length_ptr = malloc(sizeof(int));

    // Get the number of words / lines within it and save it to map
    char *next_file = file_names[i];
    *file_length_ptr = get_file_length(next_file);
    map_add(file_to_length, next_file, file_length_ptr);

    // Update the total number of banked words in any file
    add_more_samples(*file_length_ptr);
  }

  return file_to_length;
}

/**
 * From <language>.txt, return just <language>
 */
char *get_language_from_file_name(char *language_file)
{
  char *temp_name = strdup(language_file);
  char *lang_name = strdup(strtok(temp_name, "."));
  free(temp_name);

  return lang_name;
}

/**
 * Turns a character into its lower case value, if appropriate
 */
char to_lowercase(char c)
{
  if (c > 'z' || c < 'a')
    return c;
  return tolower(c);
}

/**
 * Turns all letters in a string to lowercase
 */
char *all_lower(char *string)
{
  char *new_str = strdup(string);
  for (int i = 0; i < strlen(string); i++)
  {
    new_str[i] = to_lowercase(string[i]);
  }
  return new_str;
}

/**
 * Returns whether a word is in a file
 */
int get_word_count_in_file(char *word, char *language_file)
{

  // Get the relative path
  char *lang_path = get_language_path(language_file);
  FILE *file_ptr = fopen(lang_path, "r");
  if (file_ptr == NULL)
  {
    printf("File could not be obtained. It is likely that the relative path is incorrect.");
    exit(0);
  }

  // Make our search word lowercase
  char *lower_case_word = all_lower(word);

  // For each line in a file
  char *line = malloc(sizeof(char) * 65537);
  int found_word = 0;
  while (fgets(line, 65536, file_ptr) != NULL && !found_word)
  {

    // Check to see if the line (which is just 1 word) matches our search term
    line = strtok(line, "\n");
    if (!strcmp(line, lower_case_word))
      found_word = 1;
  }

  free(line);
  free(lower_case_word);
  fclose(file_ptr);
  free(lang_path);
  return found_word;
}

/**
 * Returns P(Word | Lang), the probability a word belongs to a language
 * This is a proportion of the word, to the entire size of the words for a language known
 */
double get_prob_word_given_lang(char *word, char *language_file)
{
  return ((double)get_word_count_in_file(word, language_file)) / get_lang_sample_size(language_file);
}

/**
 * Returns P(Text | Lang), the probability a piece of text belongs to a language
 * This is done by P(Text | Lang) = P(Word1 | Lang) * P(Word2 | Lang) * ...
 * with assumption that each Word is independent to each other (hence, our approach
 * is Naive).
 */
LargeDouble get_prob_text_given_lang(const char *text, char *language_file)
{

  LargeDouble total_prob = init_large_double(1.0);
  bool no_words_seen = true;

  char *save_ptr;
  char *temp_text = strdup(text);
  char *token = strtok_r(temp_text, " \n.(),:", &save_ptr);

  // For each full word in a piece of text
  for (; token != NULL; token = strtok_r(NULL, " \n.(),:", &save_ptr))
  {

    // Find whether it is within the asked language's bank
    double word_count = get_word_count_in_file(token, language_file);

    // Find the probability of P(Word | Lang), with smoothing
    double prob_word_given_lang = (word_count + 1) / get_lang_sample_size(language_file);

    // P(Text | Lang) = P(Word1 | Lang) * P(Word2 | Lang) * ...
    total_prob = mult_large_double(total_prob, prob_word_given_lang);

    // If a word does pop up in the language's bank, there is some chance it could be of that
    // language
    if (double_gt_zero(word_count))
      no_words_seen = false;
  }

  free(temp_text);
  // Otherwise, rule it out with a probability of zero.
  return (no_words_seen) ? init_large_double(0) : total_prob;
}

/**
 * Return P(Lang)
 */
double get_prob_lang(char *language_file)
{
  return ((double)get_lang_sample_size(language_file)) / get_total_samples();
}

/**
 * Find the probability of a text being of a certain programming language, with assumption
 * that P(text) is constant for all P(Lang | Text) calculations.
 * This is done by P(Lang | Text) = P(Lang) * P(Text | Lang) / P(Text) [Bayes's Theorem]
 */
LargeDouble get_prob_lang_given_text(const char *text, char *language_file)
{
  double prob_lang = get_prob_lang(language_file);
  LargeDouble prob_text_given_lang = get_prob_text_given_lang(text, language_file);

  return mult_large_double(prob_text_given_lang, prob_lang);
}

/**
 * Using Bayes's Theorem, find the most appropriate programming language a text is likely
 * to be most related to.
 */
char *predict_language(const char *text)
{
  // We make an initial guess of not being a language.
  LargeDouble best_prob = init_large_double(0);
  char *best_lang_file = "N/A";

  // For each programming language we know,
  for (int i = 0; i < get_number_of_languages(); i++)
  {

    // Get the likelihood the text is mostly referencing this language
    char *language_file = get_known_languages()[i];
    LargeDouble next_prob = get_prob_lang_given_text(text, language_file);

    // Considering a noise factor to overcome, we calculate the difference
    LargeDouble diff = subtract_large_double(next_prob, best_prob);
    LargeDouble neededDiff = mult_large_double(best_prob, CMP_PERCENT_NEEDED);

    // And if our difference is substantial, we update the best language
    if (compare_large_doubles(diff, neededDiff) == 1)
    {
      best_prob = next_prob;
      best_lang_file = language_file;
    }
  }

  return (!strcmp(best_lang_file, "N/A")) ? strdup("N/A") : get_language_from_file_name(best_lang_file);
}
