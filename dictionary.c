#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFLENGTH 
#define BUFLENGTH 16
#endif

#define BUFFER_SIZE 1024

/*
this implementation of dictionary reading involves mallocing an array of size 104335 (max num of words of dictionary file in
/usr/share/dict/words)

dictionary reading is complete, not extensively tested other than just printing it all out
this implementation mallocs every NEW WORD read and sets buffer size to MAX_WORD_LENGTH

this is stored in the dictinoary malloced array - char** dictinoary

dictionary data structure, creation of dictionary, reading in and populating, and searching for a 
target
 /usr/share/dict/words 
*/

char **dictionary_array;
int num_lines;

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

// void make_dict(int fd, int total_lines){
//     dictionary_array = malloc(total_lines * sizeof(char*));
//     int count = 0;

//     lseek(fd, 0, SEEK_SET); // Move the file pointer to the beginning
//     int buflength = BUFLENGTH;
//     char *buf = malloc(BUFLENGTH);

//     int pos = 0;
//     int bytes;
//     int line_start;

//     while ((bytes = read(fd, buf + pos, buflength - pos)) > 0) {
// 	    //if (DEBUG) printf("read %d bytes; pos=%d\n", bytes, pos);
//         line_start = 0;
//         int bufend = pos + bytes;

//         while (pos < bufend) {
//             // if (DEBUG) printf("start %d, pos %d, char '%c'\n", line_start, pos, buf[pos]);
//             if (buf[pos] == '\n') {
//             // we found a line, starting at line_start ending before pos
//                 buf[pos] = '\0';
//                 line_start = pos + 1;
//             }
//             pos++;
//         }

//         strcpy(dictionary_array[0], buf);
//         count++;

//         // no partial line
//         if (line_start == pos) {
//             pos = 0;
//             // partial line
//             // move segment to start of buffer and refill remaining buffer
//         } else if (line_start > 0) {
//             int segment_length = pos - line_start;
//             memmove(buf, buf + line_start, segment_length);
//             pos = segment_length;
//             // if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
//         // partial line filling entire buffer
//         } else if (bufend == buflength) {
//             buflength *= 2;
//             buf = realloc(buf, buflength);
//             // if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
//         }
//     }
    
//     // partial line in buffer after EOF
//     if (pos > 0) {
//         if (pos == buflength) {
//             buf = realloc(buf, buflength + 1);
//         }
//         buf[pos] = '\0';
//     }
//     free(buf);
// }

