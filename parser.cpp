#include "parser.h"
#include <iostream>
using namespace std;

// constructor
Parser::Parser(vector<Token> tokens) : tokens(tokens), pos(0) {};

// returns current token in tokenfeed
Token Parser::current_token()
{
    return this->tokens[this->pos];
}

// advances to next token in the tokenfeed
void Parser::advance()
{
    this->pos++;
}

// called when invalid syntax/parsing error
void Parser::error(const string &expected)
{
    cout << "[leather] compilation failed:" << endl
         << "   expected " << expected << ", found token ";
    Token token = this->current_token();
    if (token.value != "")
    {
        cout << token.repr() << " (" << token.value << ") @ line " << token.line_number << endl;
    }
    else
    {
        cout << token.repr() << " @ line " << token.line_number << endl;
    }
    exit(0);
};

// parses a list of tokens into a program. returns a program node (AST representation).
// <program> ::= <statement> <statement>* ... <statement>*
program_node Parser::parse_program()
{
    // we will add statements to this and return when done parsing.
    program_node program;
    while (this->current_token().kind != END_OF_FILE)
    {
        program.statements.push_back(this->parse_statement());
    };

    // cout << "Parsed " << program.statements.size() << " statements." << endl;
    return program;
};

statement_node Parser::parse_statement()
{
    statement_node stmt;
    // <statement> ::= <assignment> | 'if' <comparison> 'then' <statement> | 'goto' <label> | 'print' <term> | <label>
    if (this->current_token().kind == IDENTIFIER)
    {
        // this has to be an assignment!
        stmt = this->parse_assignment();
    }
    else if (this->current_token().kind == IF)
    {
        // this is an if then statement
        stmt = this->parse_if_then();
    }
    else if (this->current_token().kind == GOTO)
    {
        // this is a goto...
        stmt = this->parse_goto();
    }
    else if (this->current_token().kind == PRINT)
    {
        stmt = this->parse_print();
    }
    else if (this->current_token().kind == LABEL)
    {
        stmt = this->parse_label();
    }
    else if (this->current_token().kind == WHILE)
    {
        stmt = this->parse_while_loop();
    }
    else if (this->current_token().kind == BREAK)
    {
        this->advance();
        stmt.kind = STMT_BREAK;
    }
    else if (this->current_token().kind == KEYW_BOOL || this->current_token().kind == KEYW_INT || this->current_token().kind == KEYW_REAL)
    {
        stmt = this->parse_declaration();
    }
    else
    {
        error("a statement");
    }
    return stmt;
};

// <while_loop> ::= 'while' <comparison> 'do' '{' <statement> [<statement>]* '}'
statement_node Parser::parse_while_loop()
{
    comparison_node comp;
    vector<statement_node *> stmts;

    if (this->current_token().kind == WHILE)
    {
        this->advance();
        comp = this->parse_comparison();
    }
    else
    {
        error("while");
    }
    if (this->current_token().kind == DO)
    {
        this->advance();
    }
    else
    {
        error("do");
    }
    if (this->current_token().kind == OPEN_BRACKET)
    {
        this->advance();
    }
    else
    {
        error("{");
    }
    while (this->current_token().kind != CLOSE_BRACKET)
    {
        stmts.push_back(new statement_node(this->parse_statement()));
    }
    if (this->current_token().kind == CLOSE_BRACKET)
    {
        this->advance();
    }
    else
    {
        error("}");
    }
    while_loop_node while_loop;
    while_loop.comparison = comp;
    while_loop.statements = stmts;

    statement_node stmt;
    stmt.kind = STMT_WHILE_LOOP;
    stmt.statement = while_loop;
    return stmt;
}

// <assignment> ::= <identifier> = <expr>
statement_node Parser::parse_assignment()
{
    statement_node stmt;
    string identifier;
    expr_node expr;

    if (this->current_token().kind == IDENTIFIER)
    {
        identifier = this->current_token().value;
        this->advance();
    }
    else
    {
        error("identifier");
    }
    if (this->current_token().kind == EQUAL)
    {
        this->advance();
    }
    else
    {
        error("=");
    }
    expr = this->parse_expr();
    stmt.kind = STMT_ASSIGN;
    stmt.statement = assign_node{identifier, expr};
    return stmt;
};

