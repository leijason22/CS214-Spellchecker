// dictionary.c
#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

char **dict_array;
int numlines;
int wordCount;

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFLENGTH
#define BUFLENGTH 16
#endif

int find_dictlength(int fd) {
    int result = 0;
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
				// use_line(arg, buf + line_start);
				line_start = pos + 1;
                result += 1;
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
			if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
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
		// use_line(arg, buf + line_start);
		result++;
	}
	free(buf);
	return result;
}


//currently there is an infinite loop from the lseek at the end of the read
char **read_dictionary(int fd, int *word_count) {
	
    // int fd = open(path, O_RDONLY);
    // if (fd < 0) {
    //     perror(path);
    //     exit(EXIT_FAILURE);
    // }

    // char **words = (char **)malloc(INITIAL_ARRAY_SIZE * sizeof(char *));
	numlines = find_dictlength(fd);
	// printf("numlines (line 90): %d\n", numlines);
	
	// printf("Numlines: %d\n", numlines);

	dict_array = (char **) malloc(numlines * sizeof(char *));
    if (dict_array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        close(fd);
        return NULL;
    }

	//this is a cool posix function that lets you set the read position back to the beginning
	lseek(fd, 0, SEEK_SET);
	


	//will simply do buffer size to max word length 
	//we can possibly find the max word length when we run dictLength 
	char buffer[MAX_WORD_LENGTH];
    int count = 0;
    ssize_t bytes_read;

    while ((bytes_read = (read(fd, buffer, MAX_WORD_LENGTH - 1))) > 0) {
		// printf("Bytes Read: %d\n", bytes_read);
		buffer[bytes_read] = '\0';
		int newline_pos = -1;

        for (int i = 0; i < bytes_read; ++i) {
            if (buffer[i] == '\n') {
                newline_pos = i;
                break;
            }
        }

		if (newline_pos != -1){
			buffer[newline_pos] = '\0';
			dict_array[count] = (char *)malloc((newline_pos + 1) * sizeof(char));
			if (dict_array[count] == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                for (int j = 0; j < count; ++j) {
                    free(dict_array[j]);
					close(fd);
					return dict_array;
                }
			}
			strcpy(dict_array[count], buffer);

		if (DEBUG){
			printf("Most Recently Read word: %s \n", dict_array[count]);
		}

			count++;
		}


		//edge case when EOF is reached
		else if (bytes_read > 0){
			for (int i = 0; i < bytes_read+1; ++i) {
            	if (buffer[i] == '\0') {
                	newline_pos = i;
                	break;
            	}
        	}
			buffer[newline_pos] = '\0';
			dict_array[count] = (char *)malloc((newline_pos + 1) * sizeof(char));
			if (dict_array[count] == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                
                for (int j = 0; j < count; ++j) {
                    free(dict_array[j]);
					close(fd);
					return dict_array;
                }
			}

			strcpy(dict_array[count], buffer);

		if (DEBUG){
			printf("Most Recently Read word: %s \n", dict_array[count]);
		}
			count++;
		}

		off_t current_pos = lseek(fd, 0, SEEK_CUR);  // Get current position
		off_t seek_pos = current_pos - (bytes_read - newline_pos - 1);
		lseek(fd, seek_pos, SEEK_SET);
	}

	// close(fd);
    *word_count = count;
	wordCount = count;
    return dict_array;
}

int binary_search(int dict_size, char **dict, char *target) {
	int lo = 0;
	int hi = dict_size - 1;

	while (lo <= hi) {
		int mid = lo + (hi - lo) / 2;
		int comparison = strcmp(dict[mid], target);

		if (comparison == 0) {
			return mid;
		} else if (comparison < 0) {
			lo = mid + 1;
		}
		else {
			hi = mid - 1;
		}
	}
	return -1;
}