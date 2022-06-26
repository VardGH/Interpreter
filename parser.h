#pragma once

#include <unordered_map>
#include <deque>

#include "token.hpp"

enum class statements_type : uint8_t
{
    function,
    variable,
    start,
    end,
    input,
    output,
    unknown
};

class parser
{
public:
    parser(const std::string& input)
        : m_input(input)
    {}

    parser(const parser& other) = delete;
    parser& operator=(const parser& other) = delete;

    parser(parser&& other) = delete;
    parser& operator=(parser&& other) = delete;


    void parse() const;

private:
    void parse_global_expression(const std::string& expr) const;
    void parse_global_variables(const std::string& expr, size_t delim_pos) const;
    void parse_functions(const std::string& expr, size_t delim_pos) const;

    statements_type string_to_statement(const std::string& input) const;

private:
    std::deque<token> tokenize(const std::string& expr, bool func = false) const;
    std::deque<token> compile(std::deque<token> tokens);

private:
    std::string m_input;
    mutable std::unordered_map<std::string, int> global_variables_;

    static std::unordered_map<std::string, statements_type> statements_;
};