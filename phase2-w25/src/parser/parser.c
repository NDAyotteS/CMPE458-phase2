/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../include/parser.h"
#include "../../include/lexer.h"
#include "../../include/tokens.h"

// Current token being processed
static Token current_token;
static int position = 0;
static const char *source;
static const int OPERATOR_TOKEN_MAX = 128; //arbitrary

/* ---PARSER ERROR OUTPUTS FUNCTIONS--- */

static void parse_error(ParseError error, Token token) {
    printf("Parse Error at line %d: ", token.line);
    switch (error) {
        case PARSE_ERROR_UNEXPECTED_TOKEN:
            printf("Unexpected token '%s' at position '%d'\n", token.lexeme, position);
            break;
        case PARSE_ERROR_MISSING_SEMICOLON:
            printf("Missing semicolon after '%s' at position '%d'\n", token.lexeme, position);
            break;
        case PARSE_ERROR_MISSING_IDENTIFIER:
            printf("Expected identifier after '%s' at position '%d'\n", token.lexeme, position);
            break;
        case PARSE_ERROR_MISSING_EQUALS:
            printf("Expected '=' after '%s' at position '%d'\n", token.lexeme, position);
            break;
        case PARSE_ERROR_INVALID_EXPRESSION:
            printf("Invalid expression after '%s' at position '%d'\n", token.lexeme, position);
            break;
        case PARSE_ERROR_MISSING_PAREN:
            printf("Missing parentheses after '%s' at position '%d'\n", token.lexeme, position);
            break;
        case PARSE_ERROR_MISSING_CONDITION:
            printf("Missing condition after '%s' at position '%d'\n", token.lexeme, position);
            break;
        case PARSE_ERROR_MISSING_BRACE:
            printf("Missing brace after '%s' at position '%d'\n", token.lexeme, position);
            break;
        case PARSE_ERROR_FUNC_CALL:
            printf("Function call '%s' at position '%d'\n", token.lexeme, position);
            break;
        case PARSE_ERROR_INVALID_FUNC_DECLARATION:
            printf("Function declaration syntax error '%s' at position '%d'\n", token.lexeme, position);
            break;
        default:
            printf("Unknown error\n");
    }
}

// Get next token
static void advance(void) {
    current_token = get_next_token(source, &position);
}

/* ---PARSER FLOW AND CONTROL FUNCTIONS--- */

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
        printf("Invalid token, got '%s' Expected type: '%d'", current_token.lexeme, type);

        exit(1); // Or implement error recovery
    }
}

// Forward declarations for functions
static ASTNode *parse_statement(void);
static ASTNode *parse_expression(void);
static ASTNode *parse_assignment_or_function(void);
static ASTNode *parse_block_statement(void);

/* ---PARSING FUNCTIONS FOR KEYWORDS AND PRE-MADE FUNCTIONS--- */

// Parses if() statements
static ASTNode* parse_if_statement(void) {
    ASTNode *node = create_node(AST_IF);
    advance(); // consume if keyword
    expect(TOKEN_LEFTPARENTHESES); // check for correct parentheses (
    node->left = parse_expression(); // conditions in if stored in left child (handled by parse_expression)
    expect(TOKEN_RIGHTPARENTHESES);  // check for correct parentheses )
    node->right = parse_statement(); // if body (handled by parse_statement)
    return node;
}

// Parses else statements
static ASTNode* parse_else_statement(void) {
    ASTNode *node = create_node(AST_ELSE);
    advance(); // consume else keyword
    node->right = parse_statement(); // else body (handled by parse_statement)
    return node;
}

// Parses while loop statements
static ASTNode* parse_while_statement(void) {
    ASTNode *node = create_node(AST_WHILE);
    advance(); // consume while keyword
    expect(TOKEN_LEFTPARENTHESES); // check for correct parentheses (
    node->left = parse_expression(); // conditions for looping within while (handled by parse_expression)
    expect(TOKEN_RIGHTPARENTHESES); // check for correct parentheses )
    node->right = parse_statement(); // loop body (handled by parse_statement)
    return node;
}

// Parses until loop statements
/* STATEMENTS HAVE THE FORM
 *  repeat{
 *      body code
 *  }until();
 */
