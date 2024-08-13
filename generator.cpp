#include "generator.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "util.h"

using namespace std;

// constructor
Generator::Generator(program_node program, string &output_filename) : program(program), current_if_index(0), current_while_index(0), file(output_filename)
{
    if (!file)
    {
        cout << "Couldn't open output file" << endl;
        exit(0);
    }
};

// generates x86 assembly instructions based on program_node/AST.
void Generator::to_asm()
{
    this->collect_variables(this->program.statements);
    this->collect_labels();
    this->collect_all_while_loops();
    this->collect_all_if_thens();

    if (!file.is_open())
    {
        cout << "file is closed bruh" << endl;
    }

    // INITIAL SETUP
    file << ".section .text" << endl;
    file << ".globl _start" << endl;
    file << "_start:" << endl;

    // setup stack frame
    file << "   mov %rsp, %rbp" << endl;
    // allocate memory for variables on the stack
    file << "   sub $" << this->variables.size() * 8 << ", %rsp" << endl;
    this->update_buffer_ptr();

    // allocate memory for buffer for inputs and printing to the screen
    file << "   sub $64, %rsp" << endl;
    for (statement_node stmt : this->program.statements)
    {
        this->statement_to_asm(stmt);
    }
    file << ".exit:" << endl;
    file << "   movq $60, %rax" << endl;
    file << "   xor %rdi, %rdi" << endl;
    file << "   syscall" << endl;

    // predefined functions
    file << endl;
    file << "int_to_ascii:" << endl;
    file << "   movq $10, %rbx" << endl;
    file << "   xor %rdx, %rdx" << endl;
    file << "   div %rbx" << endl;
    file << "   add $48, %dl" << endl;
    file << "   movb %dl, (%rcx)" << endl;
    file << "   inc %rsi" << endl;
    file << "   dec %rcx" << endl;
    file << "   test %rax, %rax" << endl;
    file << "   jnz int_to_ascii" << endl;
    file << "   inc %rcx" << endl;
    file << "   mov %rsi, %rdx" << endl;
    file << "   mov %rcx, %rsi" << endl;
    file << "   ret" << endl;

    file << endl;
    file << "ascii_to_int:" << endl;
    file << "   movb (%r9), %al" << endl;
    file << "   movzbq %al, %rax" << endl;
    file << "   sub $48, %rax" << endl;
    file << "   mul %rbx" << endl;
    file << "   add %rax, %r8" << endl;
    file << "   mov %rbx, %rax" << endl;
    file << "   mov $10, %rbx" << endl;
    file << "   mul %rbx" << endl;
    file << "   mov %rax, %rbx" << endl;
    file << "   dec %rcx" << endl;
    file << "   dec %r9" << endl;
    file << "   test %rcx, %rcx" << endl;
    file << "   jnz ascii_to_int" << endl;
    file << "   inc %r9" << endl;
    file << "   mov %r8, %rax" << endl;
    file << "   ret" << endl;

    file << endl;
    file << "exp:" << endl;
    file << "   test %rbx, %rbx" << endl;
    file << "   jz .endexp" << endl;
    file << "   mul %rcx" << endl;
    file << "   dec %rbx" << endl;
    file << "   jmp exp" << endl;
    file << ".endexp:" << endl;
    file << "   ret" << endl;

    file << endl;
    file << "double_to_ascii:" << endl;
    file << "   movsd %xmm0, %xmm1" << endl;
    file << "   cvttsd2si %xmm1, %r9" << endl;
    file << "   movq $10000000000, %rax" << endl;
    file << "   cvtsi2sd %rax, %xmm1" << endl;
    file << "   mulsd %xmm1, %xmm0" << endl;
    file << "   cvttsd2si %xmm0, %rax" << endl;
    file << "   movq $10000000000, %r8" << endl;
    file << "   xor %rdx, %rdx" << endl;
    file << "   div %r8" << endl;
    file << "   mov %rdx, %rax" << endl;
    file << ".double_to_ascii_loop:" << endl;
    file << "   mov %rax, %rbx" << endl;
    file << "   test %rax, %rax" << endl;
    file << "   jz .double_to_ascii_loop_end" << endl;
    file << "   xor %rdx, %rdx" << endl;
    file << "   movq $10, %r8" << endl;
    file << "   div %r8" << endl;
    file << "   test %rdx, %rdx" << endl;
    file << "   jz .double_to_ascii_loop" << endl;
    file << ".double_to_ascii_loop_end:" << endl;
    file << "   mov %rbx, %rax" << endl;
    file << "   call int_to_ascii" << endl;
    file << "   mov %rsi, %rcx" << endl;
    file << "   mov %rdx, %rsi" << endl;
    file << "   dec %rcx" << endl;
    file << "   movb $46, (%rcx)" << endl;
    file << "   inc %rsi" << endl;
    file << "   dec %rcx" << endl;
    file << "   mov %r9, %rax" << endl;
    file << "   call int_to_ascii" << endl;
    file << "   ret" << endl;
    file.close();
}

