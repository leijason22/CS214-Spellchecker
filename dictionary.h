// #ifndef DICTIONARY_H
// #define DICTIONARY_H
// #endif

#define MAX_WORD_LENGTH 100
#define INITIAL_ARRAY_SIZE 104335 //max number of words in the dict

char *allocate_and_copy(const char *word);
char **read_dictionary(const char *path, int *word_count);