static ASTNode* parse_until_statement(void) {
    ASTNode *node = create_node(AST_REPEAT);
    advance(); // consume repeat keyword
    node->left = parse_statement(); // repeated body (handled by parse_statement)
    // following block statement, need until()
    if (!match(TOKEN_UNTIL)) { // case without until
        parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
        exit(1);
    }
    advance(); // consume until keyword
    expect(TOKEN_LEFTPARENTHESES); // check for correct parentheses (
    node->right = parse_expression(); // conditions for looping (handled by parse_expression)
    expect(TOKEN_RIGHTPARENTHESES); // check for correct parentheses )
    expect(TOKEN_SEMICOLON); // check semicolon after conditions
    return node;
}

// Parses print statements
static ASTNode* parse_print_statement(void) {
    ASTNode *node = create_node(AST_PRINT);
    advance(); // consume print keyword
    expect(TOKEN_LEFTPARENTHESES); // check for correct parentheses (
    node->left = parse_expression();
    expect(TOKEN_RIGHTPARENTHESES); // check for correct parentheses )
    if (!match(TOKEN_SEMICOLON)) {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        exit(1);
    }
    advance();
    return node;
}

// Parses function declarations
/* STATEMENTS HAVE THE FORM
 *  func identifier(keyword identifier, keyword identifier,...){
 *      body code
 *  }
 */
static ASTNode* parse_function_declaration(void) {
    ASTNode *node = create_node(AST_FUNCTION_DECL);
    advance(); // consume func keyword
    expect(TOKEN_IDENTIFIER); // check for valid Identifier function name
    expect(TOKEN_LEFTPARENTHESES); // check for correct parentheses (
    // if not immediately followed by parentheses, go through arguments
    if(!match(TOKEN_RIGHTPARENTHESES)) {
        // go through function arguments (NOT CURRENTLY SAVED IN A TREE)
        while (1) {
            // starts with keyword for variable type
            if (!match(TOKEN_INT) && !match(TOKEN_CHAR) && !match(TOKEN_STRING)) {
                parse_error(PARSE_ERROR_INVALID_FUNC_DECLARATION, current_token);
                exit(1);
            }
            advance();
            expect(TOKEN_IDENTIFIER); // followed by identifier
            if(match(TOKEN_RIGHTPARENTHESES)) break; // if the parentheses are closed the function is complete
            expect(TOKEN_COMMA); // otherwise a comma to add more variable arguments
        }
    }

    advance(); // move past the )
    node->right = parse_statement(); // block statement saved to right node (handled by parse_statement)
    return node;
}

// Parses function call
/* STATEMENTS HAVE THE FORM
 *  func identifier(keyword identifier, keyword identifier,...){
 *      body code
 *  }
 */
static ASTNode* parse_function_call(void) {
    ASTNode *node = create_node(AST_FUNCTION_CALL);
    advance(); // consumes the ( to start the function call
    // go through function arguments
    while (1) {
        expect(TOKEN_IDENTIFIER); // identifier first
        if(match(TOKEN_RIGHTPARENTHESES)) break; // if the parentheses are closed the function is complete
        expect(TOKEN_COMMA); // otherwise a comma to add more variable arguments
    }
    advance(); // move past the )
    // the parse assignment_or_function handles the semicolon
    return node;
}

// Parses the factorial as though it was a function
/* STATEMENTS HAVE THE FORM
 *  $(expression)
 */
static ASTNode *parse_factorial(void){
    ASTNode *node = create_node(AST_FACTORIAL);
    advance(); // consume factorial symbol $
    expect(TOKEN_LEFTPARENTHESES); // check for correct parentheses (
    node->left = parse_expression(); // parse expression should handle the arguments for the function
    expect(TOKEN_RIGHTPARENTHESES); // check for correct parentheses )
    // Need semicolon checking???
    advance();
    return node;
}

