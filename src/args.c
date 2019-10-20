#include <stdlib.h>
#include <string.h>
#include "args.h"

Flags sf_flags = {
  .help = false
};

char *process_args(int argc, char* argv[]) {
  int i = 1;
  for (; i < argc; i++) {
    if (strncmp("--", argv[i], 2) == 0) {
      if (strcmp("--help", argv[i]) == 0) {
        sf_flags.help = true;
      }
    } else {
      i++;
      break;
    }
  }
  i--;
  char *query = calloc(1, 1);
  int query_size = 1;
  for (; i < argc; i++) {
    query_size += strlen(argv[i]) + 1;
    query = realloc(query, query_size);
    strcat(query, argv[i]);
    strcat(query, " ");
  }
  char *last_space = strrchr(query, ' ');
  if (last_space != 0) {
    *last_space = '\0';
  }
  return query;
}
