// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/path_utils.h>

#include <regex>
#include <stdexcept>

using namespace std::literals;


// Returns first section and trailing path or throws std::logic_error.
// Example: "/user/photo/123?args=..." => ("user", "/photo/123?args=...")
std::pair<std::string, std::string>
host::split_first_section(const std::string &path)
{
	static const std::regex regex{	// Example: "/user/photo/123?args=..."
		"^"
		"/"							//     Leading '/' symbol
		"([^/?]*)"					// [1] Leading section name: "user"
		"([/?].*)?"					// [2] Trailing path: "/photo/123?args=..."
		"$"s,
		std::regex::optimize
	};
	
	
	// Parsing requested path
	std::smatch m;
	if (!std::regex_match(path, m, regex))
		throw std::logic_error{"Incorrect path"s};
	
	
	return {m[1], m[2]};
}
