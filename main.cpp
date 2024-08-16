#include "lexer.h"
#include "parser.h"
#include "generator.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    string file_name = argv[1];
    string output_name;
    if (argc == 2)
    {
        size_t dotPosition = file_name.rfind('.');
        output_name = file_name.substr(0, dotPosition) + ".s";
    }
    else if (argc == 3)
    {
        output_name = argv[2];
    }
    else
    {
        cout << "Invalid number of arguments" << endl;
        exit(0);
    }

    string buffer = read_file(file_name);
    Lexer lexer(buffer);

    vector<Token> tokens = lexer.tokenize();
    // print_tokens(tokens);

    Parser parser(tokens);
    program_node p = parser.parse_program();
    // print_program(p);

    Generator generator(p, output_name, parser.get_strings());
    generator.to_asm();

    cout << "[leather] successfully compiled: " << endl
         << "    " << file_name << " -> " << output_name << endl;

    return 0;
}