#ifndef FORMATTER_H
#define FORMATTER_H

#include "../question.h"

#define ESCAPE_BOLD_BEGIN "\033[1m"
#define ESCAPE_BOLD_END   "\033[0m"

char *format_answer(Answer *answer, int width);

char *format_question(Question *question, char **prompt, int width);

/**
 * Formats the answer by removing the html tags, adding indentations,
 * and highlighting the code
 */
char *format_body(const char *body, int width);

char *format_preview(Question *question, int max_lines, int *lines_used, int width);

#endif