void make_dict(int fd, int total_lines) {
    // Allocate memory for dictionary_array
    dictionary_array = malloc(total_lines * sizeof(char*));
    if (dictionary_array == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    // Initialize variables
    int count = 0;
    int buflength = BUFLENGTH;
    char *buf = malloc(BUFLENGTH);
    if (buf == NULL) {
        perror("malloc failed");
        exit(1);
    }

    // Reset file pointer to the beginning of the file
    lseek(fd, 0, SEEK_SET);

    int pos = 0;
    int bytes;
    int line_start;

    while ((bytes = read(fd, buf + pos, buflength - pos)) > 0) {
        line_start = 0;
        int bufend = pos + bytes;

        // Process each character in the buffer
        for (int i = 0; i < bufend; i++) {
            if (buf[i] == '\n') {
                // Found a newline, so we have a complete line
                buf[i] = '\0'; // Null-terminate the line
                dictionary_array[count] = strdup(buf + line_start); // Store the line
                if (dictionary_array[count] == NULL) {
                    perror("strdup failed");
                    exit(1);
                }
                count++;
                line_start = i + 1; // Update line_start to the next character
            }
        }

        // If there is a partial line at the end of the buffer, move it to the beginning
        if (line_start < bufend) {
            int segment_length = bufend - line_start;
            memmove(buf, buf + line_start, segment_length);
            pos = segment_length;
        } else {
            pos = 0;
        }

        // If the buffer is full, double its size
        if (bufend == buflength) {
            buflength *= 2;
            buf = realloc(buf, buflength);
            if (buf == NULL) {
                perror("realloc failed");
                exit(1);
            }
        }
    }

    free(buf);
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

void remove_trailing_punc(char *word) {
    int len = strlen(word);
    while (len > 0 && ispunct(word[len - 1])) {
        word[len - 1] = '\0';
        len--;
    }
}

int spelling(char word[], char **diction){
    //printf("spelling\n");
    //tolower(word[0]); // in case first letter is capitalized and library is all lowercaps

    //case 1: exact match
    char *startOfWord = word; 

    for (int i = 0; diction[i] != NULL; i++) {
        if ((strcmp(diction[i], word)) == 0){
            return 0;
        }
    }

    //case 2: remove trailing punctuation
    char original[MAX_WORD_LENGTH];
    strcpy(original, word);

    remove_trailing_punc(word);


    for (int i = 0; diction[i] != NULL; i++) { //try again
        if ((strcmp(diction[i], word)) == 0){
            return 0;
        }
    }

    //reset back to original
    strcpy(word, original);

    //case 3: initial capital
    word[0] = toupper(word[0]);
    for (int i = 1; word[i] != '\0'; i++) {
        word[i] = tolower(word[i]);
    }

    for (int i = 0; diction[i] != NULL; i++) {
        if ((strcmp(diction[i], word)) == 0) {
            return 0; // Word with initial capital found
        }
    }

    //reset back to original
    strcpy(word, original);

    //case 4: all capitalized
    for (int i = 0; word[i] != '\0'; i++) {
        word[i] = toupper(word[i]);
    }

    for (int i = 0; diction[i] != NULL; i++) {
        if ((strcmp(diction[i], word)) == 0) {
            return 0; // Uppercase word found
        }
    }

    //reset back to original
    strcpy(word, original);

    //case 5: convert word to all lowercase and check again
    for (int i = 0; word[i] != '\0'; i++) {
        word[i] = tolower(word[i]);
    }


    for (int i = 0; diction[i] != NULL; i++) { //try again
        if ((strcmp(diction[i], word)) == 0){
            return 0;
        }
    }

    //if not found at all, return original form of the word
    strcpy(word, original);

    return 1;
}


void traverse(char *file, char** diction){
    int row = 1;
    int col = 1;
    int word_col = 0; 

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char temp[45];
    int start = 0;
    int last_letter = 0;
    int bytes_eaten = 0;
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            bytes_eaten++; 
            if (isalpha(buffer[i])) { //if it is alphabet
                temp[start] = buffer[i];
                start++;
                last_letter = 1;
                if(word_col == 0) word_col = col; 
                //printf("word started");
            }else{ 
                //printf("else");
                if(last_letter == 1){
                    //printf("if");
                    temp[start] = '\0';
                    int one_if_wrong = spelling(temp, diction);
                    if(one_if_wrong == 1){
                        printf(" %s (%d,%d): %s \n", file, row, word_col, temp);
                    }
                    //printf("%s \n", temp);
                    for (int i = 0; i < sizeof(temp); i++) { //clear out array to use again for next word
                        
                        temp[i] = 0;
                    }
                    start = 0; 
                    word_col = 0;
                    last_letter = 0;
                }
            }

            col++; 

            if(buffer[i] == '\n'){ // newline count
                row++; 
                col = 1; 
                int segment_length = bytes_read - bytes_eaten;
                memmove(buffer, buffer + start, segment_length);
            }
        }
    }

    if(last_letter == 1){ // last word case, bcuz doesn't run since never hits if condition in loop
        temp[start] = '\0';
        int one_if_wrong = spelling(temp, diction);
        if(one_if_wrong == 1){
            printf(" %s (%d,%d): %s \n", file, row, word_col, temp);
                }
        for (int i = 0; i < sizeof(temp); i++) { //clear out array to use again for next word
            
            temp[i] = 0;
        }
        start = 0; 
        word_col = 0;
        last_letter = 0;
    }

    if (bytes_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(fd);
    return;
}