// generates x86 assmebly for a statement
void Generator::statement_to_asm(statement_node stmt)
{
    switch (stmt.kind)
    {
    case STMT_ASSIGN:
        assign_to_asm(get<assign_node>(stmt.statement));
        break;
    case STMT_GOTO:
        goto_to_asm(get<goto_node>(stmt.statement));
        break;
    case STMT_IF_THEN:
        if_then_to_asm(get<if_then_node>(stmt.statement));
        break;
    case STMT_LABEL:
        label_to_asm(get<label_node>(stmt.statement));
        break;
    case STMT_PRINT:
        print_to_asm(get<print_node>(stmt.statement));
        break;
    case STMT_WHILE_LOOP:
        while_loop_to_asm(get<while_loop_node>(stmt.statement));
        break;
    case STMT_DECLARATION:
        declaration_to_asm(get<declaration_node>(stmt.statement));
        break;
    default:
        break;
    }
}

// label:
void Generator::label_to_asm(label_node label)
{
    file << label.label << ":" << endl;
}

// jmp <label>
void Generator::goto_to_asm(goto_node goto_)
{
    if (!this->goto_valid(goto_))
        exit(0);
    file << "   jmp " << goto_.label << endl;
}

void Generator::assign_to_asm(assign_node assign)
{

    if (!this->expr_valid(assign.expr))
    {
        // sem error
        exit(0);
    }

    if (!this->is_defined(assign.identifier))
    {
        cout << "[leather] compilation error:" << endl
             << "    variable \"" << assign.identifier << "\" is not defined." << endl;
        exit(0);
    }

    int index = this->variable_index(assign.identifier);

    identifier_type t = this->variables[index].type;

    identifier_type expected = expr_to_asm(assign.expr);
    if (t != expected)
    {
        cout << "[leather] compilation error:" << endl
             << "    " << "type mistmatch between expected expression when declaring variable \'" << assign.identifier << "\'" << endl;
        exit(0);
    }

    if (t == TYPE_BOOL || t == TYPE_INT)
    {
        file << "   movq %rax, -" << index * 8 + 8 << "(%rbp)" << endl;
    }
    else if (t == TYPE_REAL)
    {
        file << "   movsd %xmm0, -" << index * 8 + 8 << "(%rbp)" << endl;
    }
}

void Generator::if_then_to_asm(if_then_node if_then)
{
    // we assume that the result file the comparison is in %rax.
    comparison_to_asm(if_then.comparison);

    file << "   test %rax, %rax" << endl; // see if %rax is 0 <=> condition is false, skip to end
    file << "   jz .ENDIF" << this->current_if_index << endl;

    int endif = this->current_if_index;
    this->current_if_index++;
    for (statement_node *stmt : if_then.statements)
    {
        if (stmt->kind == STMT_BREAK)
        {
            file << "   jmp .ENDWHILE" << this->current_while_index - 1 << endl;
        }
        statement_to_asm(*stmt);
    }
    file << ".ENDIF" << endif << ":" << endl;
}

