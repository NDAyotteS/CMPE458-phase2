# SeaPlus+ DOCUMENTATION
# SeaPlus+ LEXER
## Keywords
|Category| Keywords                     |
|--------|------------------------------|
|**Logic/Conditionals:**| if, else                     |
|**Logic:**| while, repeat, until, break  |
|**IO:**| print                        |
|**Data Types:**| int, char, string            |
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
# SeaPlus+ PARSER and SEMANTICS
The SeaPlus+ Parser converts SeaPlus+ source code into an Abstract Syntax Tree (AST). It enforces syntactic 
rules and error handling to ensure valid program execution

## Variable Declaration
Parses int, char, and string declarations. The parser does not check that a given variable name already exists 
and will accept anything that meets the grammatical requirements of the type.
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
The parser does not check if the type of assignment matches the identifiers variable type and only worries about
the statement matching the grammatical requirements of "identifier = expression;"

NOTE: expression can simply be a number, character, or string and the code handles that separately.
```
# valid cases
x = 5 + 100;
y = 'c';
z = "Hello World!\n"


# invalid cases
int x = 5;  # cannot declare and assign in same statement
y = '\l';   # invalid escape character
```
## Expressions
Parses numerical, logical, and string expressions. See operators table for valid operators.

Any expression is valid as long as it meets these rules:

1. all operators exist (lexer can determine token type)
2. no repeated operators (! is an exception)
3. no repeated identifiers 
4. all parentheses must be closed 
5. must end in ;

The parser does not check that types are valid, as such a string could be added to an int at this phase 
and be considered valid.
## Conditionals
Only supports if and else blocks.

One line if and else blocks without {} are not supported.

The cases within parentheses cannot be empty and must contain some kind of expression.

Does not currently support else if or elif.
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

if (){ # does not have an expression
    # do something
}

if (x == 5){
    # do something
} else if (x == 4){ # currently we do not support else if statements (elif does not count)
    # do something
}

```
## Loops
Parses while and repeat-until loops.

All loops must have an expression as their loop case.

The parser does not check that the given expression is a logical one, and as such non-logical 
statements (like the expression 5+5) would be accepted and considered okay.

The expression must of be of numerical type, meaning int and identifiers of int type can be used in
comparisons only. String comparisons do not work semantically.
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

## Print Statements 
Parses print operations. Can accept any expression or value. Has no restrictions on what the expression is.

The expression must of be of one type, as in it must be either just a string, char, int,
or expression that can be evaluated. You cannot mix strings and ints, and you cannot chain together
statements like in traditional c.
```
# Valid Case
print(expression);

# Invalid Case
print(expression, expression);

print();
```

## Factorial Statements
Parses factorial operations. Can accept any expression or value. Has no restrictions on what the expression is. 

The function can be used inside an expression, it had the highest level of precedence (like a bracketed expression would)
```
// valid cases
x = $(expression);

x = 5 + $(expression) + 10;

// invalid cases
x = $expression; // lacks brackets

$(expression); // on its own invalid

x = 5 + $(expression); + 10; // Semi-colon only necessary when used as the whole expression
```

## Block Parsing
For any keyword that functions using repeating/callable code (functions, loops, if and else statments), 
the block parser is responsible for creating and organizing the AST nodes of the given code.


## Parser Error Generation
Below is a list of all errors caught by the system  during the parsing phase.

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

## Semantic Error Generation

| **Error Type**                      | 
|-------------------------------------|
| **SEM_ERROR_NONE**|
| **SEM_ERROR_UNDECLARED_VARIABLE**|
| **SEM_ERROR_REDECLARED_VARIABLE**|
| **SEM_ERROR_TYPE_MISMATCH**|
| **SEM_ERROR_UNINITIALIZED_VARIABLE**|
| **SEM_ERROR_INVALID_OPERATION**|
| **SEM_ERROR_SEMANTIC_ERROR**|