// <expr> ::= <term> | <term> <arithmetic_op> <term>
expr_node Parser::parse_expr()
{
    expr_node expr;
    term_node lhs = this->parse_term();
    term_node rhs;

    if (this->current_token().kind == OP_PLUS)
    {
        this->advance();
        rhs = this->parse_term();

        expr.expr = term_binary_node{lhs, rhs};
        expr.kind = BINARY_EXPR_PLUS;
    }
    else if (this->current_token().kind == OP_MINUS)
    {
        this->advance();
        rhs = this->parse_term();

        expr.expr = term_binary_node{lhs, rhs};
        expr.kind = BINARY_EXPR_MINUS;
    }
    else if (this->current_token().kind == OP_MULT)
    {
        this->advance();
        rhs = this->parse_term();

        expr.expr = term_binary_node{lhs, rhs};
        expr.kind = BINARY_EXPR_MULT;
    }
    else if (this->current_token().kind == OP_DIV)
    {
        this->advance();
        rhs = this->parse_term();

        expr.expr = term_binary_node{lhs, rhs};
        expr.kind = BINARY_EXPR_DIV;
    }
    else if (this->current_token().kind == OP_MOD)
    {
        this->advance();
        rhs = this->parse_term();

        expr.expr = term_binary_node{lhs, rhs};
        expr.kind = BINARY_EXPR_MOD;
    }
    else if (this->current_token().kind == OP_EXPONENT)
    {
        this->advance();
        rhs = this->parse_term();

        expr.expr = term_binary_node{lhs, rhs};
        expr.kind = BINARY_EXPR_EXP;
    }
    else if (this->current_token().kind == OP_RIGHT_SHIFT)
    {
        this->advance();
        rhs = this->parse_term();

        expr.expr = term_binary_node{lhs, rhs};
        expr.kind = BINARY_EXPR_RIGHT_SHIFT;
    }
    else
    {
        // unary expr
        expr.expr = lhs;
        expr.kind = UNARY_EXPR;
    }
    return expr;
};

// <term>:: = <identifier> | <int_literal> | 'input'
term_node Parser::parse_term()
{
    term_node term;
    if (this->current_token().kind == INPUT)
    {
        term.kind = TERM_INPUT;
    }
    else if (this->current_token().kind == INT_LITERAL)
    {
        term.kind = TERM_INT_LITERAL;
        term.value = this->current_token().value;
    }
    else if (this->current_token().kind == REAL_LITERAL)
    {
        term.kind = TERM_REAL_LITERAL;
        term.value = this->current_token().value;
    }
    else if (this->current_token().kind == IDENTIFIER)
    {
        term.kind = TERM_IDENTIFIER;
        term.value = this->current_token().value;
    }
    else
    {
        error("term");
    }
    this->advance();
    return term;
};

// 'if' <comparison> 'then' <statement>
statement_node Parser::parse_if_then()
{
    comparison_node comp;
    vector<statement_node *> stmts;

    if (this->current_token().kind == IF)
    {
        this->advance();
        comp = this->parse_comparison();
    }
    else
    {
        error("if");
    }
    if (this->current_token().kind == THEN)
    {
        this->advance();
    }
    else
    {
        error("then");
    }
    if (this->current_token().kind == OPEN_BRACKET)
    {
        this->advance();
    }
    else
    {
        error("{");
    }
    while (this->current_token().kind != CLOSE_BRACKET)
    {
        stmts.push_back(new statement_node(this->parse_statement()));
    }
    if (this->current_token().kind == CLOSE_BRACKET)
    {
        this->advance();
    }
    else
    {
        error("}");
    }
    if_then_node if_then;
    if_then.comparison = comp;
    if_then.statements = stmts;

    statement_node stmt;
    stmt.kind = STMT_IF_THEN;
    stmt.statement = if_then;
    return stmt;
};

