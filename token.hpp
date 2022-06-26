#pragma once

enum token_type
{
	token_open,
	token_close,
	token_pow,
	token_mult,
	token_div,
	token_add,
	token_sub,
	token_num,
	token_var,
	token_comma,
	token_function,
};

struct token
{
public:
    token()
        : contents_{ "" }
		, value_{ 0 }
		, isop_{ false }
		, oppres_{ 0 }
		, isopleft_{ true }
    {}

public:
	token_type type_; // The type of token.
	std::string contents_; // Actual string of the token.
	double value_; // Numbers store their value here.
	bool isop_; // Is it an operatator?
	bool isopleft_; // Does the operator associate to the left or right?
	int oppres_; // Order of operatations.
};
