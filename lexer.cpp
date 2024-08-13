#include "lexer.h"
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

// constructor
Lexer::Lexer(const string &buffer) : buffer(buffer), pos(0), read_pos(0), line_number(1), current_char(0)
{
    this->read();
};

// peeks at next char in buffer (doesnt move pos)
char Lexer::peek()
{
    if (this->read_pos >= this->buffer.size())
    {
        return EOF;
    }

    return this->buffer[this->read_pos];
}

// reads and moves pos of next char in buffer
char Lexer::read()
{
    this->current_char = this->peek();

    this->pos = this->read_pos;
    this->read_pos += 1;

    return this->current_char;
}

// skips whitespace in buffer
void Lexer::skip_whitespace()
{
    while (isspace(this->current_char))
    {
        if (this->current_char == '\n')
            line_number++;
        this->read();
    }
}

// returns true if its a keyword, false otherwise. stores token_type in &kind.
bool is_keyword(const string &buf, token_type &kind)
{
    if (buf == "input")
    {
        kind = INPUT;
        return true;
    }
    else if (buf == "moo" || buf == "print")
    {
        kind = PRINT;
        return true;
    }
    else if (buf == "goto")
    {
        kind = GOTO;
        return true;
    }
    else if (buf == "if")
    {
        kind = IF;
        return true;
    }
    else if (buf == "then")
    {
        kind = THEN;
        return true;
    }
    else if (buf == "while")
    {
        kind = WHILE;
        return true;
    }
    else if (buf == "do")
    {
        kind = DO;
        return true;
    }
    else if (buf == "true")
    {
        kind = BOOL_TRUE;
        return true;
    }
    else if (buf == "false")
    {
        kind = BOOL_FALSE;
        return true;
    }
    else if (buf == "break")
    {
        kind = BREAK;
        return true;
    }
    else if (buf == "int")
    {
        kind = KEYW_INT;
        return true;
    }
    else if (buf == "real")
    {
        kind = KEYW_REAL;
        return true;
    }
    else if (buf == "bool")
    {
        kind = KEYW_BOOL;
        return true;
    }
    else
    {
        return false;
    }
}

// returns next token in buffer
Token Lexer::next_token()
{
    this->skip_whitespace();

    // check current character
    if (this->current_char == EOF)
    {
        this->read();
        return Token(END_OF_FILE, "", this->line_number);
    }
    else if (this->current_char == '<')
    {
        this->read();
        return Token(OP_LT, "", this->line_number);
    }
    else if (this->current_char == '>')
    {
        this->read();
        if (this->current_char == '>')
        {
            this->read();
            return Token(OP_RIGHT_SHIFT, "", this->line_number);
        }
        return Token(OP_GT, "", this->line_number);
    }
    else if (this->current_char == '+')
    {
        this->read();
        return Token(OP_PLUS, "", this->line_number);
    }
    else if (this->current_char == '-')
    {
        this->read();
        return Token(OP_MINUS, "", this->line_number);
    }
    else if (this->current_char == '*')
    {
        this->read();
        if (this->current_char == '*') // if * follows *
        {
            this->read();
            return Token(OP_EXPONENT, "", this->line_number);
        }
        return Token(OP_MULT, "", this->line_number);
    }
    else if (this->current_char == '!')
    {
        this->read();
        if (this->current_char == '=') // if * follows *
        {
            this->read();
            return Token(NOTEQUAL, "", this->line_number);
        }
        return Token(NOT, "", this->line_number);
    }
    else if (this->current_char == '%')
    {
        this->read();
        return Token(OP_MOD, "", this->line_number);
    }
    else if (this->current_char == '/')
    {
        this->read();
        return Token(OP_DIV, "", this->line_number);
    }
    else if (this->current_char == '{')
    {
        this->read();
        return Token(OPEN_BRACKET, "", this->line_number);
    }
    else if (this->current_char == '}')
    {
        this->read();
        return Token(CLOSE_BRACKET, "", this->line_number);
    }
    // add support for == soon !
    else if (this->current_char == '=')
    {
        this->read();
        if (this->current_char == '=') // if = follows =
        {
            this->read();
            return Token(EQUALEQUAL, "", this->line_number);
        }
        return Token(EQUAL, "", this->line_number);
    }
    // label !?
    else if (this->current_char == ':')
    {
        string buf = "";
        this->read(); // advance from :

        // get label name
        while (isalnum(this->current_char) || this->current_char == '_')
        {
            buf.push_back(this->current_char);
            this->read();
        }

        return Token(LABEL, buf, this->line_number);
    }
    // int or real
    else if (isdigit(this->current_char) || this->current_char == '.')
    {
        string buf = "";
        int is_real = 0;
        // collect int
        while (isdigit(this->current_char) || this->current_char == '.')
        {
            if (this->current_char == '.')
            {
                is_real++;
            }
            if (is_real > 1)
            {
                return Token(REAL_LITERAL, buf, this->line_number);
            }
            buf.push_back(this->current_char);
            this->read();
        }

        if (is_real)
        {
            return Token(REAL_LITERAL, buf, this->line_number);
        }
        else
        {
            return Token(INT_LITERAL, buf, this->line_number);
        }
    }
    // idenitifer or other alpha keyword!
    else if (isalnum(this->current_char))
    {
        string buf = "";

        // collect buffer
        while (isalnum(this->current_char) || this->current_char == '_')
        {
            buf.push_back(this->current_char);
            this->read();
        }

        // check to see if its a keyword or not
        token_type kind;
        if (is_keyword(buf, kind))
        {
            // if its a keyword, return that.
            if (kind == BOOL_TRUE)
            {
                return Token(INT_LITERAL, "1", this->line_number);
            }
            else if (kind == BOOL_FALSE)
            {
                return Token(INT_LITERAL, "0", this->line_number);
            }
            return Token(kind, "", this->line_number);
        }
        // else, it should be an identifier.
        return Token(IDENTIFIER, buf, this->line_number);
    }
    // INVALID TOKEN!!!
    else
    {
        this->read();
        return Token(INVALID, string(1, this->current_char), this->line_number);
    }
}

// returns a vector of tokens tokenized from a buffer string.
vector<Token> Lexer::tokenize()
{
    vector<Token> tokens;

    // lex until end of file
    Token current_token = this->next_token();
    while (current_token.kind != END_OF_FILE)
    {
        tokens.push_back(current_token);
        current_token = this->next_token();
    }

    tokens.push_back(current_token);

    return tokens;
}

// reads contents of file into &buf.
string read_file(const string &filename)
{
    ifstream file(filename); // Open the file
    if (!file)
    {
        throw std::runtime_error("Could not open file");
    }

    std::stringstream buffer;
    buffer << file.rdbuf(); // Read the file content into the stringstream
    return buffer.str();
}

void print_tokens(vector<Token> &tokens)
{
    for (auto token : tokens)
    {
        {
            if (token.value != "")
            {
                cout << token.repr() << " (" << token.value << ") @ line " << token.line_number << endl;
            }
            else
            {
                cout << token.repr() << " @ line " << token.line_number << endl;
            }
        }
    }
}