/* semantic.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../include/tokens.h"
#include "../../include/parser.h"
#include "../../include/semantic.h"

/* --- SYMBOL TABLE OPERATIONS --- */
// Initialize a new symbol table
// Creates an empty symbol table structure with scope level set to 0
SymbolTable* init_symbol_table() {
    SymbolTable* table = malloc(sizeof(SymbolTable));
    if (table) {
        table->head = NULL;
        table->current_scope = 0;
    }
    return table;
}

// Add a symbol to the table
// Inserts a new variable with given name, type, and line number into the current scope
void add_symbol(SymbolTable* table, const char* name, int type, int line) {
    Symbol* symbol = malloc(sizeof(Symbol));
    if (symbol) {
        strcpy(symbol->name, name);
        symbol->type = type;
        symbol->scope_level = table->current_scope;
        symbol->line_declared = line;
        symbol->is_initialized = 0;

        // Add to beginning of list
        symbol->next = table->head;
        table->head = symbol;
    }
}

// Look up a symbol in the table by name across all accessible scopes
// Returns the symbol if found, NULL otherwise
Symbol* lookup_symbol(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Look up a symbol in the table by name across current accessible scopes
// Returns the symbol if found, NULL otherwise
Symbol* lookup_symbol_current_scope(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0 && current->scope_level == table->current_scope) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Prints the full symbol table
void print_symbol_table(SymbolTable* table){
    Symbol* current = table->head;
    while (current) {
        print_symbol(current);
        current = current->next;
    }
}

// Prints a specific symbol and its details
void print_symbol(Symbol* symbol) {
    printf("Type: %d Scope Level: %d Name: %s\n", symbol->type, symbol->scope_level, symbol->name);
}

// Increments the current scope level when entering a block (e.g., if, while)
void enter_scope(SymbolTable* table) {
    table->current_scope += 1;
}

// Decrements the current scope level when leaving a block
void exit_scope(SymbolTable* table) {
    table->current_scope--;
    remove_symbols_in_current_scope(table);
}

// Remove symbols belonging to scope being exited
// Cleans up symbols that are no longer accessible after leaving a scope
void remove_symbols_in_current_scope(SymbolTable* table) {
    Symbol* current = table->head;
    Symbol* previous = NULL;
    while (current) {
        // when above scope level
        if (current->scope_level > table->current_scope) {
            if (current == table->head) {
                table->head = current->next;
                free_symbol(current);
                current = table->head;
            } else {
                previous->next = current->next;
                free_symbol(current);
                current = previous->next;
            }
        } else { // otherwise move forward
            previous = current;
            current = current->next;
        }
    }

}

// Free twin symbol he aint deserve to be locked up for that (and his next in line)
void free_symbol(Symbol* symbol) {
    if(symbol->next!=NULL) {
        free_symbol(symbol->next);
    }
    free(symbol);
}

// Free the symbol table memory
// Releases all allocated memory when the symbol table is no longer needed
void free_symbol_table(SymbolTable* table) {
    free(table);
}

/* --- SEMANTIC ANALYSIS FUNCTIONS --- */
// Main semantic analysis function
int analyze_semantics(ASTNode* ast) {
    SymbolTable* table = init_symbol_table();
    int result = check_program(ast, table);
    free_symbol_table(table);
    return result;
}

// Check program node
int check_program(ASTNode* node, SymbolTable* table) {
    if (!node) return 1;
    int result = 1;
    if (node->type == AST_PROGRAM || node->type == AST_BLOCK) {
        // Check left child (statement)
        if (node->left) {
            result = check_statement(node->left, table) && result;
        }

        // Check right child (rest of program)
        if (node->right) {
            result = check_program(node->right, table) && result;
        }
    }
    return result;
}

// Check statements of all types, calls functions
int check_statement(ASTNode* node, SymbolTable* table) {
    if (node->type == AST_INT) {
        printf("Checking statement of type: Variable Declaration Int\n");
        return check_declaration(node, table);
    }
    if (node->type == AST_STRINGCHAR) {
        printf("Checking statement of type: Variable Declaration String\\Char\n");
        return check_declaration(node, table);
    }
    if (node->type == AST_ASSIGN) {
        printf("Checking statement of type: Variable Assignment\n");
        return check_assignment(node, table);
    }
    if (node->type == AST_BLOCK) {
        printf("Checking statement of type: Block\n");
        return check_block(node, table);
    }
    if (node->type == AST_PRINT) {
        printf("Checking statement of type: Print\n");
        return check_print(node, table);
    }
    if (node->type == AST_IF || node->type == AST_WHILE || node->type == AST_REPEAT) {
        printf("Checking statement of type: If, While, or Repeat-Until\n");
        return check_condition(node->left, table) && check_block(node->right, table);
    }
    if (node->type == AST_ELSE) {
        printf("Checking statement of type: Else\n");
        return check_block(node->right, table);
    }
    printf("STATEMENT UNRECOGNIZED\n");
    return 0;
}

// Check a variable declaration
int check_declaration(ASTNode* node, SymbolTable* table) {
    const char* name = node->token.lexeme;

    // Check if variable already declared in current scope
    Symbol* existing = lookup_symbol_current_scope(table, name);
    if (existing) {
        semantic_error(SEM_ERROR_REDECLARED_VARIABLE, name, node->token.line);
        return 0;
    }

    // Add to symbol table
    add_symbol(table, name, node->type, node->token.line);
    printf("Updated Symbol Table\n");
    print_symbol_table(table);
    return 1;
}

// Check a variable assignment
int check_assignment(ASTNode* node, SymbolTable* table) {
    const char* name = node->left->token.lexeme;

    // Check if variable exists
    Symbol* symbol = lookup_symbol(table, name);
    if (!symbol) {
        semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, name, node->token.line);
        return 0;
    }

    // Check expression
    int expr_valid = 0;
    if(symbol->type == AST_STRINGCHAR){ // STRING CONDITIONS
        expr_valid = check_string(node->right, table);
    }
    if(symbol->type == AST_INT){ // INT CONDITIONS
        expr_valid = check_expression(node->right, table);
    }

    // Mark as initialized
    if (expr_valid) {
        symbol->is_initialized = 1;
    }
    return expr_valid;
}

