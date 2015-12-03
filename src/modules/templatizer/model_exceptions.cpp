// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/model_exceptions.h>

using namespace std::literals;


templatizer::model_error::model_error(const std::string &what_arg):
	templatizer::error{"Model: "s + what_arg}
{}


templatizer::variable_not_found::variable_not_found(const std::string &var_name):
	templatizer::model_error{"Variable not found: \""s + var_name + '\"'}
{}
