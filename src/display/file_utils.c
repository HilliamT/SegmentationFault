#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

/**
 * Open a temporary file as to write to
 */
FILE *create_tmp(char **filename_ptr) {
  char filename[15] = "/tmp/sf_XXXXXX";
  FILE *fp = fdopen(mkstemp(filename), "w");
  if (filename_ptr != 0) {
    *filename_ptr = strdup(filename);
  } 
  return fp;
}

/**
 * Creates a temporary file and writes the given string to it, then return its
 * filename.
 */
char *write_to_tmp(const char *content) {
  assert(content != 0);
  char *filename;
  FILE *fp = create_tmp(&filename);
  fwrite(content, 1, strlen(content), fp);
  fclose(fp);
  return filename;
}

/**
 * Close the tmp folder connection
 */
void close_tmp(char *filename) {
  unlink(filename);
  free(filename);
}

