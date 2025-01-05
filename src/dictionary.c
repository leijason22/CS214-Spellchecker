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
        line_start = 0;
        int bufend = pos + bytes;

        while (pos < bufend) {      
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
        // partial line filling entire buffer
        } else if (bufend == buflength) {
            buflength *= 2;
            buf = realloc(buf, buflength);
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

void make_dict(int fd, int total_lines) {
    // Allocate initial memory for dictionary_array
    int capacity = total_lines + 1; // Use the input total_lines as initial capacity
    dictionary_array = malloc(capacity * sizeof(char*));
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
                buf[i] = '\0'; // Null-terminate the line
                if (count == capacity) {
                    // Increase the capacity if necessary
                    capacity *= 2;
                    dictionary_array = realloc(dictionary_array, capacity * sizeof(char*));
                    if (dictionary_array == NULL) {
                        perror("realloc failed");
                        exit(1);
                    }
                }
                dictionary_array[count++] = strdup(buf + line_start);
                if (dictionary_array[count-1] == NULL) {
                    perror("strdup failed");
                    exit(1);
                }
                line_start = i + 1; // Update line_start to the next character
            }
        }

        // Handle partial line remaining at the end of the buffer
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

   
    dictionary_array[count] = NULL; // Properly terminate the array of strings
    

    // printf("Final count: %d, Capacity: %d\n", count, capacity);
    // if (dictionary_array[count] == NULL) {
    //     printf("Dictionary is properly null-terminated at index %d\n", count);
    // }

    
    free(buf);
}




int spelling(char word[], char **diction){
    //case 1: exact match

    if (word == NULL) {
        printf("Error: word is NULL\n");
        return 1; // Error code for NULL input
    }

//       int i = 0;
// while (diction[i] != NULL) {
//     printf("diction[%d] = %p, string = '%s'\n", i, diction[i], diction[i]);
//     printf("Accessing index %d\n", i);
// if (diction[i] == NULL) {
//     printf("Reached end of dictionary at index %d\n", i);
//     break;
// }
//     i++;
// }

    for (int i = 0; diction[i] != NULL; i++) {
        if ((strcmp(diction[i], word)) == 0){
            return 0;
        }
    }

    //case 2: remove trailing punctuation
    char original[MAX_WORD_LENGTH];
    strncpy(original, word, MAX_WORD_LENGTH);
    original[MAX_WORD_LENGTH - 1] = '\0';  // Ensure null termination


    //case 3: initial capital
    word[0] = tolower(word[0]);
    for (int i = 0; diction[i] != NULL; i++) {
        if ((strcmp(diction[i], word)) == 0){
            return 0;
        }
    }
    

//reset back to original
    strncpy(word, original, MAX_WORD_LENGTH);
    word[MAX_WORD_LENGTH - 1] = '\0';

    //case 4: all capitalized
    int all_caps = 1;
    for (int i = 0; word[i] != '\0'; i++) {
        if(islower(word[i])){ 
            all_caps = 0;
        }
    }
    if (all_caps == 1){
        for (int i = 0; word[i] != '\0'; i++) {
            word[i] = tolower(word[i]);
        }
    }
    for (int i = 0; diction[i] != NULL; i++) {
        if ((strcmp(diction[i], word)) == 0){
            return 0;
        }
    }

    //reset back to original
    strncpy(word, original, MAX_WORD_LENGTH);
    word[MAX_WORD_LENGTH - 1] = '\0';

    int caps_match = 1; 
    int matching_dict = 0;
    

    for (int i = 0; word[i] != '\0'; i++) {
            word[i] = tolower(word[i]);
        }

    for (int i = 0; diction[i] != NULL; i++) {
       char *dict_word = diction[i];
        char str_comp[MAX_WORD_LENGTH];
        strncpy(str_comp, dict_word, MAX_WORD_LENGTH);
        str_comp[MAX_WORD_LENGTH - 1] = '\0';
        for (int p = 0; dict_word[p] != '\0'; p++) {
            str_comp[p] = tolower(str_comp[p]);
        }
        
        if(strcmp(word, str_comp) == 0) {
            matching_dict = i;
            break;
        }
    }

  strncpy(word, original, MAX_WORD_LENGTH);
    word[MAX_WORD_LENGTH - 1] = '\0';

    char *found_word = diction[matching_dict];

    for (int p = 0; found_word[p] != '\0'; p++) {
        if ((isupper(found_word[p])) && (found_word[p] != word[p])) {
            caps_match = 0;
        }
    }

    if (caps_match == 1 && all_caps == 1){
        return 0;
    }


    return 1;
}

int one_if_bracket(char c) {
    if (c == '"' || c == '\'' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}') return 1;
    return 0; // ” “
}

// void traverse(char *file, char** diction){
//     int row = 1;
//     int col = 1;
//     int word_col = 0; 

//     char buffer[BUFFER_SIZE];
//     ssize_t bytes_read;

