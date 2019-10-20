#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

typedef struct Flags {
  bool help;
} Flags;

extern Flags sf_flags;

char *process_args(int argc, char* argv[]);

#endif
