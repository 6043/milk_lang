#include "parser.h"
#include <fstream>
#include <string>
#pragma once

struct variable
{
    string identifier;
    identifier_type type;
};

class Generator
{
private:
    program_node program;
    // stores all the variable names used in the program.
    // helps check for any semantic errors.
    vector<variable> variables;
    vector<string> labels;
    unsigned int current_if_index;
    unsigned int current_loop_index;
    unsigned int buffer_ptr; // 24 bytes buffer to flush text to console
    ofstream file;
    vector<string_> strings_;

public:
    // constructor
    Generator(program_node program, string &output_filename, vector<string_> strings_);

    // CODE GENERATORS

    // generates x86 assembly instructions based on program_node/AST.
    void to_asm(bool logging = false);

    // generates x86 assmebly for a statement
    void statement_to_asm(statement_node stmt);

    void label_to_asm(label_node label);
    void goto_to_asm(goto_node goto_);
    void assign_to_asm(assign_node assign);
    void array_assign_to_asm(array_assign_node assign);
    void if_then_to_asm(if_then_node if_then);
    void print_to_asm(print_node print);
    identifier_type expr_to_asm(expr_node expr, string temp_register = "%rcx");
    void comparison_to_asm(comparison_node comp);
    identifier_type term_to_asm(term_node term, identifier_type expect = TYPE_INT);
    void input_to_asm(term_node term);
    void random_to_asm();
    void while_loop_to_asm(while_loop_node while_loop);
    void for_loop_to_asm(for_loop_node for_loop);
    void declaration_to_asm(declaration_node decl);
    void array_declaration_to_asm(array_declare_node decl);

    // HELPERS

    int variable_index(string var);
    int string_index(string str);

    void collect_variables(vector<statement_node> stmts);
    void collect_labels();
    void collect_all_while_loops();
    void collect_all_for_loops();
    void collect_all_if_thens();
    void collect_if_then(if_then_node if_then);
    void collect_while_loops(while_loop_node while_loop);
    void collect_for_loops(for_loop_node for_loop);

    void update_buffer_ptr();

    bool statement_valid(statement_node stmt);
    bool comparison_valid(comparison_node comp);
    bool goto_valid(goto_node goto_);

    // checks if an expr has undefined variables (semantic errors)
    bool expr_valid(expr_node expr);

    // checks if a term has undefined variables (semantic errors)
    bool term_valid(term_node term);
    bool is_defined(string var);
    identifier_type get_type(string var);
    identifier_type expected_binary_expr_result(term_binary_node binary_expr);
};

template <typename T>
bool in_vector(vector<T> vec, T value);