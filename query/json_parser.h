#ifndef ARM11_13_JSON_PARSER_H
#define ARM11_13_JSON_PARSER_H

#include <stdbool.h>
#include "../util/linked_list.h"
#include "lib-cjson/cJSON.h"
#include "../question.h"

LinkedList* json_parse(char* json_as_string);
char* copy_str(char *sentence);
cJSON* get_value(cJSON* json, char* key);
char* export_str_val(cJSON* json, char* key);
int export_int_val(cJSON* json, char* key);
void construct_question_data(Question* question_struct, cJSON* question);
void construct_answer_data(Answer* answer_struct, cJSON* answer);

#endif //ARM11_13_JSON_PARSER_H
