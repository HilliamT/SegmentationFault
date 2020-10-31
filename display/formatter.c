#include "formatter.h"
#include "file_utils.h"
#include "string_utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include "../language_inference/predict_language/naive_bayes.h"

/**
 * Constants used for specifying display properties
 */
enum FormatterConsts {
  MAX_COMMAND_LEN = 100,
  MAX_LANG_STR_LEN = 10,
  MAX_LINE_LEN = 2000,
  READ_BLOCK_UNIT = 1000,
};

/**
 * Rendering HTML tags with Lynx
 */
char *remove_html_tags(const char *para, int width) {
  assert(para != 0);

  // Run Lynx command with HTML string
  char *filename = write_to_tmp(para);
  char command[MAX_COMMAND_LEN + 1];
  sprintf(command, "/usr/bin/env lynx -dump -width=%i -force_html %s", width, filename);
  FILE *fp = popen(command, "r");


  int formatted_size = 1;
  char *formatted = calloc(1, 1);
  char buffer[READ_BLOCK_UNIT + 1];

  // Using buffer, we read back the rendered HTML text from created file
  while (fgets(buffer, READ_BLOCK_UNIT, fp)) {
    formatted_size += strlen(buffer);
    formatted = realloc(formatted, formatted_size);
    strcat(formatted, buffer);
  }

  pclose(fp);
  close_tmp(filename);
  formatted = realloc(formatted, strlen(formatted) + 1);
  return formatted;
}

/**
 * Render code blocks with highlighting using highlight
 */
char *highlight_code(const char *code) {

  // Run highlight to highlight code
  assert(code != 0);

  char *lang = predict_language(code);
  char lang_str[20];
  
  if (strcmp(lang, "N/A") == 0) {
    strcpy(lang_str, "--force ");
  } else {
    sprintf(lang_str, "--syntax=%s ", lang);
  }

  char *filename = write_to_tmp(code);
  char command[MAX_COMMAND_LEN];
  sprintf(command, "/usr/bin/env highlight --out-format=xterm256 %s" 
                   "--stdout --input=%s", lang_str, filename);
  FILE *fp = popen(command, "r");

  free(lang);


  char buffer[4 + MAX_LINE_LEN + 1] = { 0 };
  strcpy(buffer, "    ");
  int highlighted_size = 1;
  char *highlighed = calloc(highlighted_size, 1);

  // Using buffer, read back the highlighted code from created file
  while (fgets(buffer + 4, MAX_LINE_LEN, fp) != 0) {
    highlighted_size += strlen(buffer);
    highlighed = realloc(highlighed, highlighted_size);
    strcat(highlighed, buffer);
  }

  pclose(fp);
  close_tmp(filename);
  highlighed = realloc(highlighed, strlen(highlighed) + 1);
  return highlighed;
}

/**
 * Check if a section contains <code> tag denoting a block of code
 */
bool para_is_code(const char *para) {
  return strstr(para, "<pre><code>") != NULL;
}

/**
 *
 */
char *format_block(const char *para, int width) {
  assert(para != 0);

  char *formatted;

  if (para_is_code(para)) {
    char *tags_removed = remove_html_tags(para, MAX_LINE_LEN);
    formatted = highlight_code(tags_removed);
    free(tags_removed);
  } else {
    formatted = remove_html_tags(para, width);
  }

  return formatted;
}

/**
 * Convert UnixTime into appropriate MONTH DATE YEAR
 */
char *time_str_from_int(int time) {
  time_t _time = (time_t) time;
  struct tm tm;
  gmtime_r(&_time, &tm);
  char time_str[21];
  strftime(time_str, 20, "%b %d %Y", &tm);
  return strdup(time_str);
}

/**
 * Removes HTML escape characters
 */
char *unescape_html(const char *str) {
  char html_str[12 + strlen(str)];
  sprintf(html_str, "<pre>%s</pre>", str);
  char *unescaped = remove_html_tags(html_str, MAX_LINE_LEN);
  unescaped[strlen(unescaped) - 1] = '\0';
  return unescaped;
}

/**
 * Defines display format for an Answer
 */
char *format_answer(Answer *answer, int width) {
  assert(answer != 0);

  // Prepare Answer details
  char header[MAX_LINE_LEN];
  const char *accepted_str = answer->is_accepted ? "\xE2\x9C\x85" : "  ";
  char *time_str = time_str_from_int(answer->creation_date);
  char *answerer = unescape_html(answer->answerer);

  // Format header for an Answer
  sprintf(header, "%s%s %+i | by %s | %s%s\n\n", ESCAPE_BOLD_BEGIN, 
      accepted_str, answer->score, answerer, time_str, ESCAPE_BOLD_END);

  free(answerer);
  free(time_str);

  // Format body text, highlighted
  char *body = format_body(answer->body, width);
  char *formatted = malloc(strlen(header) + strlen(body) + 1 + 1);
  sprintf(formatted, "%s%s\n", header, body);

  free(body);
  return formatted;
}

