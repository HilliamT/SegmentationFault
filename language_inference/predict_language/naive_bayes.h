#ifndef ARM11_13_NAIVE_BAYES_H
#define ARM11_13_NAIVE_BAYES_H

#include "../../util/map.h"

void initialise_naiveb_lang_infer();
char **cache_languages();
Map *cache_file_lengths(char **file_names, int num_of_languages);
char *predict_language(const char *text);
#endif //ARM11_13_NAIVE_BAYES_H
