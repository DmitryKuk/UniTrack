// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <regex>


template<class KVAction, class KAction>
bool
server::protocol::http::decode_uri_args(const std::string &str,
										KVAction &&kv_action,
										KAction  &&k_action)
{
	using namespace std::literals;
	
	
	static const std::regex args_regex{
		"(\\&|\\?)?"						// [1]: '&' or '?'
		"("
			"([^\\&\\?]+)\\=([^\\&\\?]*)"	// [3], [4]: key, value
		"|"
			"([^\\&\\?]+)"					// [5]: key only
		")"s,
		std::regex::optimize
	};
	
	static const std::regex_iterator<std::string::const_iterator> end;
	
	
	if (!str.empty()) {
		for (std::regex_iterator<std::string::const_iterator> it{str.begin(), str.end(), args_regex};
			 it != end;
			 ++it) {
			if (it->length(3) > 0) kv_action(decode_str(it->str(3)), decode_str(it->str(4)));	// Key-Value pair
			else				   k_action(decode_str(it->str(5)));							// Key without value
		}
	}
	
	
	return true;
}



template<class KVAction, class KAction>
bool
server::protocol::http::decode_uri(const std::string &str,
								   std::string &path,
								   KVAction &&kv_action,
								   KAction  &&k_action)
{
	using namespace std::literals;
	
	
	static const std::regex base_regex{
		"([^?]+)(\\?(.*))?"s,				// [1], [4]: path and args
		std::regex::optimize
	};
	
	
	std::smatch m;
	if (!std::regex_match(str, m, base_regex))
		return false;
	
	path = m.str(1);
	
	return decode_uri_args(m.str(3), std::forward<KVAction>(kv_action), std::forward<KAction>(k_action));
}
