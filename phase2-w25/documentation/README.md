# SeaPlus+ DOCUMENTATION
# SeaPlus+ LEXER
## Keywords
|Category| Keywords                     |
|--------|------------------------------|
|**Logic/Conditionals:**| if, else                     |
|**Logic:**| while, repeat, until, break  |
|**IO:**| print                        |
|**Data Types:**| int, char, string            |
|**Functions:**| func                         |
|**Misc:**| null, $ (factorial function) |

## Operators
|||               ||                |               |              |
|--|--|---------------|---|----------------|---------------|--------------|
|+ (add)|- (sub)| * (mult)      |/ (div)| % (mod)        | \|\| (log or) | && (log and) |
|^^ (power)|<= (less-eq)| \>= (grt-eq)  |= (assign)| == (log eq)    | !=(log not)   | ! (not)      
|< (less)| \> (grt)||||||
## Delimiters and Punctuation
||||||    |||
|---|---|---|---|---|----|---|---|
|,|;|{|}|(| )  |[|]|

## String Literals
Strings can be up to 100 characters in length (max 99 other characters followed by \0).
Strings exceeding this length will return an ERROR_STRING_OVERFLOW type token.
Acceptable characters include all Alphanumeric symbols, punctation, and whitespace (though some of these require escape characters to parse correctly).

### Escape Characters

|||||||
|---|---|---|---|---|---|
|\\\\ |\\'|\\"|\\n|\\r|\\t|

Unrecognized/Invalid escape characters (not listed above) will return an ERROR_INVALID_ESCAPE_CHARACTER.
```
"Strings look like this"
```
Unclosed double-quotes will return an ERROR_UNTERMINATED_STRING.

## Char Literals
Char literals are only 1 character in length, and support the same character set as String Literals.
Tokens exceeding this length or missing with unclosed single-quotes will return an ERROR_UNTERMINATED_CHARACTER.
```
'a'
'\t'
'2'
```

## Special Characters
||||
|---|---|---|
|\_|&|
## Comments
Single line:
```
#Single line comments look like this
```
Multi line:
```
/*Multi line comments look  
like this*/
```
# SeaPlus+ PARSER
The SeaPlus+ Parser converts SeaPlus+ source code into an Abstract Syntax Tree (AST). It enforces syntactic rules and error handling to ensure valid program execution

## Variable Declaration
Parses int, char, and string declarations. The parser does not check that a given variable name already exists.
```
# valid cases
int x;
char y;
string z;


# invalid cases
int x       # no semi-colon
char 10x    # cannot start with numbers (must be alpha or _)
string ;    # must have associated identifier
```
## Assignments
Parses assignment statements. Declaration and Assignment cannot be done in the same line.
The parser does not check if the type of assignment matches the identifiers variable type.
```
# valid cases
x = 5;
y = 'c';
z = "Hello World!\n"


# invalid cases
int x = 5;  # cannot declare and assign in same statement
y = '\l';   # invalid escape character
```
## Expressions
Parses numerical, logical, and string expressions. See operators table for valid operators.
## Conditionals
Parses if-else blocks. One line if and else blocks without {} are not supported. 
```
# valid cases
if (x == 5){
    # do something
} else {
    # do something
}

if (x != 10){
    # do something
}


# invalid cases
if (x == 5) # do something ; # must have {} to contain operations

if (x == 5){
    # do something
} else if (x == 4){ # currently we do not support else if statements (elif does not count)
    # do something
}

```
## Loops
Parses while and repeat-until loops of form seen below:
```
# valid cases
while(expression){
    # do something
}
repeat{
    # do something
}until(expression);


# invalid cases
while(){ # while must have an expression within ()
    # do something
}

repeat{
    # do something
}until() # until must have an expression within ()
```
## Functions
Parses functions, because declaration and assignment cannot be done at the same time, functions do not support default values.
```
# valid cases
func functionName(int x, int y){
    # do something
}

func functionName(){
    # do something
}


# invalid cases
func functionName(int x,){ # improper expression format
    # do something
}

func functionName(int x,); # forward function declarations not supported
```
## Print Statements 
Parses print operations. Can accept any expression or value.
```
print(expression);
```
## Block Parsing
For any keyword that functions using repeating/callable code (functions, loops, if and else statments), 
the block parser is responsible for creating and organizing the AST nodes of the given code.


## Parser Error Generation
Below is  a list of all errors caught by the system  during the parsing phase.

| **Error Type**                      | **Error Description**                                            |
|-------------------------------------|------------------------------------------------------------------|
| **PARSE_ERROR_NONE**                | No parsing error occurred.                                       |
| **PARSE_ERROR_UNEXPECTED_TOKEN**    | Encountered a token that doesn't fit the expected syntax.        |
| **PARSE_ERROR_UNEXPECTED_EOF**      | Reached the end of the file unexpectedly while parsing.          |
| **PARSE_ERROR_UNEXPECTED_OPERATOR** | Operator appears in an invalid position in an expression.        |
| **PARSE_ERROR_MISSING_SEMICOLON**   | Missing semicolon (`;`) at the end of a statement.               |
| **PARSE_ERROR_MISSING_IDENTIFIER**  | Expected a variable, function name, or parameter but found none. |
| **PARSE_ERROR_MISSING_EQUALS**      | Assignment statement is missing the equals sign (`=`).           |
| **PARSE_ERROR_INVALID_EXPRESSION**  | Encountered an invalid or incomplete expression.                 |
| **PARSE_ERROR_MISSING_PAREN**       | Expected opening `(` or closing `)` but not found.               |
| **PARSE_ERROR_MISSING_CONDITION**   | `if`, `while`, or `repeat-until` statement missing a condition.  |
| **PARSE_ERROR_MISSING_BRACE**       | Expected `{` or `}` but not found in a block.                    |
| **PARSE_ERROR_MISSING_COLON**       | Expected `:` in a control structure (e.g., `case` statement).    |
| **PARSE_ERROR_FUNC_CALL**           | Function call has incorrect syntax or structure.                 |
| **PARSE_ERROR_FUNC_DECLARATION**    | Function declaration has incorrect syntax or structure.          |
| **PARSE_ERROR_MISSING_RETURN**      | Function is missing a `return` statement before it ends.         |
| **PARSE_ERROR_INVALID_FUNC_CALL**   | Function call has incorrect argument count or type mismatch.     |
| **PARSE_ERROR_BREAK_OUTSIDE_LOOP**  | `break` statement is used outside of a loop context.             |
| **PARSE_ERROR_INVALID_CONDITION**   | Condition in a control structure is missing or malformed.        |
