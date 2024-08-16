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
    STMT_ARRAY_ASSIGN,
    STMT_IF_THEN,
    STMT_GOTO,
    STMT_PRINT, // make this a function in later versions?
    STMT_LABEL,
    STMT_WHILE_LOOP,
    STMT_FOR_LOOP,
    STMT_BREAK,
    STMT_DECLARATION,
    STMT_ARRAY_DECLARATION,
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
    TERM_RANDOM,
    TERM_INT_LITERAL,
    TERM_REAL_LITERAL,
    TERM_STR_LITERAL,
    TERM_IDENTIFIER,
    TERM_ARRAY_INT_LITERAL,
    TERM_ARRAY_REAL_LITERAL,
    TERM_ARRAY_ACCESS,
};

enum identifier_type
{
    TYPE_INT,
    TYPE_REAL,
    TYPE_BOOL,
    TYPE_STR,
    TYPE_ARRAY_INT,
    TYPE_ARRAY_REAL,
    TYPE_INVALID,
};

struct string_
{
    string value;
    int length;
};

struct expr_node;
struct term_node
{
    term_kind kind;
    string value;          // may need union for this later, rn we are just storing identifiers and ints.
    expr_node *index_expr; // optional, needed for TERM_ARRAY_ACCESS
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

struct array_assign_node
{
    string identifier;
    expr_node expr;
    expr_node index_expr;
};

struct array_access_node
{
    string identifier;
    identifier_type type;
    expr_node index_expr;
};

struct array_declare_node
{
    string identifier;
    identifier_type type;
    expr_node len_expr;
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

struct for_loop_node
{
    declaration_node declaration;
    comparison_node comparison;
    assign_node assign;
    vector<statement_node *> statements;
};

struct statement_node
{
    statement_type kind;
    variant<assign_node, if_then_node, goto_node, print_node, label_node, while_loop_node, for_loop_node, declaration_node, array_declare_node, array_assign_node> statement;
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

    Token peek();

    // parses a list of tokens into a program. returns a program node (AST representation).
    program_node parse_program();

    vector<string_> get_strings();
    bool in_strings_(vector<string_> strings_, string str);

    statement_node parse_statement();

    // statement node parsers
    statement_node parse_assignment();
    statement_node parse_if_then();
    statement_node parse_goto();
    statement_node parse_print();
    statement_node parse_label();
    statement_node parse_while_loop();
    statement_node parse_for_loop();
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
void print_for_loop(for_loop_node for_loop);
void print_array_declare(array_declare_node decl);
void print_array_assign(array_assign_node assign);
void print_term(term_node term);