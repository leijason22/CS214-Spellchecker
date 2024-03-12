#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_WORD_LENGTH 100

//1. find and open all specified files, also performs directory traversal
int processFiles(int argc, char **argv, int *file_descriptors);

//2. reading the file and generating a sequence of position-annotated words
void generateWords(int file_descriptor);

//3. checking whether a word is contained in the dictionary
int isValidWord(char *dictionary[], int dictionary_size, char *word);

int main(int argc, char **argv) {

    char *dictionary[] = {"hello", "world", "example", "dictionary", "macDonald"}; //just to test

    int file_descriptors[argc - 2]; // Assuming the maximum number of files is argc - 2

    int num_files = processFiles(argc, argv, file_descriptors);

    if (num_files == -1) {
        // Using write() for error message
        write(STDERR_FILENO, "Error: Unable to open one or more files.\n", strlen("Error: Unable to open one or more files.\n"));
        return EXIT_FAILURE;
    }

    // Iterate through each file descriptor
    for (int i = 0; i < num_files; i++) {
        generateWords(file_descriptors[i]);
        close(file_descriptors[i]); // Using close() instead of fclose()
    }

    return EXIT_SUCCESS;
}


//find and open all specified files, also performs directory traversal
//file descriptors are integers
//0 is standard input
//1 is standard output
//2 is standard error
/*
fileno() returns the file descriptor used by a FILE
fdopen() creates a FILE for a file descriptor

ssize_t read(int fd, void *buffer, size_t n);
asks OS to write up to n bytes into the array at buffer
returns the number of bytes read
returns 0 at end of file
returns -1 for error conditions

int open(char *pathname, int flags);
int open(char *pathname, int flags, mode_t mode);

flags must include exactly one of :
O_RDONLY - open file in read mode
O_WRONLY - open file in write mode
O_RDRW - open file in read/write mode

pathname: indicates how to find the file we want to open

we can combine with additional flags using | (bitwise or)
O_CREAT - create file if it does not exist (requires mode)
O_EXCL - fail if the file already exists
O_TRUNC - set the file length to 0, if it exists
O_APPEND - all writes go to the end of the file
O_NONBLOCK - open file in non-blocking mode (doesn't matter much for
disk files)


*/

//1. find and open all specified files, also performs directory traversal
//first argument to spchk is a path to a dictionary file

//subsequent arguments will be paths to text files or directories
//int fd = open("foo.txt", O_RDONLY);
//if (fd < 0) {//report error or something}
int processFiles(int argc, char **argv, int *file_descriptors) {
    // int num_files = 0; //may not need

    for (int i = 2; i < argc; i++) {
        // Ignore files or directories starting with "."
        if (argv[i][0] == '.') {
            //check statement
            printf("Ignore: %s\n", argv[i]); 
            continue;
        }

        int file = open(argv[i], O_RDONLY);

        //case where open() fails
        if (file < 0) {
            perror("Error: Unable to open file.");
            return EXIT_FAILURE; //could also do exit(EXIT_FAILURE);
        }

        // file_descriptors[num_files++] = file; //may not need
    }

    // return num_files;
    return EXIT_SUCCESS;
}

void generateWords(int file_descriptor) {
    char line[MAX_WORD_LENGTH];
    int line_number = 0;

    while (read(file_descriptor, line, MAX_WORD_LENGTH) > 0) {
        ++line_number;

        char *token = strtok(line, " \t\n"); // Split line into words

        while (token != NULL) {
            // Process each word (token)
            // For simplicity, let's print each word
            printf("Word: %s, Line: %d\n", token, line_number);

            token = strtok(NULL, " \t\n");
        }
    }
}

int isValidWord(char *dictionary[], int dictionary_size, char *word) {
// Implement binary search on the dictionary array
    // Return 1 if the word is in the dictionary, 0 otherwise
    // Consider variations in capitalization and hyphenated words
    // For simplicity, let's assume a case-sensitive search for now
    // Return 1 for demonstration purposes
    int low = 0;
    int high = dictionary_size - 1;

    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp_result = strcmp(dictionary[mid], word);

        if (cmp_result == 0) {
            // Found the word in the dictionary
            return 1;
        } else if (cmp_result < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    // Word not found in the dictionary
    return 0;   
}




