#include <string>
#include "tokens.h"
#include <vector>

#pragma once

using namespace std;

class Lexer
{
private:
    string buffer;
    unsigned int pos;
    unsigned int read_pos;
    unsigned int line_number;
    char current_char;

public:
    // constructor
    Lexer(const string &buffer);

    // peeks at char in buffer
    char peek();

    // reads char, moves to next in buffer
    char read();

    // skips until current char isnt space
    void skip_whitespace();

    // gets next token in buffer
    Token next_token();

    // returns a vector of tokens tokenized from a buffer string.
    vector<Token> tokenize();
};

// returns true if its a keyword, false otherwise. stores token_type in &kind.
bool is_keyword(const string &buf, token_type &kind);

// reads contents of file into &buf.
string read_file(const string &filename);

void print_tokens(vector<Token> &tokens);