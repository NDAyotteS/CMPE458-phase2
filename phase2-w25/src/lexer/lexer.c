
/* lexer.c */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../../include/tokens.h"

// Line tracking
static int current_line = 1;
static char last_token_type = 'y'; // For checking consecutive operators

// Keyword Table
static struct {
    const char* word;
    TokenType type;
} keywords[] = {
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},
    {"repeat", TOKEN_REPEAT},
    {"until", TOKEN_UNTIL},
    {"break", TOKEN_BREAK},
    {"print", TOKEN_PRINT},
    {"int", TOKEN_INT},
    {"char", TOKEN_CHAR},
    {"string", TOKEN_STRING},
    {"null", TOKEN_NULL},
};

// New Keyword Checker
static int IsKeyword(const char* word) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(word, keywords[i].word) == 0) {
            return keywords[i].type;
        }
    }
    return 0;
}

/* Print error messages for lexical errors */
void print_error(ErrorType error, int line, const char *lexeme) {
    printf("Lexical Error at line %d: ", line);
    switch (error) {
        case ERROR_INVALID_CHAR:
            printf("Invalid character '%s'\n", lexeme);
            break;
        case ERROR_INVALID_NUMBER:
            printf("Invalid number format\n");
            break;
        case ERROR_CONSECUTIVE_OPERATORS:
            printf("Consecutive operators not allowed\n");
            break;
        case ERROR_STRING_OVERFLOW:
            printf("Overflow in string\n");
            break;
        case ERROR_UNTERMINATED_STRING:
            printf("Unterminated string\n");
            break;
        case ERROR_INVALID_ESCAPE_CHARACTER:
            printf("Unrecognized/invalid escape character\n");
            break;
        case ERROR_UNTERMINATED_CHARACTER:
            printf("Unterminated character\n");
            break;
        default:
            printf("Unknown error\n");
    }
}

/* Print token information */
void print_token(Token token) {
    if (token.error != ERROR_NONE) {
        print_error(token.error, token.line, token.lexeme);
        return;
    }

    printf("Token: ");
    switch (token.type) {
        case TOKEN_NUMBER:
            printf("NUMBER");
            break;
        case TOKEN_OPERATOR:
            printf("OPERATOR");
            break;
        case TOKEN_EOF:
            printf("EOF");
            break;
        case TOKEN_IF:
        case TOKEN_ELSE:
        case TOKEN_WHILE:
        case TOKEN_UNTIL:
        case TOKEN_REPEAT:
        case TOKEN_BREAK:
        case TOKEN_PRINT:
        case TOKEN_INT:
        case TOKEN_CHAR:
        case TOKEN_STRING:
        case TOKEN_NULL:
            printf("KEYWORD");
            break;
        case TOKEN_IDENTIFIER:
            printf("IDENTIFIER");
            break;
        case TOKEN_STRING_LITERAL:
            printf("STRING_LITERAL");
            break;
        case TOKEN_CHAR_LITERAL:
            printf("CHAR_LITERAL");
            break;
        case TOKEN_LEFTPARENTHESES:
        case TOKEN_LEFTBRACKET:
        case TOKEN_LEFTBRACE:
        case TOKEN_RIGHTBRACE:
        case TOKEN_RIGHTBRACKET:
        case TOKEN_RIGHTPARENTHESES:
        case TOKEN_COMMA:
            printf("DELIMITER");
            break;
        case TOKEN_SPECIAL_CHARACTER:
            printf("SPECIAL_CHARACTER");
            break;
        case TOKEN_SEMICOLON:
            printf("SEMICOLON");
            break;
        case TOKEN_EQUALS:
            printf("EQUALS");
            break;
        case TOKEN_COMPARITIVE:
            printf("COMPARATIVE SYMBOL");
            break;
        case TOKEN_FACTORIAL:
            printf("COMPARATIVE SYMBOL");
        break;
        default:
            printf("UNKNOWN");
    }
    printf(" | Lexeme: '%s' | Line: %d\n", token.lexeme, token.line);
}

