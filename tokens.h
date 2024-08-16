#include <string>

#pragma once

using namespace std;

enum token_type
{
    IDENTIFIER,
    STR_LITERAL,
    INT_LITERAL,
    REAL_LITERAL,
    INPUT,
    RANDOM,
    PRINT,
    GOTO,
    IF,
    THEN,
    WHILE,
    DO,
    OPEN_BRACKET,  // {
    CLOSE_BRACKET, // }
    OP_LT,
    OP_GT,
    OP_PLUS,
    OP_MINUS,
    OP_MULT,
    OP_DIV,
    OP_MOD, // %
    OP_EXPONENT,
    LABEL,
    EQUAL,      // =
    EQUALEQUAL, // ==
    NOT,        // !
    NOTEQUAL,   // !=
    INVALID,
    END_OF_FILE,
    BOOL_TRUE,
    BOOL_FALSE,
    BREAK,
    KEYW_INT,
    KEYW_BOOL,
    KEYW_REAL,
    KEYW_STR,
    OP_RIGHT_SHIFT, // >>
    FOR,
    COMMA,
    SEMICOLON,
    PERIOD,
    OPEN_BRACE,
    CLOSE_BRACE
};

class Token
{
public:
    token_type kind;
    string value;
    unsigned int line_number;

    Token(token_type kind, string value, unsigned int line_number);

    string repr();
};
