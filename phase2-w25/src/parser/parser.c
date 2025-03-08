/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../include/parser.h"
#include "../../include/lexer.h"
#include "../../include/tokens.h"
#include "../../include/operators.h"

// TODO 1: Add more parsing function declarations for:
// - if statements: if (condition) { ... }
// - while loops: while (condition) { ... }
// - repeat-until: repeat { ... } until (condition)
// - print statements: print x;
// - blocks: { statement1; statement2; }
// - factorial function: factorial(x)

// Current token being processed
static Token current_token;
static int position = 0;
static const char *source;
static const int OPERATOR_TOKEN_MAX = 128; //arbitrary

static void parse_error(ParseError error, Token token) {
    // TODO 2: Add more error types for:
    // - Missing parentheses
    // - Missing condition
    // - Missing block braces
    // - Invalid operator
    // - Function call errors

    printf("Parse Error at line %d: ", token.line);
    switch (error) {
        case PARSE_ERROR_UNEXPECTED_TOKEN:
            printf("Unexpected token '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_SEMICOLON:
            printf("Missing semicolon after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_IDENTIFIER:
            printf("Expected identifier after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_EQUALS:
            printf("Expected '=' after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_INVALID_EXPRESSION:
            printf("Invalid expression after '%s'\n", token.lexeme);
            break;
        default:
            printf("Unknown error\n");
    }
}

// Get next token
static void advance(void) {
    current_token = get_next_token(source, &position);
}

// Create a new AST node
static ASTNode *create_node(ASTNodeType type) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node) {
        node->type = type;
        node->token = current_token;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

// Match current token with expected type
static int match(TokenType type) {
    return current_token.type == type;
}

// Expect a token type or error
// Globally advances on success
static void expect(TokenType type) {
    if (match(type)) {
        advance();
    } else {
        parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
        exit(1); // Or implement error recovery
    }
}

// Forward declarations for functions
static ASTNode *parse_statement(void);

static ASTNode *parse_expression(void);

static ASTNode *parse_assignment(void);

static ASTNode* parse_block_statement(void);

// TODO 3: COMPLETE parsing functions for each new statement type

// Parses if(), if else(), and else() statements
static ASTNode* parse_if_statement(void) {

}

// Parses while loop statements
static ASTNode* parse_while_statement(void) {

}

// Parses for loop statements
static ASTNode* parse_for_statement(void) {

}

// Parses until loop statements
static ASTNode* parse_until_statement(void) {

}

// Parses print statements
static ASTNode* parse_print_statement(void) {

}

// Parses function declarations, forward declarations, and calls
static ASTNode* parse_func_statement(void) {

}

// Parses block statements (the { ... } inside of a function, if statement, loop, etc)
static ASTNode* parse_block_statement(void) {

}

// Parse variable declaration: e.g. int x;
static ASTNode *parse_declaration(void) {
    ASTNode *node = create_node(AST_VARDECL);
    advance(); // consume data-type

    if (!match(TOKEN_IDENTIFIER)) {
        parse_error(PARSE_ERROR_MISSING_IDENTIFIER, current_token);
        exit(1);
    }

    node->token = current_token;
    advance();

    // Correct case
    if(match(TOKEN_SEMICOLON)) {
        advance();
        return node;
    }
    // TODO: ALLOW FOR DECLARATION ASSIGNMENTS???

    // Failed case
    parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
    exit(1);

}

// Parse assignment: e.g. x = 5; or x = 'yippee';
static ASTNode *parse_assignment(void) {
    ASTNode *node = create_node(AST_ASSIGN);
    node->left = create_node(AST_IDENTIFIER);
    node->left->token = current_token;
    advance();

    if (!match(TOKEN_EQUALS)) {
        parse_error(PARSE_ERROR_MISSING_EQUALS, current_token);
        exit(1);
    }
    advance();

    // For the case where the assignment is for strings, chars, or null values
    // TODO: RUN TESTS FOR STRING DECLARATION
    if(match(TOKEN_STRING) || match(TOKEN_CHAR)) {
        node->right = create_node(AST_STRINGCHAR);
        node->right->token = current_token;
        advance();
    } else if(match(TOKEN_NULL)) {
            node->right = create_node(AST_NULL);
            node->right->token = current_token;
            advance();
    } else { // All other assignment types
        node->right = parse_expression();
    }

    // Parse_expression() and string assignment both advance, check that statement ended
    if (!match(TOKEN_SEMICOLON)) {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        exit(1);
    }
    advance();
    return node;
}

// Parse statement
static ASTNode *parse_statement(void) {
    if (match(TOKEN_INT) || match(TOKEN_CHAR) || match(TOKEN_STRING)) return parse_declaration();
    if (match(TOKEN_IDENTIFIER)) return parse_assignment();
    if (match(TOKEN_IF)) return parse_if_statement();
    if (match(TOKEN_WHILE)) return parse_while_statement();
    if (match(TOKEN_FOR)) return parse_for_statement();
    if (match(TOKEN_UNTIL)) return parse_until_statement();
    if (match(TOKEN_PRINT)) return parse_print_statement();
    if (match(TOKEN_FUNC)) return parse_func_statement();

    printf("Syntax Error: Unexpected token\n");
    exit(1);
}

// Parse expression (currently only handles numbers and identifiers)

// TODO 5: Implement expression parsing
// Current expression parsing is basic. Need to implement:
// - Binary operations (+-*/)
// - Comparison operators (<, >, ==, etc.)
// - Operator precedence
// - Parentheses grouping
// - Function calls


int getOperatorRelationship(operatorCode_t TOS, operatorCode_t Lookahead){
    return operatorParseTable[TOS][Lookahead];
}

operatorCode_t getOperatorCode(char* operatorString){
    //no switch statements on strings in C.
    if(strcmp(operatorString, "!") == 0){
        return OPCODE_NOT;
    } else if(strcmp(operatorString, "$") == 0){
        return OPCODE_FACTORIAL;
    } else if(strcmp(operatorString, "^^") == 0){
        return OPCODE_POWER;
    } else if(strcmp(operatorString, "*") == 0){
        return OPCODE_MULTIPLY;
    } else if(strcmp(operatorString, "/") == 0){
        return OPCODE_DIVIDE;
    } else if(strcmp(operatorString, "%") == 0){
        return OPCODE_MOD;
    } else if(strcmp(operatorString, "+") == 0){
        return OPCODE_ADD;
    } else if(strcmp(operatorString, "-") == 0){
        return OPCODE_SUB;
    } else if(strcmp(operatorString, ">") == 0){
        return OPCODE_GREATER;
    } else if(strcmp(operatorString, ">=") == 0){
        return OPCODE_GREAT_EQ;
    } else if(strcmp(operatorString, "<=") == 0){
        return OPCODE_LESS_EQ;
    } else if(strcmp(operatorString, "<") == 0){
        return OPCODE_LESSER;
    } else if(strcmp(operatorString, "==") == 0){
        return OPCODE_LOG_EQ;
    } else if(strcmp(operatorString, "!=") == 0){
        return OPCODE_NOT_EQ;
    } else if(strcmp(operatorString, "&&") == 0){
        return OPCODE_LOG_AND;
    } else if(strcmp(operatorString, "||") == 0){
        return OPCODE_LOG_OR;
    } else{
        //this should never actually happen
        printf("Syntax Error: Unrecognized Operator\n");
        exit(1);
    }
}

typedef struct operatorCodeIndexed {
    operatorCode_t operatorCode;
    int index;
} operatorCodeIndexed_t;

static ASTNode *parse_expression(void) {
    //treating current as Lookahead, with previous going into TOS
    ASTNode *node; //parent node of return object
    Token expressionTokenArray[OPERATOR_TOKEN_MAX];
    int tokenIndex = 0;

    operatorCodeIndexed_t operatorStack [OPERATOR_TOKEN_MAX]; //contains optype information as well as the address in the accumulated list
    int TOS = -1;

    bool acceptingID = true;
    bool acceptingOperator = false;

    //scan to end of expression
    while(current_token.type != TOKEN_SEMICOLON) {
        expressionTokenArray[tokenIndex] = current_token; //accumulate all tokens in expression in a list
        if(match(TOKEN_IDENTIFIER)) {
            if (!acceptingID) {
                perror("Syntax Error: received an unexpected identifier in expression.");
                //todo: add error code for unexpected back to back ids
                exit(1);
            }
            acceptingID = false;
            acceptingOperator = true;
            operatorCode_t lookaheadOpCode = OPCODE_ID;
            if (TOS >= 0) {
                int relation = getOperatorRelationship(operatorStack[TOS].operatorCode, lookaheadOpCode);
                //only compare if there are items in the stack, otherwise move on
            }
        } else if (match(TOKEN_OPERATOR)){

        }

    }
    //pop all remaining in stack
    while(TOS > 0){

    }



    if (match(TOKEN_NUMBER)) {
        node = create_node(AST_NUMBER);
        advance();
    } else if (match(TOKEN_IDENTIFIER)) {
        node = create_node(AST_IDENTIFIER);
        advance();
    } else {
        printf("Syntax Error: Expected expression\n");
        exit(1);
    }

    return node;
}


// Parse program (multiple statements)
static ASTNode *parse_program(void) {
    //right recursive grammar
    ASTNode *program = create_node(AST_PROGRAM);
    ASTNode *current = program;

    while (!match(TOKEN_EOF)) {
      current->left = parse_statement();
        // parse_statement() contains advance() calls, hence re-check
        if (!match(TOKEN_EOF)) {
            current->right = create_node(AST_PROGRAM);
            current = current->right;
        }
    }

    return program;
}

// Initialize parser
void parser_init(const char *input) {
    source = input;
    position = 0;
    advance(); // Get first token
}

// Main parse function
ASTNode *parse(void) {
    return parse_program();
}

// Print AST (for debugging)
void print_ast(ASTNode *node, int level) {
    if (!node) return;

    // Indent based on level
    for (int i = 0; i < level; i++) printf("  ");

    // Print node info
    switch (node->type) {
        case AST_PROGRAM:
            printf("Program\n");
            break;
        case AST_VARDECL:
            printf("VarDecl: %s\n", node->token.lexeme);
            break;
        case AST_ASSIGN:
            printf("Assign\n");
            break;
        case AST_PRINT:
            printf("Print\n");
            break;
        case AST_NUMBER:
            printf("Number: %s\n", node->token.lexeme);
            break;
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->token.lexeme);
            break;
        case AST_STRINGCHAR:
            printf("String/Char: %s\n", node->token.lexeme);
            break;
        //control flow cases
        case AST_IF:
            printf("If statement\n");
            break;
        case AST_ELSE:
            printf("Else statement\n");
            break;
        case AST_WHILE:
            printf("While statement\n");
            break;
        case AST_FOR:
            printf("For statement\n");
            break;
        case AST_UNTIL:
            printf("Repeat-Until statement\n");
            break;
        case AST_BREAK:
            printf("Break statement\n");
            break;
        //function node types
        case AST_FUNCTION_DECL:
            printf("Function declaration: %s\n", node->token.lexeme);
            break;
        case AST_FUNCTION_CALL:
            printf("Function call: %s\n", node->token.lexeme);
            break;
        case AST_RETURN:
            printf("Return statement\n");
            break;
        case AST_BLOCK:
            printf("Block\n");
            break;
        case AST_PARAM_LIST:
            printf("Parameter list\n");
            break;
        //expression cases
        case AST_BINOP:
            printf("Binary operator: %s\n", node->token.lexeme);
            break;
        case AST_UNARYOP:
            printf("Unary operator: %s\n", node->token.lexeme);
            break;
        case AST_COMPARISON:
            printf("Comparison operator: %s\n", node->token.lexeme);
            break;
        case AST_LOGIC_OP:
            printf("Logical operator: %s\n", node->token.lexeme);
            break;
        case AST_CAST:
            printf("Cast: %s\n", node->token.lexeme);
            break;
        case AST_NULL:
            printf("Null\n");
            break;

        // TODO 6: Add cases for new node types
        // case AST_IF: printf("If\n"); break;
        // case AST_WHILE: printf("While\n"); break;
        // case AST_REPEAT: printf("Repeat-Until\n"); break;
        // case AST_BLOCK: printf("Block\n"); break;
        // case AST_BINOP: printf("BinaryOp: %s\n", node->token.lexeme); break;
        default:
            printf("Unknown node type\n");
    }

    // Print children
    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
}

// Free AST memory
void free_ast(ASTNode *node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}
char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    // Seek to the end to get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Allocate memory and read the file contents
    char *buffer = (char *)malloc(file_size + 1);
    if (!buffer) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';  // Null-terminate the string

    fclose(file);
    return buffer;
}
// Main function for testing
int main() {
    ASTNode *ast = parse();

    printf("\nAbstract Syntax Tree:\n");
    print_ast(ast, 0);

    // Read from test files
    const char *filenames[] = {"../phase2-w25/test/input_valid.txt", "../phase2-w25/test/input_invalid.txt"};

    for (int i = 0; i < 2; i++) {
        printf("Parsing file: %s\n", filenames[i]);

        char *file_input = read_file(filenames[i]);
        if (!file_input) {
            printf("Skipping file due to read error.\n");
            continue;
        }

        parser_init(file_input);
        ast = parse();

        printf("\nAbstract Syntax Tree:\n");
        print_ast(ast, 0);
        free_ast(ast);

        free(file_input);
        printf("\n-----------------------------\n");
    }
    return 0;
}