// <comparison> ::= <term> <comparison_op> <term>
comparison_node Parser::parse_comparison()
{
    comparison_node comp;
    bool isnot = false;
    term_node lhs, rhs;

    if (this->current_token().kind == NOT)
    {
        this->advance();
        isnot = true;
    }

    lhs = parse_term();

    // TODO: add more comparison ops later
    if (this->current_token().kind == OP_LT)
    {
        this->advance();
        comp.kind = COMP_LESS_THAN;
    }
    else if (this->current_token().kind == OP_GT)
    {
        this->advance();
        comp.kind = COMP_GREATER_THAN;
    }
    else if (this->current_token().kind == EQUALEQUAL)
    {
        this->advance();
        comp.kind = COMP_EQUAL;
    }
    else if (this->current_token().kind == NOTEQUAL)
    {
        this->advance();
        comp.kind = COMP_NOT_EQUAL;
    }
    else
    {
        error("a comparison operator");
    }

    rhs = parse_term();
    comp.binary_expr = term_binary_node{lhs, rhs};
    comp.is_not = isnot;

    return comp;
};

// 'goto' <label>
statement_node Parser::parse_goto()
{
    statement_node stmt;
    if (this->current_token().kind == GOTO)
    {
        this->advance();
    }
    else
    {
        error("goto");
    }
    if (this->current_token().kind == LABEL)
    {
        stmt.statement = goto_node{this->current_token().value};
    }
    else
    {
        error("label");
    }
    this->advance();
    stmt.kind = STMT_GOTO;
    return stmt;
};

// <label> ::= ':' <str_literal>
statement_node Parser::parse_label()
{
    statement_node stmt;
    if (this->current_token().kind == LABEL)
    {
        stmt.statement = label_node{this->current_token().value};
    }
    else
    {
        error("label");
    }
    this->advance();
    stmt.kind = STMT_LABEL;
    return stmt;
}

statement_node Parser::parse_print()
{
    statement_node stmt;
    if (this->current_token().kind == PRINT)
    {
        this->advance();
    }
    else
    {
        error("print");
    }
    stmt.statement = print_node{this->parse_term()};
    stmt.kind = STMT_PRINT;
    return stmt;
};

statement_node Parser::parse_declaration()
{
    statement_node stmt;
    declaration_node decl;
    if (this->current_token().kind == KEYW_INT)
    {
        decl.type = TYPE_INT;
        this->advance();
    }
    else if (this->current_token().kind == KEYW_REAL)
    {
        decl.type = TYPE_REAL;
        this->advance();
    }
    else if (this->current_token().kind == KEYW_BOOL)
    {
        decl.type = TYPE_BOOL;
        this->advance();
    }
    else
    {
        error("a type");
    }
    if (this->current_token().kind == NOT)
    {
        // optional "!" for fun :)
        this->advance();
    }
    if (this->current_token().kind == IDENTIFIER)
    {
        decl.identifier = this->current_token().value;
        this->advance();
    }
    else
    {
        error("an identifier");
    }
    if (this->current_token().kind == EQUAL)
    {
        this->advance();
        decl.expr = this->parse_expr();
    }
    else
    {
        // default values
        if (decl.type == TYPE_BOOL)
        {
            term_node t;
            t.kind = TERM_INT_LITERAL;
            t.value = "1"; // true
            expr_node expr;
            expr.kind = UNARY_EXPR;
            expr.expr = t;
            decl.expr = expr;
        }
        else if (decl.type == TYPE_INT)
        {
            term_node t;
            t.kind = TERM_INT_LITERAL;
            t.value = "0";
            expr_node expr;
            expr.kind = UNARY_EXPR;
            expr.expr = t;
            decl.expr = expr;
        }
        else if (decl.type == TYPE_REAL)
        {
            term_node t;
            t.kind = TERM_REAL_LITERAL;
            t.value = "0.0";
            expr_node expr;
            expr.kind = UNARY_EXPR;
            expr.expr = t;
            decl.expr = expr;
        }
    }
    stmt.kind = STMT_DECLARATION;
    stmt.statement = decl;
    return stmt;
}

