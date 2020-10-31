CC        = clang
CFLAGS    = -Wall -g -pedantic -std=c11 -Werror -D_DEFAULT_SOURCE -MD
LDLIBS    = -lcurl
VPATH     = display/:query/:query/:query/lib-cjson/:../:language_inference/predict_language/:language_inference/search_suggest/:language_inference/predict_language/large_double:util/
SEG_FAULT = sf.o args.o $(DISPLAY) $(QUERY) $(LANG_INF) $(DATA_STR)
DISPLAY   = display.o file_utils.o formatter.o string_utils.o
QUERY     = cJSON.o json_parser.o query_verif.o
LANG_INF  = large_double.o naive_bayes.o levenshtein_dist.o trie.o
DATA_STR  = linked_list.o map.o

.SUFFIXES: .c .o

.PHONY: all clean

all: sf

sf: $(SEG_FAULT)

clean:
	rm -f sf
	rm -f $(wildcard *.o)
	rm -f $(wildcard *.d)

-include $(SEG_FAULT:.o=.d)
