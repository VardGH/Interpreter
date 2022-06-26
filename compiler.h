#pragma once

#include <string>

#include "parser.h"

class compiler
{
public:
    compiler(const std::string& input)
        : m_input(input)
        , m_parser(input)
    {}

    compiler(const compiler& other) = delete;
    compiler& operator=(const compiler& other) = delete;

    compiler(compiler&& other) = delete;
    compiler& operator=(compiler&& other) = delete;


    void compile() const;

private:
    std::string m_input;
    parser m_parser;
};