 #include <fstream>
 #include <string>
 #include <iostream>
 #include <cassert>
 #include <exception>
 #include <vector>

 #include "parser.h"

namespace {
 
    const char expression_separator = ';';
    const char comment_separator = '#';
    const char assignment_separator = '=';
    const char open_func_separator = '(';
    const char close_func_separator = ')';
    const std::string whitespaces = " \t";
    
    std::string trim(const std::string& str)
    {
        const auto start_pos = str.find_first_not_of(whitespaces);
        if (start_pos == std::string::npos)
            return ""; // no content

        const auto end_pos = str.find_last_not_of(whitespaces);

        return std::move(str.substr(start_pos, end_pos - start_pos + 1));
    }

    std::vector<std::string> split(const std::string& str, char delim = ',')
    {
        std::vector<std::string> result;
        size_t pos = 0;
        size_t curr_pos = 0;
        std::string token;
        while ((pos = str.find_first_of(delim, pos)) != std::string::npos) {
            token = trim(str.substr(curr_pos, pos - curr_pos));
            curr_pos = pos+1;
            pos = pos+1;
            result.emplace_back(std::move(token));
        }
        result.emplace_back(std::move(trim(str.substr(curr_pos))));
        return std::move(result);
    }
}

std::unordered_map<std::string, statements_type> parser::statements_ = {
    {"VAR", statements_type::variable},
    {"FUNC", statements_type::function},
    {"Begin", statements_type::start},
    {"End", statements_type::end},
    {"In", statements_type::input},
    {"Out", statements_type::output},
};

statements_type parser::string_to_statement(const std::string& input) const
{
    auto iter = statements_.find(input);
    if (iter == statements_.end()) {
        return statements_type::unknown;
    }
    return iter->second;
}

void parser::parse_global_variables(const std::string& expr, size_t delim_pos) const
{
    size_t assign_pos = expr.find_first_of(assignment_separator, delim_pos + 1);
    if (assign_pos == std::string::npos){
        //TODO throw invalid_statement exception
    }
    std::string variable_name = expr.substr(delim_pos + 1, assign_pos - delim_pos - 1);
    variable_name = std::move(trim(variable_name));
    std::string variable_value = expr.substr(assign_pos + 1);

    try {
        int value = std::stoi(variable_value);
        //std::cout << "VAR: " << variable_name << " = " << value << std::endl;
        global_variables_.emplace(std::move(variable_name), value);
    } catch (const std::invalid_argument& ia) {
        //TODO wrap the exception with internal
    } catch (const std::invalid_argument& ex) {     
    }
}

void parser::parse_functions(const std::string& expr, size_t delim_pos) const
{
    size_t assign_pos = expr.find_first_of(assignment_separator, delim_pos+1);
    if (assign_pos == std::string::npos) {
        //It means that it is function declaration
        size_t open_pos = expr.find_first_of(open_func_separator, delim_pos+1);
        size_t close_pos = expr.find_first_of(close_func_separator, open_pos+1);
        if (open_pos == std::string::npos || close_pos == std::string::npos) {
            //TODO throw and exepction about missing_separator
        }
        std::string func_name = expr.substr(delim_pos+1, open_pos-delim_pos-1);
        func_name = std::move(trim(func_name));

        std::string arguments = expr.substr(open_pos+1, close_pos-open_pos-1);
        auto args = split(arguments);
    }
}

void parser::parse_global_expression(const std::string& expr) const
{
    size_t start_pos = expr.find_first_not_of(whitespaces);
    if (start_pos == std::string::npos) {
        return; // empty expression
    }

    auto trimmed = expr.substr(start_pos);
    size_t delim_pos = trimmed.find_first_of(whitespaces);
    auto statement = trimmed.substr(0, delim_pos);
    std::cout << "expr: " << expr << " statement: " << statement << std::endl;
    
    switch (string_to_statement(statement)) {
    case statements_type::variable: {
        parse_global_variables(trimmed, delim_pos);
        break;
    }
    case statements_type::function: {
        parse_functions(trimmed, delim_pos);
        break;
    }
    default:
        assert(false);
        break;
    }
    //TODO parsing logic
    
}

 void parser::parse() const
 {
    std::ifstream infile = std::ifstream(m_input, std::ios_base::in);
    std::string line;
    bool reached_main = false;
    while (std::getline(infile, line)) {
        // remove from line comments, if those are exists
        line = line.substr(0, line.find_first_of(comment_separator));
        line = std::move(trim(line));
        if (!reached_main && line == "Begin") {
            std::cout << "reached to main function: " << std::endl;
            reached_main = true;
        }
        std::cout << "Line: " << line << std::endl;
        size_t curr_pos = 0;
        size_t pos = line.find_first_of(expression_separator, 0);
        // check and error out case if the expression separator is missed
        while (pos != std::string::npos && !reached_main) {
            auto expr =  line.substr(curr_pos, pos-curr_pos);
            parse_global_expression(expr);
            curr_pos = pos += 1;
            pos = line.find_first_of(expression_separator, curr_pos);
            // check and error out case if the expression separator is missed
        }
    }
 }