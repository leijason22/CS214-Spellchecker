README.txt

Systems Programming - Project 2: Spelling Checker
Nelson Li, njl117
Jason Lei, jjl330

Program Overview
This program is a spell-checking program that consists of two main source files that manage different aspects of the project:
spchk.c - the main driver file that handles finding and opening all specified files, as well as (recursive) directory traversal.
dictionary.c - a file that handles the creation and population of a dictionary data structure and performs the traversal (line-by-line) of a given text file, while performing spell-checking and reporting spell-checking errors.
Requirements: 

1. Finding and opening all the specified files, including directory traversal
2. Reading the file and generating a sequence of position-annotated words
3. Checking whether a word is contained in the dictionary

Special Notes 
**To avoid problems with common sentence punctuation, we ignore punctuation marks occurring at the end of a word. Similarly, we ignore quotation marks (’ and ") and brackets (( and [ and {) at the start of a word.
For these cases, we ignored all brackets and quotations at the beginning and removed any non-characters from after the end of the last letter.
**This program is designed to work with input that consists of .txt files and a newline-separated dictionary file.

Design Notes
spchk.c
- Handles finding and opening all specified files and directories, and is responsible for directory traversal to find input files given a path using the traverseFiles function.
- Calls dictionary populating functions and functions that traverse and spell-check input text files from the dictionary.c library.
- traverseFiles: recursively traverses directories, identifying and storing paths of text files. Also ignores and skips over directories that begin with “.” or “..”

dictionary.c
- The main body of code which holds almost all the functions. It contains the body for all the functions used in spchk.c: find_length, make_dict, spelling, traverse. 
- find_length: reads the dictionary file and returns an accurate count of lines 
- make_dict: populates a dictionary array with dictionary words from an input file
- spelling: performs spell checking on a given word with the provided dictionary txt file, checks for variations of capitalization (dependent on word given in dictionary file)
    - Handles Special Text Case(s):
        - Capitalization: handles case sensitivity for words of regular capitalization, initial capitalization, all capitalization, as well as special cases like “MacDonald” and acronyms.
- traverse: reads through an input text file, performing spell-checking on each word
    - Handles Special Text Case(s):
        - Trailing and Leading Punctuation
        - Hyphenated Words
- Error handling is performed throughout, reporting appropriate error messages with their respective positions and locations in the respective text files. 

dictionary.h
- Library header to be able to include into files, and use functions from dictionary.c

Test Plan Details
Main Method
- Create a char** for the given dictionary to be able to traverse by finding the number of lines in the given path, mallocing data and populating the dictionary array. 
- Read the file and when it hits a character, it starts processing it in a temporary array, when the last character is read, it processes the temporary word into the spell check function, and returns 1 if it is wrong, and 0 if correct. This all happens while keeping track of the row, and column that the word started on. 
Hyphens, commas, and apostrophes
- We consider hyphenated words as two separate words, and spell check them and return the part of the hyphenated word that would be wrong
- Our code basically stores the word into an array, by identifying if it is a character and if the word is still going on skipping any punctuations after or before the word
- We consider apostrophes as proper part of the word since the dictionary contain a lot of possessive versions of the word

Test Scenarios and Test Cases
When testing, we used the dictionary/filepath from the iLab machines: /usr/share/dict/words
However, feel free to use your own dictionary if you so desire!

1. Basic Spelling Error Testing - basic, single txt file with spelling errors: test.txt
2. Directory Testing - subdir and subdir2 are both directories, the program will execute files that end in “.txt”, but ignoring files beginning with “.” and “..”: test2.txt, test3.txt, test5.txt, test6.txt 
3. Directory Traversal (directory in a directory) Testing - subsubdir is a directory in subdir2, if subsubdir is called, the program will execute all txt files in the directory. If subdir2 is called, the program will execute all txt files in subdir2 and subsubdir: test4.txt
4. Capitalization Variations Testing - checks different variations of capitalization, regular, initial capitalization, full capitalization, unconventional capitalization, etc.: test2.txt
5. Punctuation Handling Testing- checks if trailing and leading punctuation is ignored properly: test3.txt
6. Hyphenation Testing - checks words with hyphens: test4.txt
7. Whitespace Testing - checks a file with lines that contain whitespaces: test5.txt 
8. Extra, Special Cases Testing - checks special cases such as “/"bob"/42” and cases that contain flagged punctuation that should be ignored/flagged properly: test6.txt 