// prints AST of program
void print_program(program_node program)
{
    for (statement_node stmt : program.statements)
    {
        print_statement(stmt);
    }
};

void print_assign(assign_node assign)
{
    cout << "ASSIGN " << assign.identifier << " to ";

    print_expr(assign.expr);

    cout << endl;
};

void print_declaration(declaration_node decl)
{
    cout << "DECLARE ";
    switch (decl.type)
    {
    case TYPE_BOOL:
        cout << "(bool) ";
        break;
    case TYPE_INT:
        cout << "(int) ";
        break;
    case TYPE_REAL:
        cout << "(real) ";
        break;
    default:
        cout << "(type?) ";
    }
    cout << decl.identifier << " as ";
    print_expr(decl.expr);
    cout << endl;
}

void print_expr(expr_node expr)
{
    if (expr.kind == UNARY_EXPR)
    {
        cout << (!get<term_node>(expr.expr).value.empty() ? get<term_node>(expr.expr).value : "INPUT");
    }
    else if (expr.kind == BINARY_EXPR_PLUS)
    {
        term_node lhs, rhs;
        lhs = get<term_binary_node>(expr.expr).lhs;
        rhs = get<term_binary_node>(expr.expr).rhs;

        cout << (!lhs.value.empty() ? lhs.value : "INPUT") << " + " << (!rhs.value.empty() ? rhs.value : "INPUT");
    }
    else if (expr.kind == BINARY_EXPR_MINUS)
    {
        term_node lhs, rhs;
        lhs = get<term_binary_node>(expr.expr).lhs;
        rhs = get<term_binary_node>(expr.expr).rhs;

        cout << (!lhs.value.empty() ? lhs.value : "INPUT") << " - " << (!rhs.value.empty() ? rhs.value : "INPUT");
    }
    else if (expr.kind == BINARY_EXPR_MULT)
    {
        term_node lhs, rhs;
        lhs = get<term_binary_node>(expr.expr).lhs;
        rhs = get<term_binary_node>(expr.expr).rhs;

        cout << (!lhs.value.empty() ? lhs.value : "INPUT") << " * " << (!rhs.value.empty() ? rhs.value : "INPUT");
    }
    else if (expr.kind == BINARY_EXPR_DIV)
    {
        term_node lhs, rhs;
        lhs = get<term_binary_node>(expr.expr).lhs;
        rhs = get<term_binary_node>(expr.expr).rhs;

        cout << (!lhs.value.empty() ? lhs.value : "INPUT") << " / " << (!rhs.value.empty() ? rhs.value : "INPUT");
    }
    else if (expr.kind == BINARY_EXPR_MOD)
    {
        term_node lhs, rhs;
        lhs = get<term_binary_node>(expr.expr).lhs;
        rhs = get<term_binary_node>(expr.expr).rhs;

        cout << (!lhs.value.empty() ? lhs.value : "INPUT") << " % " << (!rhs.value.empty() ? rhs.value : "INPUT");
    }
    else if (expr.kind == BINARY_EXPR_EXP)
    {
        term_node lhs, rhs;
        lhs = get<term_binary_node>(expr.expr).lhs;
        rhs = get<term_binary_node>(expr.expr).rhs;

        cout << (!lhs.value.empty() ? lhs.value : "INPUT") << " ** " << (!rhs.value.empty() ? rhs.value : "INPUT");
    }
    else if (expr.kind == BINARY_EXPR_RIGHT_SHIFT)
    {
        term_node lhs, rhs;
        lhs = get<term_binary_node>(expr.expr).lhs;
        rhs = get<term_binary_node>(expr.expr).rhs;

        cout << (!lhs.value.empty() ? lhs.value : "INPUT") << " >> " << (!rhs.value.empty() ? rhs.value : "INPUT");
    }
    else
    {
        cout << "error printing prgm" << endl;
    }
};

void print_goto(goto_node goto_)
{
    cout << "GOTO " << goto_.label << endl;
};