// Check an expression for type correctness
bool check_expression(ASTNode* node, SymbolTable* table) {
    // Recursive Until Bottom
    bool left = true;
    bool right = true;
    bool current = false;
    // go to left and right child
    if(node->left != NULL) {
        left = check_expression(node->left, table);
    }
    if (node->right != NULL) {
        right = check_expression(node->right, table);
    }

    if (node->type == AST_NUMBER) {
        //printf("Valid Number in expression\n");
        current = true;
    } else if (node->type == AST_IDENTIFIER) {
        //printf("Caught Identifier, Checking Type\n");
        const char* name = node->token.lexeme;

        // Check if variable exists
        Symbol* symbol = lookup_symbol(table, name);
        if (!symbol) {
            semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, name, node->token.line);
            return 0;
        }
        if(symbol->type == AST_INT) {
            //printf("Valid Identifier Type\n");
            if(symbol->is_initialized != 1) {
                semantic_error(SEM_ERROR_UNINITIALIZED_VARIABLE, name, node->token.line);
            }
            current = true;
        } else {
            //printf("Invalid Identifier Type\n");
            current = false;
        }
    } else if (node->type == AST_BINOP || node->type == AST_UNARYOP || node->type == AST_FACTORIAL) {
        //printf("Valid Operator in Sequence\n");
        current = true;
    }else {
        //printf("Invalid Entry in expression\n");
        current = false;
    }
    return left && right && current;
}

// Check a string based expression for type correctness
bool check_string(ASTNode* node, SymbolTable* table) {
    if(node->type == AST_STRINGCHAR) {
        printf("Valid String\\Char\n");
        return 1;
    }
    return 0;
}

// Check a block of statements, handling scope
int check_block(ASTNode* node, SymbolTable* table) {
    enter_scope(table);
    printf("Block Parse Started\n");
    int ret = check_program(node, table);
    //print_symbol_table(table);
    exit_scope(table);
    printf("Block Parse Finished\n");
    //print_symbol_table(table);
    return ret;
}

// Check print statement
int check_print(ASTNode* node, SymbolTable* table) {
    if (node->type != AST_PRINT || !node->left) {
        return 0;
    }
    // checking for string/char print or int print
    if(node->left->type == AST_STRINGCHAR) {
        // return the given string
        printf("String/Char type print\n");
        return check_string(node->left, table);
    }
    // otherwise return the expression instead
    printf("Identifier/Int type print\n");
    return check_expression(node->left, table);
}

// Check a condition (e.g., in if statements)
int check_condition(ASTNode* node, SymbolTable* table) {
    return check_expression(node, table);
}

/* --- ERROR REPORTING --- */
void semantic_error(SemanticErrorType error, const char* name, int line) {
    switch (error) {
        case SEM_ERROR_UNDECLARED_VARIABLE:
            printf("Undeclared variable '%s' on line '%d'\n", name, line);
            break;
        case SEM_ERROR_REDECLARED_VARIABLE:
            printf("Variable '%s' already declared in this scope on line '%d'\n", name, line);
            break;
        case SEM_ERROR_TYPE_MISMATCH:
            printf("Type mismatch involving '%s' on line '%d'\n", name, line);
            break;
        case SEM_ERROR_UNINITIALIZED_VARIABLE:
            printf("Variable '%s' may be used uninitialized on line '%d'\n", name, line);
            break;
        case SEM_ERROR_INVALID_OPERATION:
            printf("Invalid operation involving '%s' on line '%d'\n", name, line);
            break;
        default:
            printf("Unknown semantic error with '%s' on line '%d'\n", name, line);
    }
}

int main() {
    // get file
    FILE *file = fopen("../phase2-w25/test/input_semantic_error.txt", "r");
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

    // Lexical analysis and parsing
    printf("Parsing input:\n%s\n\n", buffer);
    parser_init(buffer);
    ASTNode *ast = parse();
    printf("AST created. Printing...\n\n");
    print_ast(ast, 0);

    // Semantic analysis
    int result = analyze_semantics(ast);
    if (result) {
        printf("Semantic analysis successful. No errors found.\n");
    } else {
        printf("Semantic analysis failed. Errors detected.\n");
    }

    // Free Vars
    free_ast(ast);
    free(buffer);
    fclose(file);

    // get file
    file = fopen("../phase2-w25/test/input_valid.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    // get buffer size based on file size for chars
    buffer = malloc(file_size + 1);
    if (!buffer) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    // fill buffer with full file of chars in order
    bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';
    b = 0;
    for (size_t i = 0; i < bytes_read; i++) {
        if (buffer[i] != '\r') {
            buffer[b++] = buffer[i];
        }
    }
    buffer[b] = '\0';

    // Lexical analysis and parsing
    printf("Parsing input:\n%s\n\n", buffer);
    parser_init(buffer);
    ast = parse();
    printf("AST created. Printing...\n\n");
    print_ast(ast, 0);

    // Semantic analysis
    result = analyze_semantics(ast);
    if (result) {
        printf("Semantic analysis successful. No errors found.\n");
    } else {
        printf("Semantic analysis failed. Errors detected.\n");
    }

    // Free Vars
    free_ast(ast);
    free(buffer);
    fclose(file);
    return 0;
}