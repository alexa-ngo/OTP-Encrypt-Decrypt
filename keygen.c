#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LETTER_A 65
#define LETTER_Z 90
#define OFFSET_OF_LETTER_A 65

/*
*     This keygen program creates a key file of a specific length.
*         Usage: ./keygen key_length
*
*    The last character keygen outputs should be a new line.
*    Any error text must be output to stderr.
*/

int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Example usage: ./keygen key_length\n");
        exit(1);
    }

    // Get the user input
    const int key_length = atoi(argv[1]);

    // Allocate memory to store the result
    char* result_str = malloc(sizeof(char) * key_length + 1);

    // Generate the number of random letters based on the user input
    for (int i = 0; i < key_length; i++) {
        int rand_int = rand() % (LETTER_Z - LETTER_A + 1) + OFFSET_OF_LETTER_A;
        result_str[i] = (char)rand_int;
    }

    result_str[key_length] = '\n';

    for (int i = 0; i < key_length; i++) {
        printf("%c", (char)result_str[i]);
    }

    printf("\n");
    free(result_str);

    return 0;
}
