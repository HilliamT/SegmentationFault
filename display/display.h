#ifndef DISPLAY_H
#define DISPLAY_H

#include "../question.h"
#include "../util/linked_list.h"

void display_clear(void);

void display_preview(ListEntry *curr_question_entry);

void display_question(Question *question);

void display_usage(void);

void display_help_page(void);

#endif
