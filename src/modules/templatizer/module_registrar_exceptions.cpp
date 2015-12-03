// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/module_registrar_exceptions.h>

using namespace std::literals;


templatizer::module_registrar_error::module_registrar_error(const std::string &what_arg):
	templatizer::error{"Module registrar: "s + what_arg}
{}


templatizer::module_not_found::module_not_found(const std::string &module_name):
	templatizer::module_registrar_error{"Module \""s + module_name + "\" not found"s}
{}
