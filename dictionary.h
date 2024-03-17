// #ifndef DICTIONARY_H
// #define DICTIONARY_H
// #endif

#define MAX_WORD_LENGTH 100
#define INITIAL_ARRAY_SIZE 104335 //max number of words in the dict

//to make these accessible in other files
extern char **dictionary_array;
extern int word_count;

char *allocate_and_copy(const char *word);
int find_length(int fd);
char **read_dictionary(int fd, int *word_count);
int binary_search(int dictionary_size, char **dictionary, char *target);


