#include "dictionary.h"
#include "parse_spchk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

//might need to incorporate traversing files in here?

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: Missing a Dictionary. \n");
        return 1;
    }

    int word_count = 0;
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    char **dictionary = read_dictionary(fd, &word_count);

    printf("Total words read: %d\n", word_count);
    for (int i = 0; i < word_count && i < 10; i++) { //only prints out first 10 to limit
        printf("%s\n", dictionary[i]);
    }

    for (int i = 0; i < word_count; i++) {
        free(dictionary[i]);
    }

    free(dictionary);
    return 0;
}