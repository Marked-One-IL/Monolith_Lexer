#include "Lexer/Generator.hpp"
#include <fstream>

int main(int argc, char** argv)
{
    const char* inputFileName = "../TestIO/input.mon";
    const char* outputFileName = "../TestIO/output.lex";

    if (argc >= 2)
    {
        inputFileName = argv[1];
    }
    if (argc >= 3)
    {
        outputFileName = argv[2];
    }

    Lexer::Generator lexer(inputFileName);
    std::fstream output(outputFileName, std::ios::out | std::ios::trunc);
    output << lexer;
    return 0;
}