//     int fd = open(file, O_RDONLY);
//     if (fd == -1) {
//         perror("open");
//         exit(EXIT_FAILURE);
//     }

//     char temp[MAX_WORD_LENGTH];
//     int start = 0;
//     int last_letter = 0;
//     int bytes_eaten = 0;
//     int last_real_char = 0;
//     int brackets = 0;

//     while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
//         for (int i = 0; i < bytes_read; i++) {
//             brackets = one_if_bracket(buffer[i]);
//             bytes_eaten++; 
//             if (isalpha(buffer[i]) || (last_letter == 0 && (!isspace(buffer[i])) && brackets == 0) ||  ((last_letter == 1) && (!isspace(buffer[i])) && (buffer[i] != '-'))) { //if it is alphabet, and not a space
//                 temp[start] = buffer[i];
//                 if(isalpha(buffer[i])) last_real_char = start;
//                 start++;
//                 last_letter = 1;
//                 if(word_col == 0) word_col = col; 
                
//                 //printf("word started");
//             }else{ 
//                 //printf("else");
//                 if(last_letter == 1){
//                     //printf("if");
//                     temp[start] = '\0';
//                     temp[last_real_char + 1] = '\0';
//                     int one_if_wrong = spelling(temp, diction);
//                     if(one_if_wrong == 1){
//                         printf(" %s (%d,%d): %s \n", file, row, word_col, temp);
//                     }
//                     //printf("%s \n", temp);
//                     for (int i = 0; i < sizeof(temp); i++) { //clear out array to use again for next word
                        
//                         temp[i] = 0;
//                     }
//                     start = 0; 
//                     word_col = 0;
//                     last_letter = 0;
//                     brackets = 0;
//                 }
//             }

//             col++; 

//             if(buffer[i] == '\n'){ // newline count
//                 row++; 
//                 col = 1; 
//                 int segment_length = bytes_read - bytes_eaten;
//                 memmove(buffer, buffer + start, segment_length);
//             }
//         }
//     }

//     if(last_letter == 1){ // last word case, bcuz doesn't run since never hits if condition in loop
//         temp[start] = '\0';
//         temp[last_real_char + 1] = '\0';
//         int one_if_wrong = spelling(temp, diction);
//         if(one_if_wrong == 1){
//             printf(" %s (%d,%d): %s \n", file, row, word_col, temp);
//                 }
//         for (int i = 0; i < sizeof(temp); i++) { //clear out array to use again for next word
            
//             temp[i] = 0;
//         }
//         start = 0; 
//         word_col = 0;
//         last_letter = 0;
//         brackets = 0;
//     }

//     if (bytes_read == -1) {
//         perror("read");
//         exit(EXIT_FAILURE);
//     }

//     close(fd);
//     return;
// }

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

    char temp[MAX_WORD_LENGTH];
    int start = 0;
    int last_letter = 0;
    int bytes_eaten = 0;
    int last_real_char = 0;
    int brackets = 0;

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            brackets = one_if_bracket(buffer[i]);
            bytes_eaten++;
            if (isalpha(buffer[i]) || (last_letter == 1 && buffer[i] == '-') || (last_letter == 1 && !isspace(buffer[i]) && brackets == 0)) {
                temp[start] = buffer[i];
                if(isalpha(buffer[i]) || buffer[i] == '-') {
                    last_real_char = start;  // Update last real character for non-space and non-punctuation
                }
                start++;
                last_letter = 1;
                if(word_col == 0) word_col = col;

            } else {
                if(last_letter == 1){
                    temp[start] = '\0';
                    if(ispunct(temp[last_real_char])) {
                        temp[last_real_char] = '\0';  // Remove last punctuation before checking spelling
                    } else {
                        temp[last_real_char + 1] = '\0';  // Properly null-terminate after the last valid character
                    }
                    int one_if_wrong = spelling(temp, diction);
                    if(one_if_wrong == 1){
                        printf(" %s (%d,%d): %s \n", file, row, word_col, temp);
                    }
                    memset(temp, 0, sizeof(temp));  // Clear the temp array
                    start = 0;
                    word_col = 0;
                    last_letter = 0;
                    brackets = 0;
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

    if(last_letter == 1){ // handle the last word if it was not followed by a whitespace or punctuation
        temp[start] = '\0';
        if(ispunct(temp[last_real_char])) {
            temp[last_real_char] = '\0';  // Remove last punctuation before checking spelling
        } else {
            temp[last_real_char + 1] = '\0';  // Properly null-terminate the string
        }
        int one_if_wrong = spelling(temp, diction);
        if(one_if_wrong == 1){
            printf(" %s (%d,%d): %s \n", file, row, word_col, temp);
        }
        memset(temp, 0, sizeof(temp));  // Clear the temp array
        start = 0;
        word_col = 0;
        last_letter = 0;
        brackets = 0;
    }

    if (bytes_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(fd);
    return;
}
