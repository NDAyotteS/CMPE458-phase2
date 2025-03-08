#ifndef OPERATORS_H
#define OPERATORS_H

#endif //OPERATORS_H

//based on c/cpp operator precedence
//https://en.cppreference.com/w/c/language/operator_precedence
//todo: add this to documentation

typedef enum operatorCode {
  OPCODE_ID=0, //used only for parse table
  OPCODE_NOT, //NOT is left assoc
  OPCODE_FACTORIAL, //factorials take math precedence over power, left assoc
  OPCODE_POWER, //right assoc
  OPCODE_MULTIPLY, OPCODE_DIVIDE, OPCODE_MOD, //all left assoc
  OPCODE_ADD, OPCODE_SUB, //left assoc
  OPCODE_GREATER, OPCODE_GREAT_EQ, OPCODE_LESS_EQ, OPCODE_LESSER, //left assoc, basing on C grammar
  OPCODE_LOG_EQ, OPCODE_NOT_EQ, //left assoc, basing on C grammar
  OPCODE_LOG_AND, //left assoc, basing on C grammar
  OPCODE_LOG_OR, //left assoc, basing on C grammar
} operatorCode_t;

//First index is row number, equal to TOS via enum. Second index gets col with index LookAhead.
//1 for TOS is higher precedence, -1 for LookAhead is higher precedence (therefore TOS is lower), 0 for null case (id on id, $ on $)
int operatorParseTable[19][19] =
{
    /*
    {id,     not,    fact,   pow,    mult,   div,    mod,    add,    sub,    grt,    geq,    leq,    less,   logEq,   notEq, logAnd, logOr}
    */

    //id
    {0,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1},
    //not
    {-1,     1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1},
    //fact
    {-1,    -1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1},
    //pow
    {-1,    -1,     -1,     -1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1},
    //mult
    {-1,    -1,     -1,     -1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1},
    //div
    {-1,    -1,     -1,     -1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1},
    //mod
    {-1,    -1,     -1,     -1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1},
    //add
    {-1,    -1,     -1,     -1,     -1,     -1,     -1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1},
    //sub
    {-1,    -1,     -1,     -1,     -1,     -1,     -1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1},
    //grt
    {-1,    -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,      1,      1,      1,      1,      1,      1,      1,      1},
    //geq
    {-1,    -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,      1,      1,      1,      1,      1,      1,      1,      1},
    //leq
    {-1,    -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,      1,      1,      1,      1,      1,      1,      1,      1},
    //less
    {-1,    -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,      1,      1,      1,      1,      1,      1,      1,      1},
    //logEq
    {-1,    -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,      1,      1,      1,      1},
    //notEq
    {-1,    -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,      1,      1,      1,      1},
    //logAnd
    {-1,    -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,      1,      1},
    //logOr
    {-1,    -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,      1},
};
//was that too much voodoo?