/**
 * Defines display format for a Question
 */
char *format_question(Question *question, char **prompt, int width) {
  assert(question != 0);

  // Format the title of the Question
  char title[MAX_LINE_LEN];
  char *title_str = unescape_html(question->title);
  sprintf(title, "%s%s%s", ESCAPE_BOLD_BEGIN, title_str, ESCAPE_BOLD_END);


  // Prepare and format Question details
  char info[MAX_LINE_LEN];
  char *time_str = time_str_from_int(question->creation_date);
  char *asker = unescape_html(question->asker);
  char *answer_str = question->answer_count > 1 ? "answers" : "answer";
  sprintf(info, "%s%+i | %i %s | by %s | %s%s", ESCAPE_BOLD_BEGIN, question->score, 
         question->answer_count, answer_str, asker, time_str, ESCAPE_BOLD_END);
  free(time_str);

  // Format the Question body
  char *formatted_body = format_body(question->body, width);
  char *formatted = malloc(strlen(title) + 1 + strlen(info) + 2 + strlen(formatted_body) + 1 + 1);
  sprintf(formatted, "%s\n%s\n\n%s\n", title, info, formatted_body);
  free(formatted_body);

  int prompt_width = width - 2;

  // Update details of the bottom fixed header, to inform the user of the current Question being viewed
  if (prompt != 0) {
    // Format bottom header
    char postfix[12 + strlen(asker) + 1];
    sprintf(postfix, " - asked by %s", asker);
    *prompt = calloc(2 * prompt_width + 1, 1);
    int max_question_len = prompt_width - strlen(postfix);

    // Check if the details of header is too long
    int q_len = snprintf(*prompt, max_question_len + 1, "%s", title_str);
    if (q_len > max_question_len) {
      strcpy(*prompt + max_question_len - 3, "...");
    }
    strcat(*prompt, postfix);
    assert(strlen(*prompt) <= prompt_width);

    // Adding shell escape characters
    for (int i = strlen(*prompt) - 1; i >= 0; i--) {
      (*prompt)[i * 2 + 1] = (*prompt)[i];
      (*prompt)[i * 2] = '\\';
    }
  }

  free(title_str);
  free(asker);

  return formatted;
}

bool block_completed(const char *block) {
  return strstr(block, "</p>") || strstr(block, "</code></pre>");
}

/**
 * Format bodytext
 */
char *format_body(const char *body, int width) {
  char *body_cpy = strdup(body);
  char *save_ptr;

  char *curr_block = strdup(sentok_r(body_cpy, "\n\n", &save_ptr));
  int curr_block_size = strlen(curr_block) + 1;

  // Format a block of code
  char *formatted = calloc(1, 1);
  int formatted_size = 1;

  // Add it to the buffer of formatted strings
  char *para;
  while (true) {
    para = sentok_r(NULL, "\n\n", &save_ptr);
    if (block_completed(curr_block)) {
      char *formatted_block = format_block(curr_block, width);
      formatted_size += strlen(formatted_block) + 1;
      formatted = realloc(formatted, formatted_size);
      strcat(formatted, formatted_block);
      strcat(formatted, "\n");
      free(formatted_block);
      free(curr_block);
      curr_block = calloc(1, 1);
      curr_block_size = 1;
    }
    if (para == 0) {
      free(curr_block);
      break;
    }
    curr_block_size += strlen(para) + 2;
    curr_block = realloc(curr_block, curr_block_size);
    strcat(curr_block, para);
    strcat(curr_block, "\n\n");
  }
  free(body_cpy);
  return formatted;
}

char *format_preview(Question *question, int max_lines, int *lines_used, int width) {
  if (max_lines <= 3) {
    // Not even one line of body!
    if (lines_used != 0) {
      *lines_used = max_lines;
    }
    char *empty = calloc(max_lines + 1, 1);
    memset(empty, '\n', max_lines);
    return empty;
  }
  char *formatted = format_question(question, NULL, width);
  int lines;
  char *end = strichr(formatted, '\n', max_lines - 2, &lines);
  if (end != NULL) {
    *end = '\0';
    formatted = realloc(formatted, strlen(formatted) + 9 + 1);
    strcat(formatted, "\n   ...\n\n");
    lines += 2;
  }
  if (lines_used != 0) {
    *lines_used = lines;
  }
  return formatted;
}
