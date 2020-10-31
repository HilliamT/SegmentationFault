#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>

FILE *create_tmp(char **filename_ptr);

/**
 * Creates a temporary file and writes the given string to it, then return its
 * filename.
 */
char *write_to_tmp(const char *content);

void close_tmp(char *filename);

#endif

