
/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

/* Token types that need to be recognized by the lexer */
typedef enum {
    TOKEN_EOF,
    TOKEN_NUMBER,           // e.g. 123
    TOKEN_OPERATOR,         // e.g. + - * / %
    TOKEN_EQUALS,           // e.g. += -= =
    TOKEN_COMPARITIVE,      // e.g. == !=
    TOKEN_IF, TOKEN_ELSE,                                         // Conditionals
    TOKEN_WHILE, TOKEN_FOR, TOKEN_UNTIL, TOKEN_BREAK,             // Looping
    TOKEN_PRINT,                                                  // Output
    TOKEN_INT, TOKEN_FLOAT, TOKEN_CHAR, TOKEN_BOOL, TOKEN_STRING, // Var Types
    TOKEN_VOID, TOKEN_FUNC,                                       // Functions
    TOKEN_NULL, TOKEN_TRUE, TOKEN_FALSE,                          // Logical Assignments
    TOKEN_IDENTIFIER,       // Any identifiers
    TOKEN_STRING_LITERAL,   // e.g. "SeaPlus+"
    TOKEN_CHAR_LITERAL,     // e.g. 'c'
    TOKEN_DELIMITER,        // e.g. {} [] ()
    TOKEN_SEMICOLON,        // e.g. ;
    TOKEN_SPECIAL_CHARACTER,// e.g. _ &
    TOKEN_ERROR             // e.g. ERROR_INVALID_CHAR
} TokenType;

/* Error types for lexical analysis */
typedef enum {
    ERROR_NONE,
    ERROR_INVALID_CHAR,
    ERROR_INVALID_NUMBER,
    ERROR_CONSECUTIVE_OPERATORS,
    ERROR_STRING_OVERFLOW,
    ERROR_UNTERMINATED_STRING,
    ERROR_INVALID_ESCAPE_CHARACTER,
    ERROR_UNTERMINATED_CHARACTER,
    ERROR_OPEN_DELIMITER
} ErrorType;

/* Token structure to store token information */
typedef struct {
    TokenType type;
    char lexeme[100];   // Actual text of the token
    int line;           // Line number in source file
    ErrorType error;    // Error type if any
} Token;

#endif /* TOKENS_H */