void Generator::declaration_to_asm(declaration_node decl)
{
    // we assume that the result file the expr is in %rax OR %xmm0.

    int index = this->variable_index(decl.identifier);

    identifier_type expected = expr_to_asm(decl.expr);
    if (decl.type != expected)
    {
        cout << "[leather] compilation error:" << endl
             << "    " << "type mistmatch between expected expression when declaring variable \'" << decl.identifier << "\'" << endl;
        exit(0);
    }

    if (decl.type == TYPE_BOOL || decl.type == TYPE_INT)
    {
        file << "   movq %rax, -" << index * 8 + 8 << "(%rbp)" << endl;
    }
    else if (decl.type == TYPE_REAL)
    {
        file << "   movsd %xmm0, -" << index * 8 + 8 << "(%rbp)" << endl;
    }
}

// make sure we never access any undeclared vars,
// and add any valid variables to our variables vector.
bool Generator::statement_valid(statement_node stmt)
{
    switch (stmt.kind)
    {
    case STMT_ASSIGN:
        return this->expr_valid(get<assign_node>(stmt.statement).expr) && is_defined(get<assign_node>(stmt.statement).identifier);
        break;
    case STMT_GOTO:
        return this->goto_valid(get<goto_node>(stmt.statement));
        return true;
        break;
    case STMT_IF_THEN:
        if (!this->comparison_valid(get<if_then_node>(stmt.statement).comparison))
            return false;
        for (statement_node *stmt : get<if_then_node>(stmt.statement).statements)
        {
            if (!this->statement_valid(*stmt))
                return false;
        }
        return true;
        break;
    case STMT_LABEL:
        return true;
        break;
    case STMT_PRINT:
        return this->term_valid(get<print_node>(stmt.statement).term);
        break;
    case STMT_WHILE_LOOP:
        if (!this->comparison_valid(get<while_loop_node>(stmt.statement).comparison))
            return false;
        for (statement_node *stmt : get<while_loop_node>(stmt.statement).statements)
        {
            if (!this->statement_valid(*stmt))
                return false;
        }
        return true;
        break;
    case STMT_BREAK:
        return true;
        break;
    case STMT_DECLARATION:
        return this->expr_valid(get<declaration_node>(stmt.statement).expr);
    default:
        cout << "possible error checking if stmt is valid" << endl;
        return false;
        break;
    }
}

bool Generator::comparison_valid(comparison_node comp)
{
    if (comp.kind == COMP_LESS_THAN || comp.kind == COMP_EQUAL || comp.kind == COMP_GREATER_THAN || comp.kind == COMP_NOT_EQUAL)
    {
        return this->term_valid(get<term_binary_node>(comp.binary_expr).lhs) && this->term_valid(get<term_binary_node>(comp.binary_expr).rhs);
    }
    else
    {
        cout << "possible error checking if comparison is valid" << endl;
        return false;
    }
}

// checks if an expr has undefined variables (semantic errors)
bool Generator::expr_valid(expr_node expr)
{
    if (expr.kind == UNARY_EXPR)
    {
        return this->term_valid(get<term_node>(expr.expr));
    }
    else if (expr.kind == BINARY_EXPR_PLUS || expr.kind == BINARY_EXPR_MINUS || expr.kind == BINARY_EXPR_MULT || expr.kind == BINARY_EXPR_DIV || expr.kind == BINARY_EXPR_MOD || expr.kind == BINARY_EXPR_EXP || expr.kind == BINARY_EXPR_RIGHT_SHIFT)
    {
        return this->term_valid(get<term_binary_node>(expr.expr).lhs) && this->term_valid(get<term_binary_node>(expr.expr).rhs);
    }
    else
    {
        cout << "possible error checking if expr is valid" << endl;
        return false;
    }
}