/* Get next token from input */
Token get_next_token(const char *input, int *pos) {
    Token token = {TOKEN_ERROR, "", current_line, ERROR_NONE};
    char c;

    // Skip whitespace and track line numbers
    while ((c = input[*pos]) != '\0' && (c == ' ' || c == '\n' || c == '\t')) {
        if (c == '\n') {
            current_line++;
        }
        (*pos)++;
    }

    // Check for end of file
    if (input[*pos] == '\0') {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    // get current character
    c = input[*pos];

    // Comment handler
    // Single line comment
    if (c == '#') {
        do{
            (*pos)++;
            c = input[*pos];
        } while(c != '\n');
        //skip newline character
        (*pos)++;
        c = input[*pos];

        while(c == ' ' || c == '\t' || c == '\n'  && c != '\0'){
            (*pos)++;
            c = input[*pos];
        }
    }

    // Multi line comment
    // should skip until */ is reached
    if (c == '/' && input[*pos + 1] == '*') {
        (*pos)++; // skip /
        // c = input[*pos];
        do{
            (*pos)++; //move ahead (will also skip asterisk in /*)
            c = input[*pos];
            if (c == '\0') {
                printf("[WARN]: Unclosed comment\n");
                break;
            }
        }while((c != '*') && (input[*pos + 1] != '/'));
        (*pos)+=2; // move ahead of */
        c = input[*pos];
        //skip to start of next token
        while(c == ' ' || c == '\t' || c == '\n'  && c != '\0'){
            (*pos)++;
            c = input[*pos];
        }
    }

    // Number handler
    // FIX THE IDENTIFIER/DIGIT CHECKING (10x should throw an error)
    if (isdigit(c)) {
        int i = 0;
        do {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while (isdigit(c) && i < sizeof(token.lexeme) - 1);

        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER;
        last_token_type = 'n'; //number
        return token;
    }

    // Keyword and Identifier handler
    if(isalpha(c) || c == '_'){
        int i = 0;
        do {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while ((isalnum(c) || c == '_') && i < sizeof(token.lexeme) - 1);

        token.lexeme[i] = '\0';

        // Check if it's a keyword
        TokenType keyword_type = IsKeyword(token.lexeme);
        if (keyword_type) {
            token.type = keyword_type;
            last_token_type = 'k'; //keyword
        } else {
            token.type = TOKEN_IDENTIFIER;
            last_token_type = 'i'; //identifier
        }
        return token;
    }

    // Special character handler
    if((c == '&' && input[*pos + 1] != '&') || c == '_') {
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        token.type = TOKEN_SPECIAL_CHARACTER;
        (*pos)++;
        last_token_type = 'z'; //special character
        return token;
    }

    // String literal handler
    if(c == '"'){
        int i = 0;
        token.lexeme[i++] = c;
        (*pos)++;
        do{
            // check to see if string is too long (above 100 characters)
            if(i >= sizeof(token.lexeme) - 1) {
                token.error = ERROR_STRING_OVERFLOW;
                token.lexeme[i] = '\0';
                last_token_type = 'e'; // error
                (*pos)++;
                char overflow = input[*pos];
                // continues until the string is closed, just doesn't save the string data anymore
                while(overflow != '\"') {
                    // have to check for EOF to ensure string is terminated at some point
                    if (input[*pos] == '\0') {
                        token.error = ERROR_UNTERMINATED_STRING;
                        break;
                    }
                    // continue to track but not save
                    overflow = input[*pos];
                    (*pos)++;
                }
                break;
            }
            // get character for comparison
            char c_string = input[*pos];
            // closing quotation case
            if (c_string == '\"') {
                token.lexeme[i++] = c_string;
                token.lexeme[i] = '\0';
                token.type = TOKEN_STRING_LITERAL;
                last_token_type = 's'; //string
                (*pos)++;
                break;
            }
            // end of file means unterminated
            if (c_string == '\0') {
                token.error = ERROR_UNTERMINATED_STRING;
                token.lexeme[i] = '\0';
                last_token_type = 'e'; //error
                break;
            }
            // case of escape character
            if (c_string == '\\' && i <= sizeof(token.lexeme) - 1) {
                // introduces niche case of the character that overflows the token size, might need its own handler
                char c_escape = input[*pos+1];
                switch (c_escape) {
                    case '\\':
                    case '\'':
                    case '\"':
                        // characters that are on their own
                        token.lexeme[i++] = c_escape;
                        (*pos) += 2;
                        break;
                    case 'n':
                        // newline
                        token.lexeme[i++] = '\n';
                        (*pos) += 2;
                        break;
                    case 'r':
                        // carriage return
                        token.lexeme[i++] = '\r';
                        (*pos) += 2;
                        break;
                    case 't':
                        // tab
                        token.lexeme[i++] = '\t';
                        (*pos) += 2;
                        break;
                    default:
                        // unrecognized escape character
                        token.error = ERROR_INVALID_ESCAPE_CHARACTER;
                        token.lexeme[i++] = c_string;
                        token.lexeme[i++] = c_escape;
                        last_token_type = 'e'; // error
                        (*pos) += 2;
                        break;
                }
            } else { // case of any valid character
                token.lexeme[i++] = c_string;
                (*pos)++;
            }
        } while(1);

        // returning
        return token;
    }

    // char literal handler
    if(c == '\''){
        // following character should be an escape character
        char c_char = input[*pos+1];
        if(c_char == '\\') {
            // check it gets closed, if not skip 4 characters and continue
            if (input[*pos+3] != '\'') {
                token.error = ERROR_UNTERMINATED_CHARACTER;
                token.lexeme[0] = c_char;
                token.lexeme[1] = '\0';
                last_token_type = 'e'; //error
                (*pos) += 4;
                return token;
            }
            // case block for all escape characters supported by the system
            char c_escape = input[*pos+2];
            switch (c_escape) {
                case '\\':
                case '\'':
                case '\"':
                    // basic escape chars
                    token.lexeme[0] = c_escape;
                    token.lexeme[1] = '\0';
                    break;
                case 'n':
                    // newline
                    token.lexeme[0] = '\n';
                    token.lexeme[1] = '\0';
                    break;
                case 'r':
                    // carriage return
                    token.lexeme[0] = '\r';
                    token.lexeme[1] = '\0';
                    break;
                case 't':
                    // tab
                    token.lexeme[0] = '\t';
                    token.lexeme[1] = '\0';
                    break;
                default:
                    // unrecognized escape character
                    token.error = ERROR_INVALID_ESCAPE_CHARACTER;
                    last_token_type = 'e'; // error
                    (*pos) += 4;
                    return token;
            }
            token.type = TOKEN_CHAR_LITERAL;
            last_token_type = 'x'; // escape char
            (*pos) += 4;
            return token;
        }

        // unterminated character
        if (input[*pos+2] != '\'') {
            token.error = ERROR_UNTERMINATED_CHARACTER;
            last_token_type = 'e'; // error
            (*pos) += 3;
        }
        else {  // any valid character
            token.lexeme[0] = c_char;
            token.lexeme[1] = '\0';
            token.type = TOKEN_CHAR_LITERAL;
            *pos += 3;
            last_token_type = 'c'; // char
        }
        // the char literal handler can finally return
        return token;
    }

    /* List of Operators (Grouped by first character and behaviour):
    //RULE: Standalone
    $:  $ (factorial)

    //RULE: Can be trailed by one repetition or an equals sign
    +:  + (add), ++ (increment)
    -:  - (sub), -- (decrement)

    //RULE: Can be trailed only by an equals sign
    *:  * (multiply)
    /:  / (divide)
    %:  % (modulo)
    =:  = (assignment), == (logic eq)
    !:  ! (bitwise not), != (logic not)

    //RULE: Can be trailed only by itself
    |:  || (logical or)
    ^:  ^^ (power)

    //RULE: Can be trailed by itself or question mark and CANT standalone
    &:  && (logical and)
    
    //RULE: Can repeat 3 times or be trailed by an equals sign
    <:  < (less), <= (less or equal), << (shift left)
    >:  > (greater), >= (greater or equal), >> (shift right)
    */
    // Operator handler
    if (c == '$' || c == '+' || c == '-' || c == '*' || c == '/'
        || c == '%' || c == '=' || c == '!'  || c == '|'
        || c == '^' || c == '&' || c == '<' || c== '>') {
        // Check for consecutive operators
        if (last_token_type == 'o' && c != '!' && c != '$') {
            token.error = ERROR_CONSECUTIVE_OPERATORS;
            token.lexeme[0] = c;
            token.lexeme[1] = '\0';
            (*pos)++;
            return token;
        }

        //Determine first-char logic
        char c_next = input[*pos + 1]; // c_next should always be in array bound, if passed in string was completed with a null terminal.
        switch (c) {
            //Can be trailed by one repetition or an equals sign
            case '+':
            case '-':
                // +, - case
                token.lexeme[0] = c;
                token.lexeme[1] = '\0';
                token.type = TOKEN_OPERATOR;
                *pos += 1;
                last_token_type = 'o'; // operator
                break;

            case '*':
            case '/':
            case '%':
                // *, /, %,
                token.lexeme[0] = c;
                token.lexeme[1] = '\0';
                token.type = TOKEN_OPERATOR;
                *pos += 1;
                last_token_type = 'o'; // operator
                break;

            case '=':
                if (c_next == '=') {
                    // == case
                    token.lexeme[0] = c;
                    token.lexeme[1] = c_next;
                    token.lexeme[2] = '\0';
                    token.type = TOKEN_COMPARITIVE;
                    *pos += 2;
                    last_token_type = 'c'; // comparative
                } else {
                    // =
                    token.lexeme[0] = c;
                    token.lexeme[1] = '\0';
                    token.type = TOKEN_EQUALS;
                    *pos += 1;
                    last_token_type = 'e'; // equals
                }
                break;

            //Can be chained together as many times as you want !!!!true, one is comparative other is operator
            case '!':
                if (c_next == '=') {
                    //!= case
                    token.lexeme[0] = c;
                    token.lexeme[1] = c_next;
                    token.lexeme[2] = '\0';
                    token.type = TOKEN_COMPARITIVE;
                    *pos += 2;
                    last_token_type = 'c'; // comparative
                } else {
                    token.lexeme[0] = c;
                    token.lexeme[1] = '\0';
                    token.type = TOKEN_OPERATOR;
                    *pos += 1;
                    last_token_type = 'u'; // repeatable operator (unary)
                }
                break;
            
            // Can be trailed only by itself, one is comparative and other is operator
            case '|':
                if (c_next == c) {
                    // ||
                    token.lexeme[0] = c;
                    token.lexeme[1] = c_next;
                    token.lexeme[2] = '\0';
                    token.type = TOKEN_OPERATOR;
                    *pos += 2;
                    last_token_type = 'o'; // comparative
                }
                break;

            // Can be trailed only by itself, both are operators
            case '^':
                if (c_next == c) {
                    // ^^
                    token.lexeme[0] = c;
                    token.lexeme[1] = c_next;
                    token.lexeme[2] = '\0';
                    token.type = TOKEN_OPERATOR;
                    *pos += 2;
                    last_token_type = 'o'; // operator
                }
                break;

            //Can be trailed by itself or question mark and CANT standalone
            case '&':
                if (c_next == c) {
                    // &&
                    token.lexeme[0] = c;
                    token.lexeme[1] = c_next;
                    token.lexeme[2] = '\0';
                    token.type = TOKEN_OPERATOR;
                    *pos += 2;
                    last_token_type = 'c'; // comparative
                }
                break;

            //RULE: Can repeat 3 times or be trailed by an equals sign
            case '<':
            case '>':
                if (c_next == '=') {
                    // <=, >=
                    token.lexeme[0] = c;
                    token.lexeme[1] = c_next;
                    token.lexeme[2] = '\0';
                    token.type = TOKEN_COMPARITIVE;
                    *pos += 2;
                    last_token_type = 'o'; // operator
                } else {
                    // <, >
                    token.lexeme[0] = c;
                    token.lexeme[1] = '\0';
                    token.type = TOKEN_COMPARITIVE;
                    *pos += 1;
                    last_token_type = 'o'; // operator
                }
                break;

            case '$':
                // $ is factorial
                token.lexeme[0] = c;
                token.lexeme[1] = '\0';
                token.type = TOKEN_FACTORIAL;
                *pos += 1;
                last_token_type = 'u'; //technically infinitely repeatable $$5 so unary
                break;

            // If it somehow caught the operator but couldn't identify it, this catches it
            default:
                printf("[WARN]: Character %c was accepted by if statement but not assigned a case. Assuming standalone operator.\n", c);
        }
        // "finally the token can return to the main function. May he finally rest..."
        return token;
    }

    // Brackets, parentheses, and brace handler
    if (c == '(') {
        token.type = TOKEN_LEFTPARENTHESES;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        last_token_type = 'b'; //brackets (any type)
        (*pos)++;
        return token;
    }

    if (c == ')') {
        token.type = TOKEN_RIGHTPARENTHESES;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        last_token_type = 'b'; //brackets (any type)
        (*pos)++;
        return token;
    }

    if (c == '{') {
        token.type = TOKEN_LEFTBRACE;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        last_token_type = 'b'; //brackets (any type)
        (*pos)++;
        return token;
    }

    if (c == '}') {
        token.type = TOKEN_RIGHTBRACE;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        last_token_type = 'b'; //brackets (any type)
        (*pos)++;
        return token;
    }

    if (c == '[') {
        token.type = TOKEN_LEFTBRACKET;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        last_token_type = 'b'; //brackets (any type)
        (*pos)++;
        return token;
    }

    if (c == ']') {
        token.type = TOKEN_RIGHTBRACKET;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        last_token_type = 'b'; //brackets (any type)
        (*pos)++;
        return token;
    }

    // SemiColon needs it own handler
    if (c == ';') {
        token.type = TOKEN_SEMICOLON;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        last_token_type = 'd'; //delimiter
        (*pos)++;
        return token;
    }

    // Other delimiters
    if (c == ',') {
        token.type = TOKEN_COMMA;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        last_token_type = 'd'; //delimiter
        (*pos)++;
        return token;
    }

    // Handle invalid characters
    token.error = ERROR_INVALID_CHAR;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    last_token_type = 'e'; //error
    (*pos)++;
    return token;
}
/*
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

    // start at beginning of buffer
    int position = 0;
    Token token;

    // perform tokenization
    printf("Analyzing Correct Input:\n%s\n\n", buffer);
    do {
        token = get_next_token(buffer, &position);
        print_token(token);
    } while (token.type != TOKEN_EOF);

    // free memory and close file
    free(buffer);
    fclose(file);

    // Repeat for Incorrect file
    current_line = 1;

    // get file
    file = fopen("../phase2-w25/test/input_invalid.txt", "r");
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

    // start at beginning of buffer
    position = 0;

    // perform tokenization
    printf("Analyzing Incorrect Input:\n%s\n\n", buffer);
    do {
        token = get_next_token(buffer, &position);
        print_token(token);
    } while (token.type != TOKEN_EOF);

    // free memory "he ain't deserve to be locked up"
    free(buffer);
    fclose(file);
    return 0;
}
*/