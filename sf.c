#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "util/linked_list.h"
#include "question.h"
#include "args.h"
#include "query/query_verif.h"
#include "query/lib-cjson/cJSON.h"
#include "query/json_parser.h"
#include "display/display.h"
#include "language_inference/predict_language/naive_bayes.h"
#include "language_inference/search_suggest/levenshtein_dist.h"

static ListEntry *curr_question_entry = 0;

void refresh_display(int signum)
{
  if (curr_question_entry != 0)
  {
    display_clear();
    display_preview(curr_question_entry);
  }
}

LinkedList *get_questions_list(char *query)
{
  char *json_results = get_stackoverflow_results(query);
  LinkedList *questions = json_parse(json_results);
  free(json_results);
  return questions;
}

int main(int argc, char **argv)
{

  char *query = process_args(argc, argv);

  if (sf_flags.help)
  {
    display_usage();
    exit(EXIT_SUCCESS);
  }

  initialise_naiveb_lang_infer("./language_inference/languages/");

  struct sigaction act = {
      .sa_handler = &refresh_display,
  };
  sigaction(SIGWINCH, &act, NULL);

  // Return a linked list of Questions
  LinkedList *questions_list = get_questions_list(query);

  while (questions_list->head->data == 0)
  {
    // Magic numbers recommended by Hilliam
    char *suggested = get_closest_query(query, 3, 5);

    if (strcmp(suggested, query) != 0)
    {

      printf("No relevant questions found for '%s'. Did you mean: '%s'? [Y/n] ", query, suggested);
      char response[5];
      scanf("%4s", response);
      getchar();
      if (strcmp(response, "y") == 0 || strcmp(response, "yes") == 0 || strcmp(response, "Y") == 0)
      {
        questions_list = get_questions_list(suggested);
        free(query);
        query = suggested;
        continue;
      }
    }
    else
    {
      printf("No relevant questions found. Try a different query.\n");
    }
    return EXIT_SUCCESS;
  }

  free(query);

  curr_question_entry = questions_list->head;

  while (true)
  {
    Question *question = curr_question_entry->data;

    refresh_display(SIGWINCH);

    system("stty raw");
    char command = getchar();
    system("stty cooked");
    switch (command)
    {
    case 'q':
      display_clear();
      exit(EXIT_SUCCESS);
      break;
    case 'h':
      display_help_page();
      break;
    case 'j':
      if (curr_question_entry->next->data != 0)
      {
        curr_question_entry = curr_question_entry->next;
      }
      break;
    case 'k':
      if (curr_question_entry->prev != 0)
      {
        curr_question_entry = curr_question_entry->prev;
      }
      break;
    case 'l':
    case ' ':
    case '\r':
      display_question(question);
      break;
    }
  }

  return 0;
}
