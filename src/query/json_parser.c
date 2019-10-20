#include "json_parser.h"
#include "../../linked_list.h"
#include "lib-cjson/cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Extracts from stringified JSON, the list of relevant question as sent in response
 */
LinkedList* json_parse(char* json_as_string) {

    // Check that the returned string is of JSON form and can be converted into such
    cJSON* json = cJSON_Parse(json_as_string);
    if (json == NULL) {
        printf("Failed to parse string as JSON data");
        exit(0);
    }

    // All question items are stored in the key "items"
    cJSON* questions = get_value(json, "items");
    cJSON* question = NULL;
    cJSON* answer = NULL;

    LinkedList* questions_list = linked_list_make();

    // For each question, we put it into our standardised struct Question
    cJSON_ArrayForEach(question, questions) {
        Question* question_data = malloc(sizeof(Question));
        construct_question_data(question_data, question); // set up a Question

        // We also include the answers to each question, saved in a list
        LinkedList* answers_list = linked_list_make();

        cJSON_ArrayForEach(answer, get_value(question, "answers")) { // for each answer
            Answer* answer_data = malloc(sizeof(Answer));
            construct_answer_data(answer_data, answer); // set up an Answer
            linked_list_append(answers_list, answer_data); // add it to a list
        }
        question_data->answers = answers_list;
            linked_list_append(questions_list, question_data);
    }

    cJSON_Delete(json); // cleanup of json internal malloc
    return questions_list;
}

/**
 * Used to copy a string, including null pointers, into dynamic memory
 */
char* copy_str(char *sentence) {
    if (sentence == 0) return 0;
    return strdup(sentence);
}

/**
 * Gets the key of a JSON object
 */
cJSON* get_value(cJSON* json, char* key) {
    return cJSON_GetObjectItemCaseSensitive(json, key);
}

/**
 * Returns the value of a JSON object, assumed to be a String constant
 */
char* export_str_val(cJSON* json, char* key) {
    cJSON* str_val = get_value(json, key);
    if (!cJSON_IsString(str_val)) {
        printf("Exporting cJSON obj is not a string: %s\n", cJSON_Print(str_val));
        exit(1);
    }
    return copy_str(str_val->valuestring);
}

/**
 * Returns the value of a JSON object, assumed to be a Integer constant
 */
int export_int_val(cJSON* json, char* key) {
    cJSON* int_val = get_value(json, key);
    if (!cJSON_IsNumber(int_val)) {
        printf("Exporting cJSON obj is not an int: %s\n", cJSON_Print(int_val));
        exit(1);
    }
    return int_val->valueint;
}

/**
 * Returns the tags of a question JSON object, allocating each tag on the heap.
 */
char** get_tags(cJSON* json) {
    cJSON* tag_array = get_value(json, "tags");
    if (!cJSON_IsArray(tag_array)) {
        printf("Exporting cJSON obj is not an array: %s\n", cJSON_Print(tag_array));
        exit(1);
    }

    int size = cJSON_GetArraySize(tag_array);
    char** arr = malloc(sizeof(char*) * size);

    for (int i = 0; i < size; i++) {
        char* elem = cJSON_GetArrayItem(tag_array, i)->valuestring;
        arr[i] = malloc(sizeof(char) * (strlen(elem) + 1));
        strcpy(arr[i], elem);
    }

    return arr;
}

/**
 * Get the number of tags in a question JSON object
 */
int get_tag_count(cJSON* json) {
    cJSON* tag_array = get_value(json, "tags");
    if (!cJSON_IsArray(tag_array)) {
        printf("Exporting cJSON obj is not an array: %s\n", cJSON_Print(tag_array));
        exit(1);
    }

    return cJSON_GetArraySize(tag_array);
}

/**
 * Converts a question JSON object into our standardised Question struct format
 */
void construct_question_data(Question* question_struct, cJSON* question) {
    question_struct->title = export_str_val(question, "title");
    question_struct->tags = get_tags(question);
    question_struct->tag_count = get_tag_count(question);
    question_struct->asker = export_str_val(get_value(question, "owner"), "display_name");
    question_struct->body = export_str_val(question, "body");
    question_struct->link = export_str_val(question, "link");
    question_struct->answered = cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(question, "is_answered"));
    question_struct->question_id = export_int_val(question, "question_id");
    question_struct->view_count = export_int_val(question, "view_count");
    question_struct->score = export_int_val(question, "score");
    question_struct->creation_date = export_int_val(question, "creation_date");
    question_struct->answer_count = export_int_val(question, "answer_count");
}

/**
 * Converts an answer JSON object into our standardised Answer struct format
 */
void construct_answer_data(Answer* answer_struct, cJSON* answer) {
    answer_struct->answerer = export_str_val(get_value(answer, "owner"), "display_name");
    answer_struct->body = export_str_val(answer, "body");
    answer_struct->is_accepted = cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(answer, "is_accepted"));
    answer_struct->score = export_int_val(answer, "score");
    answer_struct->creation_date = export_int_val(answer, "creation_date");
}
