#ifndef STRING_UTILS_H
#define STRING_UTILS_H

/**
 * Similar to `strtok_r`, but splits the input string into tokens only
 * when the entire delimit_str is matched.
 */
char *sentok_r(char *src, const char *delimit_str, char **save_ptr);

/**
 * Finds the ith occurrence of c in s and returns a pointer to it.
 * If num_c is not NULL, the total number of c found is written to it.
 * If c occurs less than i times, a NULL pointer is returned.
 */
char *strichr(char *s, char c, int i, int *num_c);

#endif
