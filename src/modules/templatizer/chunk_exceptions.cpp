// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/chunk_exceptions.h>

using namespace std::literals;


templatizer::chunk_error::chunk_error(const std::string &chunk_cmd, const std::string &what_arg):
	templatizer::error{"Chunk \""s + chunk_cmd + "\": "s + what_arg}
{}
