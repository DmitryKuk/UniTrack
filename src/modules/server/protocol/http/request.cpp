// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol/http/request.h>

#include <istream>
#include <regex>
#include <stdexcept>
#include <cctype>

#include <boost/lexical_cast.hpp>

#include <server/protocol/http/exceptions.h>


// Call this method, when all client response saved in this->buffer
void
server::protocol::http::request::process_buffer()
{
	std::istream stream(this->buffer);
	std::string str;
	
	
	// Processing start string
	std::getline(stream, str);
	server::parse_start_string(str);
	
	
	// Processing headers
	while (std::getline(stream, str) && !str.empty())
		this->add_header(server::parse_header_string(str));
	
	
	// Setting up keep-alive
	try {
		static const std::regex keep_alive_regex(".*keep-alive.*", std::regex::optimize);
		
		this->keep_alive = regex_match(this->headers.at(server::protocol::http::header_connection),
									   keep_alive_regex);
	} catch (const std::out_of_range &) {}
}


std::pair<std::string, server::port_type>
server::protocol::http::request::host_and_port()
{
	static const std::regex host_regex("([^:]+)"				// Host name [1]
									   "(:([[:digit:]]+))?",	// Port number, if specified [3]
									   std::regex::optimize);
	
	
	auto header_host_it = this->headers.find(server::protocol::http::str::header_host);
	if (header_host_it == this->headers.end())
		throw server::protocol::http::header_required(server::protocol::http::str::header_host);
	
	
	// Host checking
	const std::string &host_str = header_host_it->second;
	
	std::smatch match;
	if (!std::regex_match(host_str, match, host_regex) || match.size() != 4)
		throw server::protocol::http::incorrect_host_header(host_str);
	
	
	// Port checking
	server::port_type port = server::protocol::http::default_port;
	if (if match.length(3) > 0) {
		std::string port_str = match[3];
		try {
			port = boost::lexical_cast<server::port_type>(port_str);
		} catch (const boost::bad_lexical_cast &) {
			throw server::protocol::http::incorrect_port(port_str);
		}
	}
	
	return { match[1], port };
}


// private
void
server::protocol::http::request::process_start_string(const std::string &str)
{
	static const std::regex
		regex("^"
			  "([_[:alnum:]]+)"		"[[:space:]]+"				// Method	[1]
			  "([^[:space:]]+)"		"[[:space:]]+"				// URI		[2]
			  "([_[:alnum:]]+)/([[:digit:]]+\\.[[:digit:]]+)"	// Protocol	[3]/[4]
			  "[\r\n]*$",
			  std::regex::optimize);
	
	static const auto str_toupper =
		[](std::string str) -> std::string
		{
			for (auto &ch: str) ch = std::toupper(ch);
			return str;
		};
	
	
	std::smatch match;	// Match results
	
	
	if (!std::regex_match(str, match, regex) || match.size() != 5)
		throw server::protocol::http::incorrect_start_string(str);
	
	// Setting up method
	this->method = server::protocol::http::str_to_method(str_toupper(match[1]));
	
	// Protocol validating
	{
		auto protocol_name = str_toupper(match[3]);
		if (protocol_name != "HTTP") throw server::incorrect_protocol(protocol_name);
	}
	
	// Protocol version validating
	{
		this->version = server::protocol::http::str_to_version(str_toupper(match[4]));
		if (this->version != server::protocol::http::version::v_1_1)
			throw server::protocol::http::unsupported_protocol_version(protocol_ver);
	}
	
	// Setting up the uri
	this->uri = match[2];
}


void
server::protocol::http::request::process_header_string(const std::string &str)
{
	static const std::regex
		regex("^"
			  "([^[:space:]]+)"		// Key [1]
			  ":[[:space:]]*"
			  "(.+)"				// Value [2]
			  "[\r\n]*$",
			  std::regex::optimize),
		regex_empty("^"
					"[\r[:space:]]*"
					"$",
					std::regex::optimize);
	
	std::smatch match;
	
	if (!std::regex_match(str, match, regex) || match.size() != 3) {
		if (std::regex_match(str, regex_empty)) return;
		
		throw server::protocol::http::incorrect_header_string(str);
	}
	
	this->headers.emplace(match[1], match[2]);
}


void
server::protocol::http::request::process_uri()
{
	static const std::regex
		base_regex(
			"([^?]+)(\\?(.*))?",				// [1], [4]: path and args
			std::regex::optimize
		),
		
		args_regex(
			"(\\&|\\?)?"						// [1]: '&' or '?'
			"("
				"([^\\&\\?]+)\\=([^\\&\\?]*)"	// [3], [4]: key, value
			"|"
				"([^\\&\\?]+)"					// [5]: key only
			")",
			std::regex::optimize
		);
	
	static const std::regex_iterator<std::string::const_iterator> end;
	
	
	std::smatch m;
	
	if (!std::regex_match(this->uri, m, base_regex))
		throw server::protocol::http::uri_parse_error(this->uri);
	
	
	this->path = m[1].str();
	
	auto args_sm = m[3];
	if (args_sm.length() != 0) {	// URI containg args => parsing too
		std::string args = args_sm;
		
		auto begin = std::regex_iterator<std::string::const_iterator>(args.begin(), args.end(),
																	  args_regex);
		std::for_each(begin, end,
			[this] (const auto &m) {
				auto key = m[3];
				if (key.length() == 0) {
					this->args_set.emplace(m[5]);		// Key without value
				} else {
					this->args_map.emplace(key, m[4]);	// Key-Value pair
				}
			});
	}
}