// checks if a term has undefined variables (semantic errors)
bool Generator::term_valid(term_node term)
{
    switch (term.kind)
    {
    case TERM_INPUT:
        return true;
    case TERM_INT_LITERAL:
        return true;
    case TERM_REAL_LITERAL:
        return true;
    case TERM_IDENTIFIER:
        return this->is_defined(term.value);
    default:
        cout << "possible error checking if term is valid" << endl;
        return false;
    }
}

bool Generator::goto_valid(goto_node goto_)
{
    for (auto l : this->labels)
    {
        if (l == goto_.label)
            return true;
    }
    cout << "[leather] compilation failure:" << endl
         << "   label \"" << goto_.label << "\" is not defined." << endl;
    return false;
}

// check if a variable is defined
bool Generator::is_defined(string var)
{
    for (auto v : this->variables)
    {
        if (v.identifier == var)
            return true;
    }
    return false;
}

void Generator::update_buffer_ptr()
{
    this->buffer_ptr = this->variables.size() * 8;
}

void Generator::collect_variables(vector<statement_node> stmts)
{
    for (auto stmt : stmts)
    {
        if (stmt.kind == STMT_DECLARATION)
        {
            declaration_node decl = get<declaration_node>(stmt.statement);
            if (!this->expr_valid(decl.expr))
            {
                // can't compile because file a sem error.
                exit(0);
            }
            // add to our variables vector
            if (is_defined(decl.identifier))
            {
                cout << "[leather] compilation failure:" << endl
                     << "   attempted redeclaration of variable \'" << decl.identifier << "\'" << endl;
                exit(0);
            }
            this->variables.push_back(variable{decl.identifier, decl.type});
        }
    }
}

void Generator::collect_labels()
{
    for (auto stmt : this->program.statements)
    {
        if (stmt.kind == STMT_LABEL)
        {
            label_node label = get<label_node>(stmt.statement);
            // if it's a valid ssignment, make sure we store that
            // this variable isa now defined.
            if (!in_vector(this->labels, label.label))
                this->labels.push_back(label.label);
        }
    }
}

/*void Generator::collect_while_loops()
{
    for (auto stmt : this->program.statements)
    {
        if (stmt.kind == STMT_LABEL)
        {
            label_node label = get<label_node>(stmt.statement);
            // if it's a valid ssignment, make sure we store that
            // this variable isa now defined.
            if (!in_vector(this->labels, label.label))
                this->labels.push_back(label.label);
        }
    }
}*/

void Generator::collect_if_then(if_then_node if_then)
{
    for (statement_node *stmt : if_then.statements)
    {
        if (stmt->kind == STMT_DECLARATION)
        {
            declaration_node decl = get<declaration_node>(stmt->statement);
            if (!this->expr_valid(decl.expr))
            {
                // can't compile because file a sem error.
                exit(0);
            }
            // add to our variables vector
            this->variables.push_back(variable{decl.identifier, decl.type});
        }
        else if (stmt->kind == STMT_LABEL)
        {
            label_node label = get<label_node>(stmt->statement);
            // if it's a valid ssignment, make sure we store that
            // this variable isa now defined.
            if (!in_vector(this->labels, label.label))
                this->labels.push_back(label.label);
        }
        else if (stmt->kind == STMT_IF_THEN)
        {
            this->collect_if_then(get<if_then_node>(stmt->statement)); // follow the nest
            if (!statement_valid(*stmt))
            {
                exit(0);
            }
        }
        else if (stmt->kind == STMT_WHILE_LOOP)
        {
            this->collect_while_loops(get<while_loop_node>(stmt->statement)); // follow the nest
            if (!statement_valid(*stmt))
            {
                exit(0);
            }
        }
    }
}

