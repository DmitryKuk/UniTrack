// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol/http/utils.h>

#include <regex>

using namespace std::literals;


// Decodes str: replaces '+' by ' ' and each "%XX" by char with code XX.
// Returns decoded string.
std::string
server::protocol::http::decode_str(std::string str)
{
	static const std::regex regex{
		"%([[:xdigit:]]{2,2})"s,	// "%XX", where both X-s are hexademical numbers
		std::regex::optimize
	};
	
	static const std::regex_iterator<std::string::const_iterator> end;
	
	
	// Step 1: replace all '+' by spaces
	for (auto &c: str)
		if (c == '+') c = ' ';
	
	
	// Step 2: replace all %XX to chars with code XX
	std::string res;
	res.reserve(str.size());
	
	
	size_t str_pos_old = 0;
	const auto add_data =
		[&](size_t str_pos_curr)
		{
			size_t len = str_pos_curr - str_pos_old;
			if (len > 0) {
				res.append(str, str_pos_old, len);
				str_pos_old = str_pos_curr;
			}
		};
	
	
	for (std::regex_iterator<std::string::const_iterator> it{str.begin(), str.end(), regex};
		 it != end;
		 ++it) {
		// Adding previous block of non-replaced data
		add_data(it->position());
		
		// Adding current replaced data
		res += static_cast<char>(std::stoi(it->str(1), nullptr, 16));
		str_pos_old += it->length();
	}
	
	// Adding the last block of non-replaced data
	add_data(str.size());
	
	
	return res;
}