void print_print(print_node print)
{
    cout << "PRINT ";
    switch (print.term.kind)
    {
    case TERM_INPUT:
        cout << "INPUT" << endl;
        break;
    case TERM_IDENTIFIER:
        cout << print.term.value << endl;
        break;
    case TERM_INT_LITERAL:
        cout << print.term.value << endl;
        break;
    default:
        break;
    }
};

void print_statement(statement_node stmt)
{
    switch (stmt.kind)
    {
    case STMT_ASSIGN:
        print_assign(get<assign_node>(stmt.statement));
        break;
    case STMT_GOTO:
        print_goto(get<goto_node>(stmt.statement));
        break;
    case STMT_IF_THEN:
        print_if_then(get<if_then_node>(stmt.statement));
        break;
    case STMT_LABEL:
        print_label(get<label_node>(stmt.statement));
        break;
    case STMT_PRINT:
        print_print(get<print_node>(stmt.statement));
        break;
    case STMT_WHILE_LOOP:
        print_while_loop(get<while_loop_node>(stmt.statement));
        break;
    case STMT_DECLARATION:
        print_declaration(get<declaration_node>(stmt.statement));
        break;
    case STMT_BREAK:
        cout << "BREAK" << endl;
        break;
    default:
        break;
    }
}

void print_label(label_node label)
{
    cout << "LABEL " << label.label << endl;
};

void print_if_then(if_then_node if_then)
{
    if (if_then.comparison.kind == COMP_LESS_THAN)
    {
        cout << "IF " << get<term_binary_node>(if_then.comparison.binary_expr).lhs.value << " < " << get<term_binary_node>(if_then.comparison.binary_expr).rhs.value << " THEN" << " {" << endl;
    }
    else if (if_then.comparison.kind == COMP_GREATER_THAN)
    {
        cout << "IF " << get<term_binary_node>(if_then.comparison.binary_expr).lhs.value << " > " << get<term_binary_node>(if_then.comparison.binary_expr).rhs.value << " THEN" << " {" << endl;
    }
    else if (if_then.comparison.kind == COMP_EQUAL)
    {
        cout << "IF " << get<term_binary_node>(if_then.comparison.binary_expr).lhs.value << " == " << get<term_binary_node>(if_then.comparison.binary_expr).rhs.value << " THEN" << " {" << endl;
    }
    else
    {
        cout << "IF " << get<term_binary_node>(if_then.comparison.binary_expr).lhs.value << " ?? " << get<term_binary_node>(if_then.comparison.binary_expr).rhs.value << " THEN" << " {" << endl;
    }
    for (statement_node *stmt : if_then.statements)
    {
        print_statement(*stmt);
    }
    cout << "}" << endl;
};

void print_while_loop(while_loop_node while_loop)
{
    if (while_loop.comparison.kind == COMP_LESS_THAN)
    {
        cout << "WHILE " << get<term_binary_node>(while_loop.comparison.binary_expr).lhs.value << " < " << get<term_binary_node>(while_loop.comparison.binary_expr).rhs.value << " DO" << " {" << endl;
    }
    else if (while_loop.comparison.kind == COMP_GREATER_THAN)
    {
        cout << "WHILE " << get<term_binary_node>(while_loop.comparison.binary_expr).lhs.value << " > " << get<term_binary_node>(while_loop.comparison.binary_expr).rhs.value << " DO" << " {" << endl;
    }
    else if (while_loop.comparison.kind == COMP_EQUAL)
    {
        cout << "WHILE " << get<term_binary_node>(while_loop.comparison.binary_expr).lhs.value << " == " << get<term_binary_node>(while_loop.comparison.binary_expr).rhs.value << " DO" << " {" << endl;
    }
    else
    {
        cout << "WHILE " << get<term_binary_node>(while_loop.comparison.binary_expr).lhs.value << " ?? " << get<term_binary_node>(while_loop.comparison.binary_expr).rhs.value << " DO" << " {" << endl;
    }
    for (statement_node *stmt : while_loop.statements)
    {
        print_statement(*stmt);
    }
    cout << "}" << endl;
}