void Generator::collect_all_if_thens()
{
    for (auto stmt : this->program.statements)
    {
        if (stmt.kind == STMT_IF_THEN)
        {
            this->collect_if_then(get<if_then_node>(stmt.statement));
            if (!statement_valid(stmt))
            {
                exit(0);
            }
        }
    }
}

void Generator::collect_all_while_loops()
{
    for (auto stmt : this->program.statements)
    {
        if (stmt.kind == STMT_WHILE_LOOP)
        {
            this->collect_while_loops(get<while_loop_node>(stmt.statement));
            if (!statement_valid(stmt))
            {
                exit(0);
            }
        }
    }
}

void Generator::collect_while_loops(while_loop_node while_loop)
{
    for (statement_node *stmt : while_loop.statements)
    {
        if (stmt->kind == STMT_DECLARATION)
        {
            declaration_node decl = get<declaration_node>(stmt->statement);
            if (!this->expr_valid(decl.expr))
            {
                // can't compile because file a sem error.
                exit(0);
            }
            // add to our variables vector
            this->variables.push_back(variable{decl.identifier, decl.type});
        }
        else if (stmt->kind == STMT_LABEL)
        {
            label_node label = get<label_node>(stmt->statement);
            // if it's a valid ssignment, make sure we store that
            // this variable isa now defined.
            if (!in_vector(this->labels, label.label))
                this->labels.push_back(label.label);
        }
        else if (stmt->kind == STMT_IF_THEN)
        {
            this->collect_if_then(get<if_then_node>(stmt->statement)); // follow the nest
            if (!statement_valid(*stmt))
            {
                exit(0);
            }
        }
        else if (stmt->kind == STMT_WHILE_LOOP)
        {
            this->collect_while_loops(get<while_loop_node>(stmt->statement)); // follow the nest
            if (!statement_valid(*stmt))
            {
                exit(0);
            }
        }
    }
}

template <typename T>
bool in_vector(vector<T> vec, T value)
{
    for (auto v : vec)
    {
        if (v == value)
            return true;
    }
    return false;
}

int Generator::variable_index(string var)
{
    for (unsigned int i = 0; i < this->variables.size(); i++)
    {
        if (this->variables[i].identifier == var)
            return i;
    }
    // not found
    return -1;
}

