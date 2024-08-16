#include "generator.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "util.h"

using namespace std;

// constructor
Generator::Generator(program_node program, string &output_filename, vector<string_> strings_) : program(program), current_if_index(0), current_loop_index(0), file(output_filename), strings_(strings_)
{
    if (!file)
    {
        cout << "Couldn't open output file" << endl;
        exit(0);
    }
};

// generates x86 assembly instructions based on program_node/AST.
void Generator::to_asm(bool logging)
{
    this->collect_variables(this->program.statements);
    this->collect_labels();
    this->collect_all_while_loops();
    this->collect_all_if_thens();
    this->collect_all_for_loops();

    if (!file.is_open())
    {
        cout << "file is closed bruh" << endl;
    }

    file << ".section .data" << endl;
    for (auto str : this->strings_)
    {
        file << "str" << this->string_index(str.value) << ":" << endl;
        file << "   .string \"" << str.value << "\"" << endl;
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
    file << "   sub $128, %rsp" << endl;
    for (statement_node stmt : this->program.statements)
    {
        if (logging)
        {
            cout << "[leather-logging] successfully generated ";
            switch (stmt.kind)
            {
            case STMT_ASSIGN:
                cout << "ASSIGN statement" << endl;
                break;
            case STMT_ARRAY_ASSIGN:
                cout << "ARRAY_ASSIGN statement" << endl;
                break;
            case STMT_GOTO:
                cout << "GOTO statement" << endl;
                break;
            case STMT_IF_THEN:
                cout << "IF_THEN statement" << endl;
                break;
            case STMT_LABEL:
                cout << "LABEL statement" << endl;
                break;
            case STMT_PRINT:
                cout << "PRINT statement" << endl;
                break;
            case STMT_WHILE_LOOP:
                cout << "WHILE_LOOP statement" << endl;
                break;
            case STMT_FOR_LOOP:
                cout << "FOR_LOOP statement" << endl;
                break;
            case STMT_DECLARATION:
                cout << "DECLARATION statement" << endl;
                break;
            case STMT_ARRAY_DECLARATION:
                cout << "ARRAY_DECLARATION statement" << endl;
                break;
            default:
                break;
            }
            cout << endl;
        }
        this->statement_to_asm(stmt);
    }
    file << ".exit:" << endl;
    file << "   movq $60, %rax" << endl;
    file << "   xor %rdi, %rdi" << endl;
    file << "   syscall" << endl;

    // predefined functions
    file << endl;
    file << "int_to_ascii:" << endl;
    file << "   xor %r10, %r10" << endl;
    file << "   test %rax, %rax" << endl;
    file << "   jns .convert_positive" << endl;
    file << "   movq $1, %r10" << endl;
    file << "   neg %rax" << endl;
    file << ".convert_positive:" << endl;
    file << "   movq $10, %rbx" << endl;
    file << "   xor %rdx, %rdx" << endl;
    file << "   div %rbx" << endl;
    file << "   add $48, %dl" << endl;
    file << "   movb %dl, (%rcx)" << endl;
    file << "   inc %rsi" << endl;
    file << "   dec %rcx" << endl;
    file << "   test %rax, %rax" << endl;
    file << "   jnz .convert_positive" << endl;
    file << "   test %r10, %r10" << endl;
    file << "   jz .end_itoa" << endl;
    file << "   movb $'-', (%rcx)" << endl;
    file << "   dec %rcx" << endl;
    file << "   inc %rsi" << endl;
    file << ".end_itoa:" << endl;
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
    file << "   xor %r12, %r12" << endl;
    file << "   cvttsd2si %xmm0, %r9" << endl;
    file << "   test %r9, %r9" << endl;
    file << "   jns .convert_positive_dta" << endl;
    file << "   movq $1, %r12" << endl;
    file << "   movabs $0x8000000000000000, %rax" << endl;
    file << "   movq %rax, %xmm2" << endl;
    file << "   xorpd %xmm2, %xmm0" << endl;
    file << "   cvttsd2si %xmm0, %r9" << endl;
    file << ".convert_positive_dta:" << endl;
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
    file << "   test %r12, %r12" << endl;
    file << "   jz .end_dtoa" << endl;
    file << "   dec %rsi" << endl;
    file << "   movb $'-', (%rsi)" << endl;
    file << "   inc %rdx" << endl;
    file << ".end_dtoa:" << endl;
    file << "   ret" << endl;

    file << endl;
    file << "heapalloc:" << endl;
    file << "   mov %rdi, %rsi" << endl;
    file << "   movq $0, %rdi" << endl;
    file << "   movq $0x3, %rdx" << endl;
    file << "   movq $0x22, %r10" << endl;
    file << "   movq $-1, %r8" << endl;
    file << "   xor %r9, %r9" << endl;
    file << "   movq $9, %rax" << endl;
    file << "   syscall" << endl;
    file << "   ret" << endl;

    file << endl;
    file << "strcpy:" << endl;
    file << "   test %rdx, %rdx" << endl;
    file << "   jz .end_strcpy" << endl;
    file << "   movb (%rdi), %al" << endl;
    file << "   movb %al, (%rsi)" << endl;
    file << "   dec %rdx" << endl;
    file << "   inc %rdi" << endl;
    file << "   inc %rsi" << endl;
    file << "   jmp strcpy" << endl;
    file << ".end_strcpy:" << endl;
    file << "   movb $0, (%rsi)" << endl;
    file << "   ret" << endl;

    file << endl;
    file << "strlen:" << endl;
    file << "   xor %r9, %r9" << endl;
    file << ".strlen_loop:" << endl;
    file << "   movb (%rdi), %al" << endl;
    file << "   test %al, %al" << endl;
    file << "   jz .end_strlen" << endl;
    file << "   inc %r9" << endl;
    file << "   inc %rdi" << endl;
    file << "   jmp .strlen_loop" << endl;
    file << ".end_strlen:" << endl;
    file << "   mov %r9, %rax" << endl;
    file << "   ret" << endl;

    file << endl;
    file << "get_rand:" << endl;
    file << "   movq $318, %rax" << endl;
    file << "   movq $8, %rsi" << endl;
    file << "   movq $0, %rdx" << endl;
    file << "   syscall" << endl;
    file << "   movq (%rdi), %rax" << endl;
    file << "   ret" << endl;

    file << endl;
    file << "print_str:" << endl;
    file << "   mov %rdi, %rsi" << endl;
    file << "   call strlen" << endl;
    file << "   mov %rax, %rdx" << endl;
    file << "   mov %rsi, %r8" << endl;
    file << "   addq %rdx, %r8" << endl;
    file << "   movb $0x0A, (%r8)" << endl;
    file << "   inc %rdx" << endl;
    file << "   movq $1, %rax" << endl;
    file << "   movq $1, %rdi" << endl;
    file << "   syscall" << endl;
    file << "   movb $0, (%r8)" << endl;
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
    case STMT_ARRAY_ASSIGN:
        array_assign_to_asm(get<array_assign_node>(stmt.statement));
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
    case STMT_FOR_LOOP:
        for_loop_to_asm(get<for_loop_node>(stmt.statement));
        break;
    case STMT_DECLARATION:
        declaration_to_asm(get<declaration_node>(stmt.statement));
        break;
    case STMT_ARRAY_DECLARATION:
        array_declaration_to_asm(get<array_declare_node>(stmt.statement));
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
             << "    variable \"" << assign.identifier << "\" is not declared." << endl;
        exit(0);
    }

    int index = this->variable_index(assign.identifier);

    identifier_type t = this->variables[index].type;

    identifier_type expected = expr_to_asm(assign.expr);
    if (t != expected)
    {
        cout << "[leather] compilation error:" << endl
             << "    " << "type mistmatch between expected expression when assigning variable \'" << assign.identifier << "\'" << endl;
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
    else if (t == TYPE_STR)
    {
        cout << "[leather] compilation error:" << endl
             << "    " << "strings are constant, when assigning \'" << assign.identifier << "\'" << endl;
        exit(0);
    }
}

void Generator::array_assign_to_asm(array_assign_node assign)
{
    if (!this->expr_valid(assign.expr))
    {
        // sem error
        exit(0);
    }

    string identifier = assign.identifier;
    unsigned int index = this->variable_index(identifier);

    identifier_type array_type = this->get_type(identifier);
    identifier_type expected = expr_to_asm(assign.expr);

    // WE NEED TO SAVE THE VALUE IN %rax! we are storing it in memory...
    file << "   push %rax" << endl;

    if (!((array_type == TYPE_ARRAY_INT && expected == TYPE_INT) || (array_type == TYPE_ARRAY_REAL && expected == TYPE_REAL)))
    {
        cout << "[leather] compilation error:" << endl
             << "    " << "type mistmatch between expected expression when assign array variable \'" << identifier << "\' at index ";
        print_expr(assign.index_expr);
        cout << endl;
        exit(0);
    }

    // get ptr of array into %rdx. %rax or %xmm0 holds our value we want to store.
    file << "   movq -" << index * 8 + 8 << "(%rbp), %rdx" << endl;

    // move index into %rbx.
    expr_to_asm(assign.index_expr);
    file << "   mov %rax, %rbx" << endl;
    // WE NEED TO RESTORE THE VALUE IN %rax! we are storing it in memory...
    file << "   pop %rax" << endl;

    if (array_type == TYPE_ARRAY_INT)
    {
        file << "   movq %rax, (%rdx, %rbx, 8)" << endl;
    }
    else if (array_type == TYPE_ARRAY_REAL)
    {
        file << "   movsd %xmm0, (%rdx, %rbx, 8)" << endl;
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
            file << "   jmp .ENDLOOP" << this->current_loop_index - 1 << endl;
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
             << "    " << "type mismatch between expected expression when declaring variable \'" << decl.identifier << "\'" << endl;
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
    else if (decl.type == TYPE_STR)
    {
        if (decl.expr.kind != UNARY_EXPR)
        {
            cout << "[leather] compilation error:" << endl
                 << "    attemped binary expression with string" << endl;
            exit(0);
        }
        int strlen = get<term_node>(decl.expr.expr).value.size();
        file << "   movq $" << strlen + 1 << ", %rdi" << endl; // + 1 for \0
        // allocate and store ptr of array!
        file << "   xor %r12, %r12" << endl;
        file << "   call heapalloc" << endl;
        // save ptr at address
        file << "   movq %rax, -" << index * 8 + 8 << "(%rbp)" << endl;
        // TODO!
        // setup for strcpy
        int string_index = this->string_index(get<term_node>(decl.expr.expr).value);
        if (string_index == -1)
        {
            cout << "[leather] compilation error:" << endl
                 << "    " << "unknown string \"" << get<term_node>(decl.expr.expr).value << "\"" << endl;
            exit(0);
        }
        file << "   lea str" << string_index << "(%rip), %rdi" << endl;
        file << "   mov %rax, %rsi" << endl;
        file << "   movq $" << strlen << ", %rdx" << endl;
        file << "   call strcpy" << endl;
    }
}

void Generator::array_declaration_to_asm(array_declare_node decl)
{
    int index = this->variable_index(decl.identifier);
    // get len expr in %rax.
    expr_to_asm(decl.len_expr);

    // now put it in %rdi
    file << "   mov %rax, %rdi" << endl;

    // allocate and store ptr of array!
    file << "   xor %r12, %r12" << endl;
    // do len * 8 to find how many bytes we need to allocate
    file << "   lea (%r12, %rdi, 8), %rdi" << endl;
    file << "   call heapalloc" << endl;
    file << "   movq %rax, -" << index * 8 + 8 << "(%rbp)" << endl;
};

// make sure we never access any undeclared vars,
// and add any valid variables to our variables vector.
bool Generator::statement_valid(statement_node stmt)
{
    switch (stmt.kind)
    {
    case STMT_ASSIGN:
        return this->expr_valid(get<assign_node>(stmt.statement).expr) && is_defined(get<assign_node>(stmt.statement).identifier);
        break;
    case STMT_ARRAY_ASSIGN:
    {
        if (!this->expr_valid(get<array_assign_node>(stmt.statement).expr))
            return false;

        string identifier = get<array_assign_node>(stmt.statement).identifier;
        if (!is_defined(identifier))
            return false;

        return true;

        break;
    }
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
    case STMT_FOR_LOOP:
        if (!this->expr_valid(get<for_loop_node>(stmt.statement).declaration.expr))
            return false;
        if (!this->comparison_valid(get<for_loop_node>(stmt.statement).comparison))
            return false;
        for (statement_node *stmt : get<for_loop_node>(stmt.statement).statements)
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
    case TERM_RANDOM:
        return true;
    case TERM_INT_LITERAL:
        return true;
    case TERM_REAL_LITERAL:
        return true;
    case TERM_STR_LITERAL:
        return true;
    case TERM_IDENTIFIER:
        return this->is_defined(term.value);
    case TERM_ARRAY_REAL_LITERAL:
        return true;
    case TERM_ARRAY_INT_LITERAL:
        return true;
    case TERM_ARRAY_ACCESS:
    {
        return this->is_defined(term.value) && expr_valid(*term.index_expr);
    }
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
        else if (stmt.kind == STMT_ARRAY_DECLARATION)
        {
            array_declare_node decl = get<array_declare_node>(stmt.statement);
            if (!this->expr_valid(decl.len_expr))
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
        else if (stmt->kind == STMT_ARRAY_DECLARATION)
        {
            array_declare_node decl = get<array_declare_node>(stmt->statement);
            if (!this->expr_valid(decl.len_expr))
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
        else if (stmt->kind == STMT_FOR_LOOP)
        {
            this->collect_for_loops(get<for_loop_node>(stmt->statement)); // follow the nest
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

void Generator::collect_all_for_loops()
{
    for (auto stmt : this->program.statements)
    {
        if (stmt.kind == STMT_FOR_LOOP)
        {
            for_loop_node for_loop = get<for_loop_node>(stmt.statement);
            this->collect_for_loops(for_loop);
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
        else if (stmt->kind == STMT_ARRAY_DECLARATION)
        {
            array_declare_node decl = get<array_declare_node>(stmt->statement);
            if (!this->expr_valid(decl.len_expr))
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
        else if (stmt->kind == STMT_FOR_LOOP)
        {
            this->collect_for_loops(get<for_loop_node>(stmt->statement)); // follow the nest
            if (!statement_valid(*stmt))
            {
                exit(0);
            }
        }
    }
}

void Generator::collect_for_loops(for_loop_node for_loop)
{
    declaration_node decl = for_loop.declaration;
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
    // add for loop var to variables vector
    this->variables.push_back(variable{decl.identifier, decl.type});
    for (statement_node *stmt : for_loop.statements)
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
        else if (stmt->kind == STMT_ARRAY_DECLARATION)
        {
            array_declare_node decl = get<array_declare_node>(stmt->statement);
            if (!this->expr_valid(decl.len_expr))
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
        else if (stmt->kind == STMT_FOR_LOOP)
        {
            this->collect_for_loops(get<for_loop_node>(stmt->statement)); // follow the nest
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

int Generator::string_index(string str)
{
    for (unsigned int i = 0; i < this->strings_.size(); i++)
    {
        if (this->strings_[i].value == str)
            return i;
    }
    // not found
    return -1;
}

identifier_type Generator::expr_to_asm(expr_node expr, string temp_register)
{
    // WE WANT TO STORE INT RESULTS IN %rax AND REAL RESULTS IN %xmm0.
    if (expr.kind == UNARY_EXPR)
    {
        identifier_type expected = term_to_asm(get<term_node>(expr.expr));
        return expected;
        // result is already in %rax or %xmm0.
    }
    // WE HAVE TO SAVE %tmp FOR NESTED EXPRESSIONS! this is where we store the other value. (for binary expressions only)
    // TODO: This will be useful later on when nested expressions can happen outside of array accessing!
    file << "   push " << temp_register << endl;

    if (expr.kind == BINARY_EXPR_PLUS)
    {
        term_binary_node binary_expr = get<term_binary_node>(expr.expr);
        identifier_type expected = this->expected_binary_expr_result(binary_expr);

        if (expected == TYPE_INT)
        {
            term_to_asm(binary_expr.lhs);
            // assume lhs result in %rax. let's store in %rcx for now.
            file << "   mov %rax, " << temp_register << endl;
            term_to_asm(binary_expr.rhs);
            // now we have to add them
            file << "   addq " << temp_register << ", %rax" << endl;
            // notice result is in %rax.
            file << "   pop " << temp_register << endl; // restore temp register
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
            file << "   mov %rax, " << temp_register << endl;
            term_to_asm(binary_expr.lhs);
            // now we have to subtract them
            file << "   subq " << temp_register << ", %rax" << endl;
            // notice result is in %rax.
            file << "   pop " << temp_register << endl; // restore temp register
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
            file << "   mov %rax, " << temp_register << endl;
            term_to_asm(binary_expr.rhs);
            file << "   mul %" << temp_register << endl;
            file << "   pop " << temp_register << endl; // restore temp register
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
            file << "   mov %rax, " << temp_register << endl;
            term_to_asm(binary_expr.lhs);
            // now we have to divide them
            file << "   xor %rdx, %rdx" << endl; // clear for div instr
            file << "   div " << temp_register << endl;
            file << "   pop " << temp_register << endl; // restore temp register
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
        file << "   mov %rax, " << temp_register << endl;
        term_to_asm(binary_expr.lhs);
        // now we have to divide them
        file << "   xor %rdx, %rdx" << endl; // clear for div instr
        file << "   div " << temp_register << endl;
        // the remainder is in %rdx, move to %rax.
        file << "   mov %rdx, %rax" << endl;
        file << "   pop " << temp_register << endl; // restore temp register
        return TYPE_INT;
    }
    else if (expr.kind == BINARY_EXPR_EXP)
    {
        term_binary_node binary_expr = get<term_binary_node>(expr.expr);
        term_to_asm(binary_expr.lhs);

        file << "   mov %rax, %rcx" << endl;
        term_to_asm(binary_expr.rhs);
        // rhs is exponent, store in %rbx
        file << "   mov %rax, %rbx" << endl;
        // perform exponentiation with result in %rax
        file << "   movq $1, %rax" << endl;
        file << "   call exp" << endl;
        file << "   pop " << temp_register << endl; // restore temp register
        return TYPE_INT;
    }
    else if (expr.kind == BINARY_EXPR_RIGHT_SHIFT)
    {
        term_binary_node binary_expr = get<term_binary_node>(expr.expr);
        term_to_asm(binary_expr.rhs);
        file << "   mov %rax, %rcx" << endl;
        term_to_asm(binary_expr.lhs);
        file << "   shr %rcx, %rax" << endl;
        file << "   pop " << temp_register << endl; // restore temp register
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

void Generator::random_to_asm()
{
    // get ptr to free memory in %rdi
    file << "   mov %rbp, %rdi" << endl;
    file << "   addq $" << this->buffer_ptr << ", %rdi" << endl;
    file << "   push %rcx" << endl;
    file << "   call get_rand" << endl;
    file << "   pop %rcx" << endl;
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
        else if (this->get_type(term.value) == TYPE_STR)
        {
            // load ptr to str into %rax
            file << "   movq -" << index * 8 + 8 << "(%rbp)" << ", %rax" << endl;
            return TYPE_STR;
        }
        else if (this->get_type(term.value) == TYPE_ARRAY_INT)
        {
            file << "   movq -" << index * 8 + 8 << "(%rbp)" << ", %rax" << endl;
            return TYPE_ARRAY_INT;
        }
        else if (this->get_type(term.value) == TYPE_ARRAY_REAL)
        {
            file << "   movq -" << index * 8 + 8 << "(%rbp)" << ", %rax" << endl;
            return TYPE_ARRAY_REAL;
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
    else if (term.kind == TERM_STR_LITERAL)
    {
        int string_index = this->string_index(term.value);
        if (string_index == -1)
        {
            cout << "[leather] compilation error:" << endl
                 << "    " << "unknown string \"" << term.value << "\"" << endl;
            exit(0);
        }
        file << "   lea str" << string_index << "(%rip), %rax" << endl;
        return TYPE_STR;
    }
    else if (term.kind == TERM_INPUT)
    {
        input_to_asm(term);
        return TYPE_INT;
    }
    else if (term.kind == TERM_RANDOM)
    {
        random_to_asm();
        return TYPE_INT;
    }
    else if (term.kind == TERM_ARRAY_INT_LITERAL)
    {
        // TODO!
        return TYPE_ARRAY_INT;
    }
    else if (term.kind == TERM_ARRAY_REAL_LITERAL)
    {
        // TODO!
        return TYPE_ARRAY_REAL;
    }
    else if (term.kind == TERM_ARRAY_ACCESS)
    {
        string identifier = term.value;
        unsigned int index = this->variable_index(identifier);

        // store ptr to arr in %rdx.
        file << "   movq -" << index * 8 + 8 << "(%rbp)" << ", %rdx" << endl;

        // put index expr into %rbx.
        expr_to_asm(*term.index_expr);
        file << "   mov %rax, %rbx" << endl;

        if (this->get_type(identifier) == TYPE_ARRAY_INT)
        {
            // index into array, move into rax.
            file << "   movq (%rdx, %rbx, 8), %rax" << endl;
            if (expected == TYPE_REAL) // convert to real and store in xmm0 if expected is a real.
                file << "   cvtsi2sd %rax, %xmm0" << endl;
            return TYPE_INT;
        }
        else if (this->get_type(identifier) == TYPE_ARRAY_REAL)
        {
            // index into array, move into xmm0.
            file << "   movsd (%rdx, %rbx, 8), %xmm0" << endl;
            return TYPE_REAL;
        }
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
    identifier_type expected = expected_binary_expr_result(get<term_binary_node>(comp.binary_expr));
    if (expected == TYPE_INT)
    {
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
    }
    else if (expected == TYPE_REAL)
    {
        if (comp.kind == COMP_LESS_THAN || comp.kind == COMP_GREATER_THAN)
        {
            term_binary_node binary_expr = get<term_binary_node>(comp.binary_expr);
            term_node lhs = binary_expr.lhs;
            term_to_asm(lhs, expected = TYPE_REAL);
            // lhs is in %xmm0 -> store temp in %xmm1.
            file << "   movsd %xmm0, %xmm1" << endl;
            term_node rhs = binary_expr.rhs;
            term_to_asm(rhs, expected = TYPE_REAL);
            // rhs is in %xmm1, so lets compare now.
            file << "   ucomisd %xmm0, %xmm1" << endl;
            // if res is less than 0, we know lhs < rhs,
            // so put a 1 in %rax or else %rax should be 0.
            if (comp.kind == COMP_LESS_THAN)
                file << "   setb %al" << endl;
            else if (comp.kind == COMP_GREATER_THAN)
                file << "   seta %al" << endl;
            // put it in %rax with zero extend
            file << "   movzbq %al, %rax" << endl;
        }
        else if (comp.kind == COMP_EQUAL || comp.kind == COMP_NOT_EQUAL)
        {
            term_binary_node binary_expr = get<term_binary_node>(comp.binary_expr);
            term_node lhs = binary_expr.lhs;
            term_to_asm(lhs, expected = TYPE_REAL);
            file << "   movsd %xmm0, %xmm1" << endl;
            term_node rhs = binary_expr.rhs;
            term_to_asm(rhs, expected = TYPE_REAL);
            file << "   ucomisd %xmm0, %xmm1" << endl;
            if (comp.kind == COMP_EQUAL)
                file << "   sete %al" << endl;
            else if (comp.kind == COMP_NOT_EQUAL)
                file << "   setne %al" << endl;
            file << "   movzbq %al, %rax" << endl;
        }
    }
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

    if (expected == TYPE_INT || expected == TYPE_ARRAY_INT || expected == TYPE_ARRAY_REAL || expected == TYPE_REAL)
    {
        // preparation for call to int_to_ascii
        file << "   movq $1, %rsi" << endl; // we have to print at least a \n
        file << "   mov %rbp, %rcx" << endl;
        file << "   addq $" << this->buffer_ptr + 63 << ", %rcx" << endl;
        file << "   movq $0x0A, (%rcx)" << endl; // newline character at end file str
        file << "   dec %rcx" << endl;

        if (expected == TYPE_REAL)
        {
            file << "   call double_to_ascii" << endl;
        }
        else
        {
            file << "   call int_to_ascii" << endl;
        }
        file << "   movq $1, %rax" << endl;
        file << "   movq $1, %rdi" << endl;
        file << "   syscall" << endl;
    }
    else if (expected == TYPE_STR)
    {
        file << "   mov %rax, %rdi" << endl;
        file << "   call print_str" << endl;
    }
}

void Generator::while_loop_to_asm(while_loop_node while_loop)
{
    file << ".STARTLOOP" << this->current_loop_index << ":" << endl;
    // check condition, if true: execute statments, jump back to beginning of loop (check condition again.)
    // if false: skip to after while loop
    // we assume that the result file the comparison is in %rax.
    comparison_to_asm(while_loop.comparison);

    file << "   test %rax, %rax" << endl; // see if %rax is 0 <=> condition is false, skip to end
    file << "   jz .ENDLOOP" << this->current_loop_index << endl;

    int endloop = this->current_loop_index;
    this->current_loop_index++;
    for (statement_node *stmt : while_loop.statements)
    {
        if (stmt->kind == STMT_BREAK)
        {
            file << "   jmp .ENDLOOP" << endloop << endl;
        }
        statement_to_asm(*stmt);
    }
    // if we made it to this point, we loop again!
    file << "   jmp .STARTLOOP" << endloop << endl;
    file << ".ENDLOOP" << endloop << ":" << endl;
}

void Generator::for_loop_to_asm(for_loop_node for_loop)
{
    // start w/ for loop declaration of variable
    declaration_to_asm(for_loop.declaration);
    file << ".STARTLOOP" << this->current_loop_index << ":" << endl;
    // check condition, if true: execute statments, jump back to beginning of loop (check condition again.)
    // if false: skip to after while loop
    // we assume that the result file the comparison is in %rax.
    comparison_to_asm(for_loop.comparison);

    file << "   test %rax, %rax" << endl; // see if %rax is 0 <=> condition is false, skip to end
    file << "   jz .ENDLOOP" << this->current_loop_index << endl;

    int endloop = this->current_loop_index;
    this->current_loop_index++;
    for (statement_node *stmt : for_loop.statements)
    {
        if (stmt->kind == STMT_BREAK)
        {
            file << "   jmp .ENDLOOP" << endloop << endl;
        }
        statement_to_asm(*stmt);
    }
    // now we do our assignment
    assign_to_asm(for_loop.assign);
    // if we made it to this point, we loop again!
    file << "   jmp .STARTLOOP" << endloop << endl;
    file << ".ENDLOOP" << endloop << ":" << endl;
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
    if (binary_expr.lhs.kind == TERM_ARRAY_ACCESS)
    {
        string identifier = binary_expr.lhs.value;
        identifier_type array_type = this->get_type(identifier);
        if (array_type == TYPE_ARRAY_REAL)
        {
            return TYPE_REAL;
        }
    }
    if (binary_expr.rhs.kind == TERM_ARRAY_ACCESS)
    {
        string identifier = binary_expr.rhs.value;
        identifier_type array_type = this->get_type(identifier);
        if (array_type == TYPE_ARRAY_REAL)
        {
            return TYPE_REAL;
        }
    }
    return (binary_expr.lhs.kind == TERM_REAL_LITERAL || binary_expr.rhs.kind == TERM_REAL_LITERAL) ? TYPE_REAL : TYPE_INT;
}