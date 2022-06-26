#include <iostream>

#include "compiler.h"

void compiler::compile() const
{
    try {
        m_parser.parse();
    } catch (...) {
        std::cerr << "input file parseing failed" << std::endl;
        return;
    }
}