identifier_type Generator::expr_to_asm(expr_node expr)
{
    // WE WANT TO STORE INT RESULTS IN %rax AND REAL RESULTS IN %xmm0.
    if (expr.kind == UNARY_EXPR)
    {
        identifier_type expected = term_to_asm(get<term_node>(expr.expr));
        return expected;
        // result is already in %rax or %xmm0.
    }
    else if (expr.kind == BINARY_EXPR_PLUS)
    {
        term_binary_node binary_expr = get<term_binary_node>(expr.expr);
        identifier_type expected = this->expected_binary_expr_result(binary_expr);

        if (expected == TYPE_INT)
        {
            term_to_asm(binary_expr.lhs);
            // assume lhs result in %rax. let's store in %rcx for now.
            file << "   mov %rax, %rcx" << endl;
            term_to_asm(binary_expr.rhs);
            // now we have to add them
            file << "   addq %rcx, %rax" << endl;
            // notice result is in %rax.
        }
        else if (expected == TYPE_REAL)
        {
            term_to_asm(binary_expr.lhs, expected = TYPE_REAL);
            // assume lhs result in %xmm0. let's store in %xmm1 for now.
            file << "   movsd %xmm0, %xmm1" << endl;
            term_to_asm(binary_expr.rhs, expected = TYPE_REAL);
            // now we have to add them
            file << "   addsd %xmm1, %xmm0" << endl;
            // notice result is in %xmm0.
        }

        return expected;
    }
    else if (expr.kind == BINARY_EXPR_MINUS)
    {
        term_binary_node binary_expr = get<term_binary_node>(expr.expr);
        identifier_type expected = this->expected_binary_expr_result(binary_expr);

        if (expected == TYPE_INT)
        {
            term_to_asm(binary_expr.rhs);
            // assume rhs result in %rax. let's store in %rcx for now.
            file << "   mov %rax, %rcx" << endl;
            term_to_asm(binary_expr.lhs);
            // now we have to subtract them
            file << "   subq %rcx, %rax" << endl;
            // notice result is in %rax.
        }
        else if (expected == TYPE_REAL)
        {
            term_to_asm(binary_expr.rhs, expected = TYPE_REAL);
            file << "   movsd %xmm0, %xmm1" << endl;
            term_to_asm(binary_expr.lhs, expected = TYPE_REAL);
            file << "   subsd %xmm1, %xmm0" << endl;
        }
        return expected;
    }
    else if (expr.kind == BINARY_EXPR_MULT)
    {
        term_binary_node binary_expr = get<term_binary_node>(expr.expr);
        identifier_type expected = this->expected_binary_expr_result(binary_expr);

        if (expected == TYPE_INT)
        {
            term_to_asm(binary_expr.lhs);
            file << "   mov %rax, %rcx" << endl;
            term_to_asm(binary_expr.rhs);
            file << "   mul %rcx" << endl;
        }
        else if (expected == TYPE_REAL)
        {
            term_to_asm(binary_expr.rhs, expected = TYPE_REAL);
            file << "   movsd %xmm0, %xmm1" << endl;
            term_to_asm(binary_expr.lhs, expected = TYPE_REAL);
            file << "   mulsd %xmm1, %xmm0" << endl;
        }
        return expected;
    }
    else if (expr.kind == BINARY_EXPR_DIV)
    {
        term_binary_node binary_expr = get<term_binary_node>(expr.expr);
        identifier_type expected = this->expected_binary_expr_result(binary_expr);

        if (expected == TYPE_INT)
        {
            term_to_asm(binary_expr.rhs);
            // assume rhs result (divisor) in %rax. let's store in %rcx for now.
            file << "   mov %rax, %rcx" << endl;
            term_to_asm(binary_expr.lhs);
            // now we have to divide them
            file << "   xor %rdx, %rdx" << endl; // clear for div instr
            file << "   div %rcx" << endl;
        }
        else if (expected == TYPE_REAL)
        {
            term_to_asm(binary_expr.rhs, expected = TYPE_REAL);
            file << "   movsd %xmm0, %xmm1" << endl;
            term_to_asm(binary_expr.lhs, expected = TYPE_REAL);
            file << "   divsd %xmm1, %xmm0" << endl;
        }
        return expected;
    }
    else if (expr.kind == BINARY_EXPR_MOD)
    {
        term_binary_node binary_expr = get<term_binary_node>(expr.expr);
        term_to_asm(binary_expr.rhs);
        // assume rhs result (divisor) in %rax. let's store in %rcx for now.
        file << "   mov %rax, %rcx" << endl;
        term_to_asm(binary_expr.lhs);
        // now we have to divide them
        file << "   xor %rdx, %rdx" << endl; // clear for div instr
        file << "   div %rcx" << endl;
        // the remainder is in %rdx, move to %rax.
        file << "   mov %rdx, %rax" << endl;
        return TYPE_INT;
    }
    else if (expr.kind == BINARY_EXPR_EXP)
    {
        term_binary_node binary_expr = get<term_binary_node>(expr.expr);
        term_to_asm(binary_expr.lhs);
        // lhs is base, store in %rcx for now
        file << "   mov %rax, %rcx" << endl;
        term_to_asm(binary_expr.rhs);
        // rhs is exponent, store in %rbx
        file << "   mov %rax, %rbx" << endl;
        // perform exponentiation with result in %rax
        file << "   movq $1, %rax" << endl;
        file << "   call exp" << endl;
        return TYPE_INT;
    }
    else if (expr.kind == BINARY_EXPR_RIGHT_SHIFT)
    {
        term_binary_node binary_expr = get<term_binary_node>(expr.expr);
        term_to_asm(binary_expr.rhs);
        file << "   mov %rax, %rcx" << endl;
        term_to_asm(binary_expr.lhs);
        file << "   shr %rcx, %rax" << endl;
        return TYPE_INT;
    }
    return TYPE_INVALID;
}

