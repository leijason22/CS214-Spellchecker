#define MAX_WORD_LENGTH 100
#define INITIAL_ARRAY_SIZE 104335 //max number of words in the dict

//to make these accessible in other files
extern char **dictionary_array;
extern int word_count;

extern char **dictionary_array;

int find_length(int fd);
void make_dict(int fd, int total_lines);
void traverse(char *file, char** diction);