// Parses block statements (the { ... } inside of a function, if statement, loop, etc)
static ASTNode* parse_block_statement(void) {
    ASTNode *node = create_node(AST_BLOCK);
    ASTNode *current = NULL; // track the current node as to build the full block statement tree
    advance(); // consume { symbol
    // will continue to build the tree of the block
    while (!match(TOKEN_RIGHTBRACE) && !match(TOKEN_EOF)) {
        ASTNode *next_statement = parse_statement();
        //builds to the left on with first statement
        if (node->left == NULL) {
            node->left = next_statement;
            current = node->left;
        } else { // then builds on right side perpetually
            current->right = next_statement;
            current = current->right;
        }
    }
    // checks the condition that ended the loop (should be } if correct)
    if (!match(TOKEN_RIGHTBRACE)) {
        parse_error(PARSE_ERROR_MISSING_BRACE, current_token);
        exit(1);
    }
    advance(); // consume } symbol
    return node;
}


/* ---PARSING FUNCTIONS FOR BASIC DECLARATIONS AND ASSIGNMENTS--- */

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
    // Failed case
    parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
    exit(1);
}

// Parse assignment or function call: e.g. x = 5; or x = 'yippee'; or add(x, y);
static ASTNode *parse_assignment_or_function(void) {
    ASTNode *node = create_node(AST_ASSIGN);
    node->left = create_node(AST_IDENTIFIER);
    node->left->token = current_token;
    advance();

    // Case for identifier being used to call a function
    if (match(TOKEN_LEFTPARENTHESES)) {
        node->type = AST_FUNCTION_CALL; // overwrite assign node type
        node->right = parse_function_call();
    }
    else {
        // Case of identifier being assigned a value
        if (!match(TOKEN_EQUALS)) {
            parse_error(PARSE_ERROR_MISSING_EQUALS, current_token);
            exit(1);
        }
        advance();

        // For the case where the assignment is for strings, chars, or null values
        if(match(TOKEN_STRING) || match(TOKEN_CHAR)) {
            node->right = create_node(AST_STRINGCHAR);
            node->right->token = current_token;
            advance();
        }
        else if(match(TOKEN_NULL)) {
            node->right = create_node(AST_NULL);
            node->right->token = current_token;
            advance();
        }
        else { // All other assignment types
            node->right = parse_expression();
        }
    }

    // Parse_expression(), string assignment, and function calls all advance, check that statement ended with ;
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
    if (match(TOKEN_IDENTIFIER)) return parse_assignment_or_function();
    if (match(TOKEN_IF)) return parse_if_statement();
    if (match(TOKEN_ELSE)) return parse_else_statement();
    if (match(TOKEN_WHILE)) return parse_while_statement();
    if (match(TOKEN_REPEAT)) return parse_until_statement();
    if (match(TOKEN_PRINT)) return parse_print_statement();
    if (match(TOKEN_FUNC)) return parse_function_declaration();
    if (match(TOKEN_FACTORIAL)) return parse_factorial();
    if (match(TOKEN_LEFTBRACE)) return parse_block_statement();

    printf("Syntax Error: Unexpected token %s at position %d line %d\n", current_token.lexeme, position, current_token.line);
    exit(1);
}

//for things like identifiers, numbers, functions, and nested expressions
static ASTNode *parse_non_ops(void) {
    ASTNode *node;
    if (match(TOKEN_NUMBER)) {
        node = create_node(AST_NUMBER);
        advance();
        return node;
    }
    if (match(TOKEN_IDENTIFIER)) {
        int holdPos = position;
        Token next_token = get_next_token(source, &position);
        position = holdPos; // Restore position
        if (next_token.type == TOKEN_LEFTPARENTHESES) { // function call case
            return parse_function_call();
        }
        if (next_token.type == TOKEN_FACTORIAL) { // factorial case
            return parse_factorial();
        }
        // all other cases
        node = create_node(AST_IDENTIFIER);
        advance();
        return node;
    }
    if (match(TOKEN_STRING_LITERAL) || match(TOKEN_CHAR_LITERAL)) {
        node = create_node(AST_STRINGCHAR);
        advance();
        return node;
    }
    if (match(TOKEN_LEFTPARENTHESES)) {
        advance();
        //call recursively on expression in parentheses
        node = parse_expression();
        expect(TOKEN_RIGHTPARENTHESES);//make sure it closes
        return node;
    }
    printf("Expected an identifier, number, function, or parentheses sub-expression\n");
    printf("Token: %s LINE: %d\n", current_token.lexeme, current_token.line);
    exit(1);
}

