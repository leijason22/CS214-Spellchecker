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

    
    int a = 0;
    int f = open(argv[1], O_RDONLY);
    int lines = find_length(f);

    make_dict(f, lines);

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
