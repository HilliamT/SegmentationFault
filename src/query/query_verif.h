#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_QUERY_SIZE 65536

char* get_stackoverflow_results(char *query);
