#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFLENGTH
#define BUFLENGTH 1024
#endif

static int row_line;
static int col_num;
static int word_start_pos;

/*
parses input files and performs spell-check operation

- text files broken into LINES, which are a sequence of characters ENDING with a newline character
- lines may contain WORDS, which are a sequence of non-whitespace characters

- number all lines in a file, starting from one
- each character in a line has a column number, which also starts from one
    -track line and column num for each word, defined as the colun number of the word's first character

TRAILING PUNCTUATION
- to avoid problems with common sentence punctuation, we ignore punctuation marks occurring at the end
of a word
- ignore quotation marks ' and " and brackets ( and [ and { at the start of a word

HYPHENS
- a hyphenated word contains two or more smaller words separated by hyphens (-)
- these are correctly spelled if all the component words are correctly spelled

CAPITALIZATION
- allow up to three variations of a word based on capitalization: 
    1. regular
    2. initial capital
    3. all capital letters
    ex: dictionary contains "hello" => "hello", "Hello", "HELLO" are all correctly spelled
    
    capital letters in dictionary must be capital: "MacDonald" => MacDonald and MACDONALD, but NOT Macdonald or macdonald
*/

// Function to trim punctuation and handle special cases
char* trim_word(char* word) {
    // Trim leading characters
    char *start = word;
    while (*start && strchr("'\"([{", *start)) {
        start++;
    }

    // Trim trailing punctuation
    char *end = word + strlen(word) - 1;
    while (end > start && ispunct((unsigned char)*end)) {
        *end = '\0';
        end--;
    }

    return start;
}

char* to_lowercase(const char *word) {
    // char *lowercase = malloc(strlen(word) + 1);
    char *lowercase = strdup(word);
    if (lowercase == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; word[i]; i++) {
        lowercase[i] = tolower(word[i]);
    }
    lowercase[strlen(word)] = '\0'; //null terminate the lowercased word

    return lowercase;
}

//case of hyphenated word
int check_component(char *component) {
    char *lowercase_component = to_lowercase(component);
    int check = binary_search(word_count, dictionary_array, lowercase_component);
    free(lowercase_component);

    return check;
}

int check_word(char *word, char *path, int word_start_col) {
    if (DEBUG) {
        printf("Checking Word: %s\n", word);
    }

    word = trim_word(word);

    char *hyphenated = strtok(word, "-");
    while (hyphenated) {
        if (check_component(hyphenated) == -1) {
            printf("%s (%d,%d): %s\n", path, row_line, word_start_col, word);
            return 0; //not hyphenated
        }
        hyphenated = strtok(NULL, "-");
    }
    return 1; //hyphenated and goes through
}

void parse_line(char *path, char *line) {
    int word_start_col = 1;
    // int count = 1; //keep track of letter count in word
    char *word;
    char *ptr = line;
    char *prev = ptr;
    while (*ptr != '\0') {
        while (!isalpha(*ptr) && *ptr != '\0') {
            ptr ++;
            col_num += 1; //increment column number
        }
        prev = ptr; //beginning of the word

        if (*ptr == '\0') break; //exits loop if end of word is reached

        word_start_col = col_num; //record starting column number of the word
        // count = 1; //reset word count per word

        while (isalpha(*ptr) && *ptr != '\0') {
            // count += 1;
            ptr ++;
            col_num += 1; //increment column number
        }

        int word_length = ptr - prev;
        if (word_length > 0) {
            word = (char *) malloc(sizeof(char) * (word_length + 1)); //allocate memory for word
            strncpy(word, prev, word_length); //copies word
            word[word_length] = '\0'; //null terminates the word

            //process word (trimming, checking with dictionary)
            int result = check_word(word, path, word_start_col);
            free(word); //free allocated memory for word after
        }
    }
}

//new traverse and parse file
void read_file(char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror(path);
        exit(EXIT_FAILURE);
    }

    int buflength = BUFLENGTH;
    char *buf = malloc(buflength);
    if (buf == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

     // Reset file pointer to the beginning of the file
    // lseek(fd, 0, SEEK_SET);

    int pos = 0;
    int bytes;
    int line_start;

    while ((bytes = read(fd, buf + pos, buflength - pos)) > 0) {
        line_start = 0;
        int bufend = pos + bytes;

        // Process each character in the buffer
        for (int i = 0; i < bufend; i++) {
            if (buf[i] == '\n') {
                //newline found, complete line read
                buf[i] = '\0'; // null-terminate/end of word
                //put some error handling here
                parse_line(path, buf + line_start);
                col_num = 1; //resets column number for new line
                row_line += 1; //new line/next row
                line_start = i + 1; // Update line_start to the next character
            }
        }

        //no partial line
        if (line_start == pos) {
            pos = 0;
            //partial line
            //move segment to start of buffer and refill remaining buffer
        } else if (line_start > 0) {
            int segment_length = pos - line_start;
            memmove(buf, buf + line_start, segment_length);
			pos = segment_length;
			// if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
			// partial line filling entire buffer
        } else if (bufend == buflength) {
            buflength *= 2;
			buf = realloc(buf, buflength);
			// if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
        }
    }

    // partial line in buffer after EOF
    if (pos > 0) {
        if (pos == buflength) {
            buf = realloc(buf, buflength + 1);
        }
        buf[pos] = '\0';
        parse_line(path, buf);
        // printf("%s\n", buf);
        row_line += 1;
    }
    close(fd);
    free(buf);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *filename = argv[1];
    parse_file(filename);

    return EXIT_SUCCESS;
}