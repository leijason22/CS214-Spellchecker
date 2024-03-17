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

void dirTraversal(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }



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

    int a = 0;
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

    for (int i = 0; i < lines; i++) {
        free(dictionary_array[i]);
    }

     free(dictionary_array);
     return 0;
}
