// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_HTTP_UTILS_H
#define SERVER_PROTOCOL_HTTP_UTILS_H

#include <string>


namespace server {
namespace protocol {
namespace http {


// Parses string, decoded as URI. Calls handlers, when element found.
// Returns true, if uri parsed successfully, or false otherwise.
// NOTE: kv_action and/or k_action can be called even if false returned!

// Arguments:
// str can have '?' or '&' as first character, it will be ignored. So, even trailing path of URI can be parsed.
// All '&'s in str will be interpreted as key-values/keys separators.
// Each sequence like "%XX" will be converted to symbol with code XX (both X are hexademical numbers).

// Example:
//	std::unordered_multimap<std::string, std::string> map;
//	std::unordered_multimap<std::string> set;
//	decode_uri_args(
//		"?key1=value1&key2&key3=value3",
//		std::bind(decltype(map)::emplace, map, _1, _2),
//		std::bind(decltype(set)::emplace, set, _1)
//	);
//	// Now map == { {"key1", "value1"}, {"key3", "value3"} }
//	// Now set == { "key2" }

template<class KVAction, class KAction>
bool decode_uri_args(const std::string &str,	// String to parse
					 KVAction &&kv_action,		// Will be called as kv_action(key, value), when kv-pair found
					 KAction  &&k_action);		// Will be called as k_action(key), when key without value found
												// NOTE: key and value may be moved to function.



// Same as decode_uri_args, but parses full URI (without "protocol://").
// Returns true, if uri parsed successfully, or false otherwise.
// NOTE: path can be modified and kv_action and/or k_action can be called even if false returned!

// Example:
//	std::string path;
//	if (decode_uri("/path/to/file.html?key1=value&key2&...", path, ..., ...))
//		// path == "/path/to/file.html"

template<class KVAction, class KAction>
bool decode_uri(const std::string &str,		// String to parse
				std::string &path,			// Reference to path string
				KVAction &&kv_action,		// Will be called as kv_action(key, value), when kv-pair found
				KAction  &&k_action);		// Will be called as k_action(key), when key without value found
											// NOTE: key and value may be moved to function.



// Decodes str: replaces '+' by ' ' and each "%XX" by char with code XX.
// Returns decoded string.
std::string decode_str(std::string str);


};	// namespace http
};	// namespace protocol
};	// namespace server


#include <server/protocol/http/utils.hpp>

#endif	// SERVER_PROTOCOL_HTTP_UTILS_H