void Generator::input_to_asm(term_node term)
{
    // we need to allocate space for the input
    file << "   mov %rbp, %rsi" << endl;
    file << "   addq $" << this->buffer_ptr << ", %rsi" << endl;
    file << "   movq $0, %rax" << endl;
    file << "   movq $0, %rdi" << endl;
    file << "   movq $64, %rdx" << endl;
    file << "   syscall" << endl;

    // now, the number file bytes read in should be in %rax.
    // + setup for call to ascii_to_int
    file << "   mov %rax, %rcx" << endl;
    file << "   subq $1, %rcx" << endl;
    file << "   mov %rsi, %r9" << endl;
    file << "   addq %rcx, %r9" << endl;
    file << "   subq $1, %r9" << endl;

    file << "   movq $1, %rbx" << endl;
    file << "   xor %r8, %r8" << endl;
    file << "   call ascii_to_int" << endl;
    // input value is now in %rax.
}

identifier_type Generator::term_to_asm(term_node term, identifier_type expected)
{
    // we need to store the result in %rax or %xmm0 if decimal.
    if (term.kind == TERM_IDENTIFIER)
    {
        unsigned int index = this->variable_index(term.value);
        if (this->get_type(term.value) == TYPE_REAL)
        {
            // real! pi = 3.14
            file << "   movsd -" << index * 8 + 8 << "(%rbp)" << ", %xmm0" << endl;
            return TYPE_REAL;
        }
        else if (this->get_type(term.value) == TYPE_INT)
        {
            // get its value, load from its index!
            file << "   movq -" << index * 8 + 8 << "(%rbp)" << ", %rax" << endl;
            // if we are expecting a real number, we need to convert this and store it in %xmm0.
            if (expected == TYPE_REAL)
                file << "   cvtsi2sd %rax, %xmm0" << endl;
            return TYPE_INT;
        }
    }
    else if (term.kind == TERM_INT_LITERAL)
    {
        file << "   movq $" << term.value << ", %rax" << endl;
        if (expected == TYPE_REAL)
            file << "   cvtsi2sd %rax, %xmm0" << endl;
        return TYPE_INT;
    }
    else if (term.kind == TERM_REAL_LITERAL)
    {
        file << "   movabs $" << convertToHex(term.value) << ", %rax" << endl;
        file << "   movq %rax, %xmm0" << endl;
        return TYPE_REAL;
    }
    else if (term.kind == TERM_INPUT)
    {
        input_to_asm(term);
        return TYPE_INT;
    }
    else
    {
        return TYPE_INT;
    }
    return TYPE_INVALID;
}

void Generator::comparison_to_asm(comparison_node comp)
{
    // we need to store in %rax.
    if (comp.kind == COMP_LESS_THAN || comp.kind == COMP_GREATER_THAN)
    {
        term_binary_node binary_expr = get<term_binary_node>(comp.binary_expr);
        term_node lhs = binary_expr.lhs;
        term_to_asm(lhs);
        // lhs is in %rax -> store temp in %rcx.
        file << "   mov %rax, %rcx" << endl;
        term_node rhs = binary_expr.rhs;
        term_to_asm(rhs);
        // rhs is in %rax, so lets compare now.
        file << "   cmpq %rax, %rcx" << endl;
        // if res is less than 0, we know lhs < rhs,
        // so put a 1 in %rax or else %rax should be 0.
        if (comp.kind == COMP_LESS_THAN)
            file << "   setl %al" << endl;
        else if (comp.kind == COMP_GREATER_THAN)
            file << "   setg %al" << endl;
        // put it in %rax with zero extend
        file << "   movzbq %al, %rax" << endl;
    }
    else if (comp.kind == COMP_EQUAL || comp.kind == COMP_NOT_EQUAL)
    {
        term_binary_node binary_expr = get<term_binary_node>(comp.binary_expr);
        term_node lhs = binary_expr.lhs;
        term_to_asm(lhs);
        file << "   mov %rax, %rcx" << endl;
        term_node rhs = binary_expr.rhs;
        term_to_asm(rhs);
        file << "   cmpq %rcx, %rax" << endl;
        if (comp.kind == COMP_EQUAL)
            file << "   sete %al" << endl;
        else if (comp.kind == COMP_NOT_EQUAL)
            file << "   setne %al" << endl;
        file << "   movzbq %al, %rax" << endl;
    }
    // now, there is either a 0 or 1 is %rax. lets just flip the last bit if is_not is true.
    if (comp.is_not)
    {
        file << "   xor $1, %rax" << endl;
    }
}

