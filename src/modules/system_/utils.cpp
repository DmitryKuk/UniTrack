// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <system_/utils.h>

#include <cstdio>

#include <system_/exceptions.h>

using namespace std::literals;


namespace {

constexpr size_t max_output_len = 256;	// Including trailing '\0'

};	// namespace


// Returns given file mime type as a string or throws system_::unknown_error.
std::string
system_::get_file_mime(const std::string &path,
					   const std::string &command)
{
	std::string command_full = command + " \'"s + path + '\'';
	std::FILE *stream = ::popen(command_full.c_str(), "r");
	char str[max_output_len];
	
	if (stream == nullptr
		|| std::fgets(str, max_output_len, stream) == nullptr
		|| ::pclose(stream) != 0)
		throw system_::unknown_error{};
	
	std::string res{str};
	res.pop_back();	// Delete last '\n' symbol
	return std::move(res);
}
