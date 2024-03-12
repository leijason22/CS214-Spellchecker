#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/*
this implementation of dictionary reading involves mallocing an array of size 104335 (max num of words of dictionary file in
/usr/share/dict/words)

not sure if we have to use that as dictionary but its there

dictionary reading is complete, not extensively tested other than just printing it all out
this implementation mallocs every NEW WORD read and sets buffer size to MAX_WORD_LENGTH

this is stored in the dictinoary malloced array - char** dictinoary

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

//this is chat gptd hahah, need to check and modify prob
int find_length(int file_descriptor) {
    int line_count = 0;
    char buffer[MAX_WORD_LENGTH];

    ssize_t read_bytes;
    while ((read_bytes = read(file_descriptor, buffer, MAX_WORD_LENGTH - 1)) > 0) {
        buffer[read_bytes] = '\0'; // null-terminate the buffer
        char *newline_position = strchr(buffer, '\n');
        while (newline_position != NULL) {
            line_count++;
            newline_position = strchr(newline_position + 1, '\n');
        }
    }

    //if last line is not counted (does not end with newline)
    if (line_count > 0 && buffer[read_bytes - 1] != '\n') {
        line_count++;
    }

    //resets the file pointer to the beginning of the file
    lseek(file_descriptor, 0, SEEK_SET);

    return line_count;
}

char **read_dictionary(const char *path, int *word_count) {
    int file_descriptor = open(path, O_RDONLY);
    if (file_descriptor < 0) {
        fprintf(stderr, "Error: Can't open dictionary\n");
        return NULL;
    }

    num_lines = find_length(file_descriptor);

    dictionary_array = (char **)malloc(num_lines * sizeof(char *));

    if (dictionary_array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        close(file_descriptor);
        return NULL;
    }

    char buffer[MAX_WORD_LENGTH];
    int count = 0;

    ssize_t read_bytes;
    while ((read_bytes = read(file_descriptor, buffer, MAX_WORD_LENGTH - 1)) > 0) {
        buffer[read_bytes] = '\0'; // null-terminate the buffer
        char *newline_position = strchr(buffer, '\n');
        if (newline_position != NULL) {
            *newline_position = '\0'; // Remove the newline character
        }

        dictionary_array[count] = allocate_and_copy(buffer);

        //check if malloc fails
        if (dictionary_array[count] == NULL) {
            fprintf(stderr, "Memory allocation for word failed\n");
            close(file_descriptor);
            for (int i = 0; i < count; i++) {
                free(dictionary_array[i]);
            }
            free(dictionary_array);
            return NULL;
        }
        count++;
    }

    close(file_descriptor);
    *word_count = count;
    return dictionary_array;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: Missing a Dictionary. \n");
        return 1;
    }

    int word_count = 0;
    char **dictionary = read_dictionary(argv[1], &word_count);

    if (dictionary == NULL) {
        fprintf(stderr, "Error: Failed to read the dictionary.\n");
        return 1;
    }

    printf("Total words read: %d\n", word_count);
    for (int i = 0; i < word_count && i < 10; i++) {
        printf("%s\n", dictionary[i]);
    }

    for (int i = 0; i < word_count; i++) {
        free(dictionary[i]);
    }

    free(dictionary);
    return 0;
}
