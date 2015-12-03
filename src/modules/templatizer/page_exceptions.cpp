// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/page_exceptions.h>

using namespace std::literals;


templatizer::page_error::page_error(const std::string &what_arg):
	templatizer::error{"Page: "s + what_arg}
{}


templatizer::file_mapping_error::file_mapping_error(const std::string &file_name,
													const std::string &message):
	templatizer::page_error{"File \""s + file_name + "\" mapping error: "s + message}
{}


templatizer::file_parsing_error::file_parsing_error(const std::string &file_name,
													const std::string &message):
	templatizer::page_error{"File \""s + file_name + "\" parsing error: "s + message}
{}
