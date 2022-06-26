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

std::deque<token> parser::tokenize(const std::string& expr, bool func) const
{
	int i = 0;
	std::deque<token> result = std::deque<token>();

	while (i != expr.length())
	{
		char c = expr[i++]; // Get the character and move to the next character. After
		// this i will be pointing to the next character not that current one.
		token t;

		switch (c) {
		case '(':
			t.type_ = token_type::token_open;
			t.contents_ = "(";
			break;
		case ')':
			t.type_ = token_type::token_close;
			t.contents_ = ")";
			break;
		case '^':
			t.type_ = token_type::token_pow;
			t.contents_ = "^";
			t.isop_ = true;
			t.oppres_ = 3;
			t.isopleft_ = false;
			break;
		case '*':
			t.type_ = token_type::token_mult;
			t.contents_ = "*";
			t.isop_ = true;
			t.oppres_ = 2;
			break;
		case '/':
			t.type_ = token_type::token_div;
			t.contents_ = "/";
			t.isop_ = true;
			t.oppres_ = 2;
			break;
		case '+':
			t.type_ = token_type::token_add;
			t.contents_ = "+";
			t.isop_ = true;
			t.oppres_ = 1;
			break;
		case ',':
			t.type_ = token_type::token_comma;
			t.contents_ = ",";
			break;
		default:
			if (isdigit(c) || c == '.' || (c == '-' && (isdigit(expr[i]) || expr[i] == '.'))) {
				std::string buffer;
				--i;
				while (isdigit(c) || c == '.' || c == '-') {
					buffer += c;
					c = expr[++i];
				}

				t.type_ = token_type::token_num;
				t.contents_ = buffer;
				t.value_ = std::atof(buffer.c_str());
			} else if (c == '-') {
				t.type_ = token_type::token_sub;
				t.contents_ = "-";
				t.isop_ = true;
				t.oppres_ = 1;
			} else if (isspace(c)) {
				continue;
            } else if (isalpha(c)) {
				t.type_ = token_type::token_var;
				t.contents_ = c;

				if (isalpha(expr[i])) {
					t.contents_ = "";
					t.isop_ = false;
					i--;
					while (isalpha(c)) {
						t.contents_ += c;
						c = expr[++i];
					}
				}
                if (func) {
                    t.type_ = token_type::token_function;
                    t.oppres_ = 4; // Functions always come before anything else.
                }
			}
			else // Unkown token. Error!
				throw std::logic_error("Unkown token!");
			break;
		}

		result.push_back(t);
	}

	return std::move(result);
}

std::deque<token> parser::compile(std::deque<token> tokens)
{
	std::deque<token> list = std::deque<token>();
	std::stack<token> opstack = std::stack<token>();

	token t;
	while (tokens.size() != 0) {
		t = tokens.front();
		tokens.pop_front();

		if (t.type_ == token_type::token_num || t.type_ == token_type::token_var) {
			list.push_back(t);
        } else if (t.type_ == token_type::token_function) {
			opstack.push(t);
        } else if (t.type_ == token_type::token_comma) {
			while (!opstack.empty() && opstack.top().type_ != token_type::token_open) {
				token t = opstack.top();
				opstack.pop();
				list.push_back(t);
			}
			if (opstack.empty()) {
                throw std::logic_error("Bad function call syntax.");
            }
		} else if (t.isop_) {
			while (!opstack.empty() && opstack.top().isop_) {
				if ((t.isopleft_ && (t.oppres_ <= opstack.top().oppres_)) || (!t.isopleft_ && (t.oppres_ < opstack.top().oppres_))) {
					token t = opstack.top();
					opstack.pop();
					list.push_back(t);
				}
				else
					break;
			}
			opstack.push(t);
		} else if (t.type_ == token_type::token_open) {
			opstack.push(t);
        } else if (t.type_ == token_type::token_close) {
			while (!opstack.empty() && opstack.top().type_ != token_type::token_open) {
				token t = opstack.top();
				opstack.pop();
				list.push_back(t);
			}
			if (opstack.empty()) throw std::logic_error("Bracket mismatch!");
			opstack.pop();
			if (!opstack.empty() && opstack.top().type_ == token_type::token_function) {
				token t = opstack.top();
				opstack.pop();
				list.push_back(t);
			}
		}
	}

	// Move anything left on the stack to the output list.
	while (!opstack.empty()) {
		token t = opstack.top();
		if (t.type_ == token_type::token_open || t.type_ == token_type::token_close) {
            throw std::logic_error("Bracket mismatch!");
        }
		opstack.pop();
		list.push_back(t);
	}

	return list;
}
