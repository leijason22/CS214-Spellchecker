#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFLENGTH 
#define BUFLENGTH 16
#endif

/*
this implementation of dictionary reading involves mallocing an array of size 104335 (max num of words of dictionary file in
/usr/share/dict/words)

dictionary reading is complete, not extensively tested other than just printing it all out
this implementation mallocs every NEW WORD read and sets buffer size to MAX_WORD_LENGTH

this is stored in the dictinoary malloced array - char** dictinoary

dictionary data structure, creation of dictionary, reading in and populating, and searching for a 
target

*/

static char **dictionary_array;
static int num_lines;

char *allocate_and_copy(const char *word) {
    char *new_word = (char *)malloc(strlen(word) + 1);  // +1 for the null terminator
    if (new_word != NULL) {
        strcpy(new_word, word);
    }
    return new_word;
}

int find_length(int fd) {
    lseek(fd, 0, SEEK_SET); // Move the file pointer to the beginning
    int line_count = 0;
    int buflength = BUFLENGTH;
    char *buf = malloc(BUFLENGTH);

    int pos = 0;
    int bytes;
    int line_start;

    while ((bytes = read(fd, buf + pos, buflength - pos)) > 0) {
	    //if (DEBUG) printf("read %d bytes; pos=%d\n", bytes, pos);
        line_start = 0;
        int bufend = pos + bytes;

        while (pos < bufend) {
            // if (DEBUG) printf("start %d, pos %d, char '%c'\n", line_start, pos, buf[pos]);
            if (buf[pos] == '\n') {
            // we found a line, starting at line_start ending before pos
                buf[pos] = '\0';
                line_start = pos + 1;
                line_count += 1; //increment line count
            }
            pos++;
        }

        // Print buffer content for debugging
        printf("Buffer content: %s\n", buf);

        // Print number of lines detected
        printf("Number of lines detected: %d\n", line_count);

        // no partial line
        if (line_start == pos) {
            pos = 0;
            // partial line
            // move segment to start of buffer and refill remaining buffer
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
        line_count++; //increment line count
    }
    free(buf);
    return line_count;
}

char **read_dictionary(int fd, int *word_count) {
    // int fd = open(path, O_RDONLY);
    // if (fd < 0) {
    //     fprintf(stderr, "Error: Can't open dictionary\n");
    //     return NULL;
    // }

    num_lines = find_length(fd);

    dictionary_array = (char **)malloc(num_lines * sizeof(char *));

    if (dictionary_array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        close(fd);
        return NULL;
    }

    //resets the file pointer to the beginning of the file
    lseek(fd, 0, SEEK_SET);

    char buffer[MAX_WORD_LENGTH];
    int count = 0;
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, MAX_WORD_LENGTH - 1)) > 0) {
        buffer[bytes_read] = '\0'; // null-terminate the buffer
        char *newline_pos = strchr(buffer, '\n'); //looks for first occurrence of \n

        if (newline_pos != NULL) {
            *newline_pos = '\0'; // Remove the newline character
        }

        dictionary_array[count] = allocate_and_copy(buffer);

        //check if malloc fails
        if (dictionary_array[count] == NULL) {
            fprintf(stderr, "Memory allocation for word failed\n");
            close(fd);
            for (int i = 0; i < count; i++) {
                free(dictionary_array[i]);
            }
            free(dictionary_array);
            return NULL;
        }
        count++;
    }

    close(fd);
    *word_count = count;
    return dictionary_array;
}

//binary search to check if a word is in the dictionary

int binary_search(int dictionary_size, char **dictionary, char *target) {
    int low = 0;
    int high = dictionary_size - 1;
    
    while (low <= high) {
        int mid = low + (low + high) / 2;
        int cmp = strcmp(dictionary[mid], target);
        
        if (cmp == 0) {
            return mid; //word found
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1; //word not found
}