void Generator::print_to_asm(print_node print)
{
    // get term that must be printed
    identifier_type expected = term_to_asm(print.term);

    // assume result is in %rax.
    // however, it is an actual integer but we
    // need to print ASCII codes.

    // now, we have to allocate memory on the stack
    // to store our buffer

    // preparation for call to int_to_ascii
    file << "   movq $1, %rsi" << endl; // we have to print at least a \n
    file << "   mov %rbp, %rcx" << endl;
    file << "   addq $" << this->buffer_ptr + 63 << ", %rcx" << endl;
    file << "   movq $0x0A, (%rcx)" << endl; // newline character at end file str
    file << "   dec %rcx" << endl;

    if (expected == TYPE_INT)
    {
        file << "   call int_to_ascii" << endl;
    }
    else if (expected == TYPE_REAL)
    {
        file << "   call double_to_ascii" << endl;
    }

    file << "   movq $1, %rax" << endl;
    file << "   movq $1, %rdi" << endl;
    file << "   syscall" << endl;
}

void Generator::while_loop_to_asm(while_loop_node while_loop)
{
    file << ".STARTWHILE" << this->current_while_index << ":" << endl;
    // check condition, if true: execute statments, jump back to beginning of loop (check condition again.)
    // if false: skip to after while loop
    // we assume that the result file the comparison is in %rax.
    comparison_to_asm(while_loop.comparison);

    file << "   test %rax, %rax" << endl; // see if %rax is 0 <=> condition is false, skip to end
    file << "   jz .ENDWHILE" << this->current_while_index << endl;

    int endwhile = this->current_while_index;
    this->current_while_index++;
    for (statement_node *stmt : while_loop.statements)
    {
        if (stmt->kind == STMT_BREAK)
        {
            file << "   jmp .ENDWHILE" << endwhile << endl;
        }
        statement_to_asm(*stmt);
    }
    // if we made it to this point, we loop again!
    file << "   jmp .STARTWHILE" << endwhile << endl;
    file << ".ENDWHILE" << endwhile << ":" << endl;
}

identifier_type Generator::get_type(string var)
{
    for (auto v : this->variables)
    {
        if (v.identifier == var)
        {
            return v.type;
        }
    }
    return TYPE_INVALID;
}

// returns the expected type of the result of a binary expression.
identifier_type Generator::expected_binary_expr_result(term_binary_node binary_expr)
{
    // check for identifiers
    if (binary_expr.lhs.kind == TERM_IDENTIFIER)
    {
        if (this->get_type(binary_expr.lhs.value) == TYPE_REAL)
            return TYPE_REAL;
    }
    if (binary_expr.rhs.kind == TERM_IDENTIFIER)
    {
        if (this->get_type(binary_expr.rhs.value) == TYPE_REAL)
            return TYPE_REAL;
    }
    return (binary_expr.lhs.kind == TERM_REAL_LITERAL || binary_expr.rhs.kind == TERM_REAL_LITERAL) ? TYPE_REAL : TYPE_INT;
}