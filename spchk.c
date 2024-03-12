#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: Missing a Dictionary. \n");
        return 1;
    }

    int word_count = 0;
    char **dictionary = read_dictionary(argv[1], &word_count);

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
