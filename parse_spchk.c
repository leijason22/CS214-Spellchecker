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
#define BUFLENGTH 16
#endif

static int row_line;
static int col_num;
static int word_start_pos;

// void parse_file(char *path) {
//     row_line = 1;
//     col_num = 1;

//     int fd = open(path, O_RDONLY);
//     if (fd < 0) {
//         perror(path);
//         exit(EXIT_FAILURE);
//     }

//     int buflength = BUFLENGTH;
//     char *buf = malloc(BUFLENGTH);
//     if (buf == NULL) {
//         perror("malloc failed");
//         exit(EXIT_FAILURE);
//     }

//     int pos = 0;
//     int bytes;
//     int line_start;
// }

// Function to trim punctuation and handle special cases
char* trim_word(char* word) {
   char *end;

    // Trim leading characters
    while (strchr("'\"([{", *word)) word++;

    // Trim trailing punctuation
    end = word + strlen(word) - 1;
    while (end > word && ispunct((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = 0;

    return word;
}

void read_words(int fd, void (*use_word)(void *, char *, int, int), void *arg) {
    int buflength = BUFLENGTH;
    char *buf = malloc(BUFLENGTH);
    int pos = 0;
    int bytes;
    int line_num = 1;
    int col_num = 1;
    int word_start = 0;
    
    while ((bytes = read(fd, buf + pos, buflength - pos)) > 0) {
        if (DEBUG) printf("read %d bytes; pos=%d\n", bytes, pos);
        int bufend = pos + bytes;
        
        for (int i = pos; i < bufend; i++) {
            if (buf[i] == '\n') {
                line_num++;
                col_num = 1;
            } else if (!isalpha(buf[i])) {
                if (word_start < i) {
                    buf[i] = '\0';
                    char* trimmed_word = trim_word(buf + word_start);
                    use_word(arg, trimmed_word, line_num, col_num);
                }
                word_start = i + 1;
            } else if (word_start == i) {
                // Word starts here, update line and column numbers
                use_word(arg, buf + i, line_num, col_num);
            }
            col_num++;
        }
        
        // no partial word
        if (word_start == pos) {
            pos = 0;
            // partial word
            // move segment to start of buffer and refill remaining buffer
        } else if (word_start > 0) {
            int segment_length = bufend - word_start;
            memmove(buf, buf + word_start, segment_length);
            pos = segment_length;
            if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
            word_start = 0;
            // partial word filling entire buffer
        } else if (bufend == buflength) {
            buflength *= 2;
            buf = realloc(buf, buflength);
            if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
        }
    }
    
    free(buf);
}

void process_word(void *st, char *word, int line_num, int col_num) {
    int *p = st;
    printf("Line: %d, Column: %d, Word: %s\n", line_num, col_num, word);
    (*p)++;
}

int main(int argc, char **argv) {
    char *fname = argc > 1 ? argv[1] : "test.txt";
    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
        perror(fname);
        exit(EXIT_FAILURE);
    }
    
    int n = 0;
    read_words(fd, process_word, &n);
    
    close(fd);
    return EXIT_SUCCESS;
}