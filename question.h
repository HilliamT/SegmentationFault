#ifndef QUESTION_H
#define QUESTION_H

#include <stdbool.h>
#include "util/linked_list.h"

/**
 * Used to hold information about Answers
 */
typedef struct Answer
{
    char *answerer; // display name of answerer
    char *body;
    bool is_accepted; // whether answer is the question's chosen best answer
    int score;
    int creation_date;
} Answer;

/**
 * Used to hold information about returned Questions, with a pointer to its Answers
 */
typedef struct Question
{
    char *title;         // title of the question
    char **tags;         // tags associated with question
    char *asker;         // display name of asker
    char *body;          // body of the question (its text content)
    char *link;          // URL to the question
    bool answered;       // whether the question has been answered (an answer has been accepted)
    int tag_count;       // number of tags
    int question_id;     // id of the question
    int view_count;      // how many views the question has
    int score;           // upvotes of the question
    int creation_date;   // unix-time of the date of creation
    int answer_count;    // count of the answers
    LinkedList *answers; // linked list of answers to this question
} Question;

#endif
