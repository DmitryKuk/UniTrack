// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <system_/utils.h>

#include <unordered_map>

#include <cstdio>

#include <system_/exceptions.h>

using namespace std::literals;


namespace {


constexpr size_t max_output_len = 256;	// Including trailing '\0'


using p = std::pair<std::string, std::string>;

std::unordered_map<std::string, std::string> mime_types_by_extensions{
	{
		p{"html"s,	"text/html"s		},
		p{"css"s,	"text/css"s			},
		p{"js"s,	"text/javascript"s	},
		p{"txt"s,	"text/plain"		}
	}
};


};	// namespace



// Returns given file mime type as a string or throws system_::unknown_error.
std::string
system_::get_file_mime(const std::string &path,
					   const std::string &command)
{
	// Try to analyze file extension
	size_t pos = path.find_last_of("./"s);
	if (pos != std::string::npos && path[pos] == '.') {
		auto it = mime_types_by_extensions.find(path.substr(pos + 1));	// Search for extension
		if (it != mime_types_by_extensions.end())
			return it->second;
	}
	
	
	// Ask external utility
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
