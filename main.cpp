#include <iostream>
#include <fstream>

#include "compiler.h"

int main(int argc, char **argv)
{
    std::cout << "Interpreater" << std::endl;
    std::cout << "Usage: <executable> inputFileName" << std::endl;
    std::string input_file = "";
    if (argc == 1) {
        std::cerr << "missing input agrument, application expectes to get input .ps file" << std::endl;
        return 0;
    } else if (argc == 2) {
        input_file = argv[1];
    } else {
        std::cout << "input params are more than expected" << std::endl;
        return 0;
    }

    compiler comp(input_file);
    comp.compile();
    return 0;
}