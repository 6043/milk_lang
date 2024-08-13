#include <string>
#include "tokens.h"
#include <vector>
#include <variant>

#pragma once

using namespace std;

// skeleton for AST
enum statement_type
{
    STMT_ASSIGN,
    STMT_IF_THEN,
    STMT_GOTO,
    STMT_PRINT, // make this a function in later versions?
    STMT_LABEL,
    STMT_WHILE_LOOP,
    STMT_BREAK,
    STMT_DECLARATION
};

enum expr_type
{
    UNARY_EXPR,
    BINARY_EXPR_PLUS,
    BINARY_EXPR_MINUS,
    BINARY_EXPR_MULT,
    BINARY_EXPR_DIV,
    BINARY_EXPR_MOD,
    BINARY_EXPR_EXP,
    BINARY_EXPR_RIGHT_SHIFT
};

enum term_kind
{
    TERM_INPUT,
    TERM_INT_LITERAL,
    TERM_REAL_LITERAL,
    TERM_IDENTIFIER,
};

enum identifier_type
{
    TYPE_INT,
    TYPE_REAL,
    TYPE_BOOL,
    TYPE_INVALID,
};

struct term_node
{
    term_kind kind;
    string value; // may need union for this later, rn we are just storing identifiers and ints.
};

struct term_binary_node
{
    term_node lhs;
    term_node rhs;
};

struct expr_node
{
    expr_type kind;
    variant<term_binary_node, term_node> expr;
};

struct assign_node
{
    string identifier;
    expr_node expr;
};

struct declaration_node
{
    string identifier;
    identifier_type type;
    expr_node expr;
};

enum comparison_type
{
    COMP_LESS_THAN,
    COMP_GREATER_THAN,
    COMP_EQUAL,
    COMP_NOT_EQUAL
};

struct comparison_node
{
    comparison_type kind;
    variant<term_binary_node> binary_expr;
    bool is_not;
};

struct statement_node;
struct if_then_node
{
    comparison_node comparison;
    vector<statement_node *> statements;
};

struct goto_node
{
    string label;
};

struct print_node
{
    term_node term;
};

struct label_node
{
    string label;
};

struct while_loop_node
{
    comparison_node comparison;
    vector<statement_node *> statements;
};

struct statement_node
{
    statement_type kind;
    variant<assign_node, if_then_node, goto_node, print_node, label_node, while_loop_node, declaration_node> statement;
};

struct program_node
{
    vector<statement_node> statements;
};

// parser object
class Parser
{
private:
    vector<Token> tokens;
    unsigned int pos;

public:
    // constructor
    Parser(vector<Token> tokens);

    // called when invalid syntax/parsing error
    void error(const string &expected);

    // returns current token in tokenfeed
    Token current_token();

    // advances to next token in the tokenfeed
    void advance();

    // parses a list of tokens into a program. returns a program node (AST representation).
    program_node parse_program();

    statement_node parse_statement();

    // statement node parsers
    statement_node parse_assignment();
    statement_node parse_if_then();
    statement_node parse_goto();
    statement_node parse_print();
    statement_node parse_label();
    statement_node parse_while_loop();
    statement_node parse_declaration();

    expr_node parse_expr();

    term_node parse_term();

    comparison_node parse_comparison();
};

// prints AST of program
void print_program(program_node program);
void print_statement(statement_node stmt);
void print_assign(assign_node assign);
void print_expr(expr_node expr);
void print_goto(goto_node goto_);
void print_if_then(if_then_node if_then);
void print_print(print_node print);
void print_label(label_node label);
void print_while_loop(while_loop_node while_loop);