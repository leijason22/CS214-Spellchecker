// #ifndef DICTIONARY_H
// #define DICTIONARY_H
// #endif

#define MAX_WORD_LENGTH 100
#define INITIAL_ARRAY_SIZE 104335 //max number of words in the dict

extern char **dictionary_array;

int find_length(int fd);
void make_dict(int fd, int total_lines);
void traverse(char *file, char** diction);
int binary_search(int dictionary_size, char **dictionary, char *target);


