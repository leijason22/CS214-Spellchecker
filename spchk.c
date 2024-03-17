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

//finding and opening all specified files, also directory traversal
/*
- when spchk is given a directory name as an argument, it will perform a recursive directory traversal
and check spelling in all files whose names end with ".txt", but ignoring any files or directories
whose names begin with "."

- do not need spchk to check the files in any particular order, but all specified files must be checked
- note: the requirements to ignore files beginning with "." and not ending with ".txt" ONLY FOR
directory traversal, not to files in arg list

*/

#define MAX_PATH_LENGTH 1024

void dirTraversal(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    char fullPath[MAX_PATH_LENGTH];

    while ((entry = readdir(dir)) != NULL) {
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            //skip current and parent directories
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            //recursively traverse subdirectories
            dirTraversal(fullPath);
        } else if (entry->d_type == DT_REG) {
            //if file ends in ".txt" and does not start with '.'
            char *ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".txt") == 0 && entry->d_name[0] != '.') {
                printf("Text file found: %s\n", fullPath);
            }
        }
    }

    closedir(dir);

}


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

    //test to print out the dictionary array + count
    // for (int i = 0; dictionary_array[i] != NULL; i++) {
    //     printf("%s\n", dictionary_array[i]);
    //     //printf("%s\n", dictionary_array[i]);
    //     a++; 
    // }
    // printf("%d \n", a);

   // printf("%d \n", a);


    traverse(argv[2], dictionary_array);

    //decides if argv[x] is a file or directory
    struct stat path_stat;
    for (int i = 2; i < argc; i++) {
        if (stat(argv[i], &path_stat) == -1) {
            perror("stat failed");
            continue;
        }

        if (S_ISREG(path_stat.st_mode)) {
            printf("regular file: %s\n", argv[i]); //regular file found
        } else if (S_ISDIR(path_stat.st_mode)) {
            printf("directory: %s\n", argv[i]); //directory found
            dirTraversal(argv[i]);
        } else {
            fprintf(stderr, "%s is not a regular file or directory\n", argv[i]);
        }
    }

    for (int i = 0; i < lines; i++) {
        free(dictionary_array[i]);
    }

     free(dictionary_array);
     close(fd);
     return 0;
}
