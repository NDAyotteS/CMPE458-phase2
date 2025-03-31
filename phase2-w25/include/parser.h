/* parser.h */
#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"

// Basic node types for AST
typedef enum {
    AST_PROGRAM,        // Program node
    AST_ASSIGN,         // Assignment (x = 5)
    AST_PRINT,          // Print statement
    AST_NUMBER,         // Number literal
    AST_IDENTIFIER,     // Variable name
    AST_INT,            // Integers
    AST_STRINGCHAR,     // String or Character
    // Control Flow node types
    AST_IF,             //If statement
    AST_ELSE,           //Else statement
    AST_WHILE,          //while loop
    AST_REPEAT,         //repeat...
    AST_UNTIL,          //until loop
    AST_BREAK,          //break statement
    // Block statements for loops
    AST_BLOCK,         //block {...}
    //Expressions
    AST_EXPRESSION,    // Unresolved state for AST nodes in expression parser
    AST_BINOP,         // Binary operators
    AST_UNARYOP,       // Unary operators (i think just !)
    AST_COMPARISON,    // Comparisons
    AST_LOGIC_OP,      // Logical operators
    AST_CAST,          // typecasting, not sure if were doing this
    //Extra
    AST_NULL,          // null values
    AST_FACTORIAL      // factorial
    // TODO: Add more node types as needed
} ASTNodeType;

typedef enum {
    PARSE_ERROR_NONE,
    PARSE_ERROR_UNEXPECTED_TOKEN,
    PARSE_ERROR_UNEXPECTED_EOF,
    PARSE_ERROR_UNEXPECTED_OPERATOR,
    PARSE_ERROR_MISSING_SEMICOLON,
    PARSE_ERROR_MISSING_IDENTIFIER,
    PARSE_ERROR_MISSING_EQUALS,
    PARSE_ERROR_INVALID_EXPRESSION,
    PARSE_ERROR_MISSING_PAREN,
    PARSE_ERROR_MISSING_CONDITION,
    PARSE_ERROR_MISSING_BRACE,
    PARSE_ERROR_MISSING_COLON,
    PARSE_ERROR_FUNC_CALL,
    PARSE_ERROR_BREAK_OUTSIDE_LOOP,
    PARSE_ERROR_INVALID_CONDITION,
} ParseError;



// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;           // Type of node
    Token token;               // Token associated with this node
    struct ASTNode* left;      // Left child
    struct ASTNode* right;     // Right child
} ASTNode;

// Parser functions
void parser_init(const char* input);
ASTNode* parse(void);
void print_ast(ASTNode* node, int level);
void free_ast(ASTNode* node);

#endif /* PARSER_H */