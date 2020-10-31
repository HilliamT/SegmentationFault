#include "string_utils.h"
#include <string.h>
#include <assert.h>

/**
 * Similar to `strtok_r`, but splits the input string into tokens only
 * when the entire delimit_str is matched.
 */
char *sentok_r(char *src, const char *delimit_str, char **save_ptr) {
  assert(delimit_str != 0);
  assert(save_ptr != 0);
  if (src == 0) {
    src = *save_ptr;
  }
  if (*src == '\0') {
    return 0;
  }
  char *loc = strstr(src, delimit_str);
  if (loc == 0) {
    *save_ptr = src + strlen(src);
    return src;
  }
  *loc = '\0';
  *save_ptr = loc + strlen(delimit_str);
  return src;
}

/**
 * Finds the ith occurrence of c in s and returns a pointer to it.
 * If num_c is not NULL, the total number of c found is written to it.
 * If c occurs less than i times, a NULL pointer is returned.
 */
char *strichr(char *s, char c, int i, int *num_c) {
  assert(c != '\0');
  assert(i != 0);
  char *next = s;
  int j = 0;
  for (; j < i; j++) {
    next = strchr(next, c);
    if (next != NULL) {
      next++;
    } else {
      break;
    }
  }
  if (num_c != 0) {
    *num_c = j;
  }
  return j == i ? next - 1 : NULL;
}
