#include <string.h>
#include "keywords.h"

const char* keywords[NUM_KEYWORDS] = {
    "if", "else",
    "while", "for", "until", "break",
    "print", "read",
    "int", "float", "char", "bool", "string", "void",
    "func",
    "null", "true", "false"
};

// Function to check if a given token is a keyword
int iskeyword(const char* token) {
    for (int i = 0; i < NUM_KEYWORDS; i++) {
        if (strcmp(token, keywords[i]) == 0) {
            return 1;  // It's a keyword
        }
    }
    return 0;  // Not a keyword
}
