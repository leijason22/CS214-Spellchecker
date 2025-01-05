#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PATH_MAX 4096
#define MAX_TXT_FILES 100

//finding and opening all specified files, also directory traversal
/*
- when spchk is given a directory name as an argument, it will perform a recursive directory traversal
and check spelling in all files whose names end with ".txt", but ignoring any files or directories
whose names begin with "."

- do not need spchk to check the files in any particular order, but all specified files must be checked
- note: the requirements to ignore files beginning with "." and not ending with ".txt" ONLY FOR
directory traversal, not to files in arg list

main test dict:
/usr/share/dict/words 
*/

char textFiles[MAX_TXT_FILES][PATH_MAX]; // Assuming a maximum of 100 text files with max path length of 255

int textFileCount = 0;

void traverseFiles(char *dir) {
    DIR *dp;
    struct dirent *entry;

    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        // Skip entries that start with '.'
        if (entry->d_name[0] == '.') {
            continue;
        }

        // Construct the full path of the entry
        char fullPath[PATH_MAX];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, entry->d_name);

        // Check if the entry is a directory or a file ending with ".txt"
        struct stat path_stat;
        stat(fullPath, &path_stat);
        if (S_ISDIR(path_stat.st_mode)) {
            // Recursively traverse directories
            traverseFiles(fullPath);
        } else {
            // Check specifically if the file ends with ".txt"
            const char *ext = strrchr(entry->d_name, '.');
            if (ext != NULL && strcmp(ext, ".txt") == 0) {
                // File ends with ".txt", add it to the list
                snprintf(textFiles[textFileCount], sizeof(textFiles[textFileCount]), "%s", fullPath);
                if (textFileCount < MAX_TXT_FILES - 1) {
                    textFileCount++;
                }
            }
        }
    }

    closedir(dp);
}


// void traverseFiles(char *dir) {
//     DIR *dp;
//     struct dirent *entry;

//     // Open the directory
//     if ((dp = opendir(dir)) == NULL) {
//         fprintf(stderr, "Cannot open directory: %s\n", dir);
//         return;
//     }

//     // Traverse each entry in the directory
//     while ((entry = readdir(dp)) != NULL) {
//         // Skip "." and ".." directories
//         if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
//             continue;

//         // Construct the full path of the entry
//         char fullPath[PATH_MAX];
//         snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, entry->d_name);

//         // Check if the entry is a directory
//         if (entry->d_type == DT_DIR) {
//             // Recursively traverse subdirectories
//             traverseFiles(fullPath);
//         } else {
//             // Check if the file is a text file
//             if (strstr(entry->d_name, ".txt") != NULL) {
//                 snprintf(textFiles[textFileCount], sizeof(textFiles[textFileCount]), "%s", fullPath);
//                 textFileCount++;
//             }
//         }
//     }

//     // Close the directory
//     closedir(dp);
// }

int main(int argc, char *argv[]) {

    //checks if dictionary file is provided
    if (argc < 2) {
        fprintf(stderr, "Error: Missing a Dictionary. \n");
        return 1;
    }

    //checks if test files are provided, need at least one
    if (argc < 3) {
         fprintf(stderr, "Error: Please provide file(s) to test. \n");
         exit(EXIT_FAILURE);
    }

    // int a = 0;
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    int lines = find_length(fd);

    make_dict(fd, lines);

    for (int i = 2; i < argc; i++) {
        struct stat path_stat;
        if (stat(argv[i], &path_stat) == 0) {
            if (S_ISDIR(path_stat.st_mode)) {
                // if it's a directory, traverse it
                traverseFiles(argv[i]);
            } else if (S_ISREG(path_stat.st_mode)) {
                // if it's a regular file, process it (dictionary.c traverse)
                traverse(argv[i], dictionary_array);
            } else {
                fprintf(stderr, "not a directory or a regular file: %s\n", argv[i]);
            }
        } else {
            perror(argv[i]);
        }
    }

    // Print the paths of all text files
    for (int i = 0; i < textFileCount; i++) {
        traverse((textFiles[i]), dictionary_array); 
    }

    for (int i = 0; i < lines; i++) {
        free(dictionary_array[i]);
    }

     free(dictionary_array);
     close(fd);
     return 0;
}