static ASTNode *parse_not(void) {
    ASTNode *node = parse_non_ops();
    while (strcmp(current_token.lexeme, "!")==0) {
        Token operator = current_token;
        advance();
        ASTNode *new = create_node(AST_UNARYOP);
        new->token = operator;
        new->left = node;
        new->right;
        node = new;
    }
    return node;
}

static ASTNode *parse_pow(void) {
    ASTNode *node = parse_not();
    while (strcmp(current_token.lexeme, "^^")==0){
        Token operator = current_token;
        advance();
        ASTNode *left = parse_not();
        ASTNode *new = create_node(AST_BINOP);
        new->token = operator;
        new->left = left;
        new->right = node;
        node = new;
    }
    return node;
}

static ASTNode *parse_mult_div_mod(void) {
    ASTNode *node = parse_pow();
    while (strcmp(current_token.lexeme, "/")==0 || strcmp(current_token.lexeme, "*")==0){
        Token operator = current_token;
        advance();
        ASTNode *right = parse_pow();
        ASTNode *new = create_node(AST_BINOP);
        new->token = operator;
        new->left = node;
        new->right = right;
        node = new;
    }
    return node;
}

static ASTNode *parse_add_sub(void) {
    ASTNode *node = parse_mult_div_mod();
    while (strcmp(current_token.lexeme, "+")==0 || strcmp(current_token.lexeme, "-")==0) {
        Token operator = current_token;
        advance();
        ASTNode *right = parse_mult_div_mod();
        ASTNode *new = create_node(AST_BINOP);
        new->token = operator;
        new->left = node;
        new->right = right;
        node = new;
        }
    return node;
}

static ASTNode *parse_grt_geq_leq_les(void) {
    ASTNode *node = parse_add_sub();
    while (strcmp(current_token.lexeme, ">")==0 || strcmp(current_token.lexeme, "<")==0
        || strcmp(current_token.lexeme, ">=")==0 || strcmp(current_token.lexeme, "<=")==0){
        Token operator = current_token;
        advance();
        ASTNode *right = parse_add_sub();
        ASTNode *new = create_node(AST_BINOP);
        new->token = operator;
        new->left = node;
        new->right = right;
        node = new;
        }
    return node;
}

static ASTNode *parse_logical_eq_not_eq(void) {
    ASTNode *node = parse_grt_geq_leq_les();
    while (strcmp(current_token.lexeme, "==")==0 || strcmp(current_token.lexeme, "!=")==0) {
        Token operator = current_token;
        advance();
        ASTNode *right = parse_grt_geq_leq_les();
        ASTNode *new = create_node(AST_BINOP);
        new->token = operator;
        new->left = node;
        new->right = right;
        node = new;
        }
    return node;
}

static ASTNode *parse_logical_and(void) {
    ASTNode *node = parse_logical_eq_not_eq();
    while (strcmp(current_token.lexeme, "&&")==0){
        Token operator = current_token;
        advance();
        ASTNode *right = parse_logical_eq_not_eq();
        ASTNode *new = create_node(AST_BINOP);
        new->token = operator;
        new->left = node;
        new->right = right;
        node = new;
        }
    return node;
}
static ASTNode *parse_logical_or(void) {
    ASTNode *node = parse_logical_and();
    while (strcmp(current_token.lexeme, "||")==0){
        Token operator = current_token;
        advance();
        ASTNode *right = parse_logical_and();
        ASTNode *new = create_node(AST_BINOP);
        new->token = operator;
        new->left = node;
        new->right = right;
        node = new;
    }
    return node;
}

static ASTNode *parse_expression(void) {
    ASTNode *node = parse_logical_or();
    return node;
}



/* ---PARSER INITIALIZATION AND OUTPUT FUNCTIONS--- */

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
        case AST_FACTORIAL:
            printf("Factorial %s\n", node->token.lexeme);
            break;
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

// For reading input files
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
    free_ast(ast);
    return 0;
}
