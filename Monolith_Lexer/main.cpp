#include "Generator.hpp"
#include <fstream>

int main()
{
    Lexer::Generator lexer("input.mon");
    std::fstream output("output.lex", std::ios::out | std::ios::trunc);
    output << lexer;
    return 0;
}