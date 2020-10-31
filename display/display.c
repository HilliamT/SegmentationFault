#include "display.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "formatter.h"
#include "file_utils.h"
#include "string_utils.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <assert.h>

int get_term_width() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_col;
}

int get_term_height() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_row;
}

/**
 * Performs 'clear' command in terminal to wipe contents of terminal
 */
void display_clear(void) {
  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    execl("/usr/bin/env", "/usr/bin/env", "clear", NULL);
  }
  wait(NULL);
}

void display_preview(ListEntry *curr_question_entry) {
  int max_lines = get_term_height() - 1;
  int lines_printed = 0;
  char *page = calloc(1, 1);
  int page_size = 1;
  int term_width = get_term_width();
  while (curr_question_entry->data != 0 && lines_printed < max_lines) {
    int new_lines;
    int lines_rem = max_lines - lines_printed;
    char *formatted = format_preview(curr_question_entry->data, 
                                     lines_rem > 15 ? 15 : lines_rem, &new_lines, term_width);
    page_size += strlen(formatted);
    page = realloc(page, page_size);
    strcat(page, formatted);
    free(formatted);
    lines_printed += new_lines;
    curr_question_entry = curr_question_entry->next;
  }
  char *first_body_line = strichr(page, '\n', 3, NULL) + 1;
  first_body_line[0] = '=';
  first_body_line[1] = '>';
  printf("%s", page);
  free(page);
}

/**
 * Displays the contents of a file, using less
 */
void display_file(const char *filename, const char *prompt) {
  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    execl("/usr/bin/env", "/usr/bin/env", "less", "-c", "--prompt=", prompt, "-RS", filename, NULL);
  }
  int wstatus;
  wait(&wstatus);
}

/**
 * Write a string to a file
 */
void write_body_to_file(FILE *fp, char *body) {
  char *formatted_body = format_body(body, get_term_width());
  fwrite(formatted_body, 1, strlen(formatted_body), fp);
  free(formatted_body);
}

/**
 * Displays a question, and its accompanying answers, to the terminal
 */
void display_question(Question *question) {
  assert(question != 0);
  // Create a temporary buffer file for syntax highlighting and HTML rendering
  char *filename;
  FILE *fp = create_tmp(&filename);
  char *prompt;

  // format and write question to buffer file, currently considering language highlighting in C.
  char *formatted_question = format_question(question, &prompt, get_term_width());
  fwrite(formatted_question, 1, strlen(formatted_question), fp);
  free(formatted_question);

  // Go through each of the answers, and format this answer
  LinkedList *answers = question->answers;
  ListEntry *curr_entry = answers->head;
  while (curr_entry->data != 0) {
    Answer *answer = curr_entry->data;
    assert(answer != 0);

    // Write formatted answer to buffer file
    char *formatted_answer = format_answer(answer, get_term_width());
    fwrite(formatted_answer, 1, strlen(formatted_answer), fp);
    free(formatted_answer);

    ListEntry *next_entry = curr_entry->next;
    curr_entry = next_entry;
  }
  fclose(fp);

  // display the contents of the buffer file
  display_file(filename, prompt);
  free(prompt);
  close_tmp(filename);
}

void display_usage(void) {
  printf("\n          segmentation" ESCAPE_BOLD_BEGIN "fault" ESCAPE_BOLD_END "\n\n");
  printf("Usage: sf [OPTION...] QUERY\n");
  printf("Search for QUERY on Stack Overflow.\n");
  printf("Example: sf C segmentation fault\n");
  printf("Key bindings:\n");
  printf("  Use j to scroll downwards and k upwards.\n");
  printf("  q to return to the previous level.\n");
  printf("  h in program for more information.\n");
  printf("Options:\n");
  printf("  --help Prints out this page\n");
}

void display_help_page(void) {
  display_clear();
  printf("Segmentation Fault Commands\n\n");
  printf("h                           Display this page.\n");
  printf("j                           Select the next question.\n");
  printf("k                           Select the previous question.\n");
  printf("l / ENTER / SPACE           View the selected question thread.\n");
  printf("q                           Go back to the last level / Quits this program.\n");
  printf("\nPress any button to quit this page.\n");
  system("stty raw");
  getchar();
  system("stty cooked");
  return;
}

