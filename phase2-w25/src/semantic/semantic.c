/* semantic.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../include/parser.h"
#include "../../include/lexer.h"
#include "../../include/tokens.h"
#include "../../include/semantic.h"

// TODO: Step 2: Implement Symbol Table Operations! This is very important
// TODO: Step 3: Implement Semantic Analysis Functions
// TODO: Step 4: Semantic Error Reporting

int main() {
    // get file
    FILE *file = fopen("../phase2-w25/test/input_valid.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // get buffer size based on file size for chars
    char *buffer = malloc(file_size + 1);
    if (!buffer) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    // fill buffer with full file of chars in order
    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';
    size_t b = 0;
    for (size_t i = 0; i < bytes_read; i++) {
        if (buffer[i] != '\r') {
            buffer[b++] = buffer[i];
        }
    }
    buffer[b] = '\0';

    // TODO: MAKE THE MAIN FUNCTION CALL PARSER TO CREATE A TREE


    free(buffer);
    fclose(file);
    return 0;
}
