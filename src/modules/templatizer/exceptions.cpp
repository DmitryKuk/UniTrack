// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/exceptions.h>

using namespace std::literals;


templatizer::error::error(const std::string &what_arg):
	std::logic_error{"Templatizer: "s + what_arg}
{}
