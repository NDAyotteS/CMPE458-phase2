/* parser.h */
#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"

// Basic node types for AST
typedef enum {
    AST_PROGRAM,        // Program node
    AST_VARDECL,        // Variable declaration (int x)
    AST_ASSIGN,         // Assignment (x = 5)
    AST_PRINT,          // Print statement
    AST_NUMBER,         // Number literal
    AST_IDENTIFIER,     // Variable name
    // Control Flow node types
    AST_IF,             //If statement
    AST_ELSE,           //Else statement
    AST_WHILE,          //while loop
    AST_FOR,            //for loop
    AST_REPEAT_UNTIL,   //repeat until loop
    AST_BREAK,          //break statement
    // Function node types
    AST_FUNCTION_DECL, //function declaration
    AST_FUNCTION_CALL, //function call
    AST_RETURN,        //return statement
    AST_BLOCK,         //block {...}
    AST_PARAM_LIST,    //function parameters
    //Expressions
    AST_BINOP,         // Binary operators
    AST_UNARYOP,       // Unary operators (i think just !)
    AST_COMPARISON,    // Comparisons
    AST_LOGIC_OP,      // Logical operators
    AST_CAST,           // typecasting, not sure if were doing this
    //Extra
    AST_NULL,           // null values
    // TODO: Add more node types as needed
} ASTNodeType;

typedef enum {
    PARSE_ERROR_NONE,
    PARSE_ERROR_UNEXPECTED_TOKEN,
    PARSE_ERROR_MISSING_SEMICOLON,
    PARSE_ERROR_MISSING_IDENTIFIER,
    PARSE_ERROR_MISSING_EQUALS,
    PARSE_ERROR_INVALID_EXPRESSION
} ParseError;

// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;           // Type of node
    Token token;               // Token associated with this node
    struct ASTNode* left;      // Left child
    struct ASTNode* right;     // Right child
    // TODO: Add more fields if needed
} ASTNode;

// Parser functions
void parser_init(const char* input);
ASTNode* parse(void);
void print_ast(ASTNode* node, int level);
void free_ast(ASTNode* node);

#endif /* PARSER_H */