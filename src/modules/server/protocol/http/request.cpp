// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol/http/request.h>

#include <regex>
#include <stdexcept>
#include <cctype>

#include <boost/lexical_cast.hpp>

#include <server/protocol/http/exceptions.h>

using namespace std::literals;


namespace {


void process_header_content_length(::server::protocol::http::request &request,
								   const std::string & /* header */, const std::string &value)
{
	request.content_length = boost::lexical_cast<decltype(request.content_length)>(value);
}


void process_header_connection(::server::protocol::http::request &request,
							   const std::string & /* header */, const std::string &value)
{
	static const std::regex keep_alive_regex{".*keep-alive.*"s, std::regex::optimize};
	
	request.keep_alive = regex_match(value, keep_alive_regex);
}


void process_header_host(::server::protocol::http::request &request,
						 const std::string & /* header */, const std::string &value)
{
	static const std::regex regex{"([^:]+)"					// Host name [1]
								  "(:([[:digit:]]+))?"s,	// Port number, if specified [3]
								  std::regex::optimize};
	
	
	std::smatch match;
	if (!std::regex_match(value, match, regex) || match.size() != 4)
		throw ::server::protocol::http::incorrect_host_header{value};
	
	// Host
	request.host = match[1];
	
	// Port
	if (match.length(3) > 0)
		request.port = boost::lexical_cast<decltype(request.port)>(match[3]);
}


void process_header_cookie(::server::protocol::http::request &request,
						   const std::string & /* header */, const std::string &value)
{
	static const std::regex regex{"([[:alpha:]_][[:alnum:]_]*)=([[:alnum:]_]+)"s,
								  std::regex::optimize};
	static const std::regex_iterator<std::string::const_iterator> end;
	
	
	// Looking for all pairs like 'name=value'
	for (std::regex_iterator<std::string::const_iterator> it{value.begin(), value.end(), regex}; it != end; ++it)
		request.cookies.emplace(it->str(1), it->str(2));
}


using process_header_fn_type = void (*)(::server::protocol::http::request &,
										const std::string &, const std::string &);

using ph_pair = std::pair<std::string, process_header_fn_type>;


const std::unordered_map<std::string, process_header_fn_type> process_header_fn_map =
	{
		ph_pair{::server::protocol::http::header::content_length,	process_header_content_length	},
		ph_pair{::server::protocol::http::header::connection,		process_header_connection		},
		ph_pair{::server::protocol::http::header::host,				process_header_host				},
		ph_pair{::server::protocol::http::header::cookie,			process_header_cookie			}
	};


};	// namespace



// Prepares request for new data from same client
// Resets buffer and keep_alive, saving client_address
void
server::protocol::http::request::reset() noexcept
{
	this->::server::protocol::request::reset();
	
	// Protocol info
	this->method  = ::server::protocol::http::method::unknown;
	this->version = ::server::protocol::http::version::unknown;
	
	// URI
	this->uri.clear();
	this->path.clear();
	this->args_map.clear();
	this->args_set.clear();
	
	// Headers
	this->host.clear();
	this->port = ::server::protocol::http::default_port;
	this->content_length = 0;
	this->cookies.clear();
	this->parsed_headers.clear();
	this->unparsed_headers.clear();
	
	// Body
	this->body.clear();
	this->body_bytes_read_ = 0;
}


// Fills request data from the stream
// Returns 0, if no additional data required, otherwise returns number of bytes to read
size_t
server::protocol::http::request::process_stream(std::istream &stream)
{
	this->reset();	// Clear old data
	
	std::string str;
	
	// Processing start string
	std::getline(stream, str);
	this->process_start_string(str);
	
	// Processing headers
	while (std::getline(stream, str) && !str.empty() && this->process_header_string(str))
		;
	
	// Processing URI
	this->process_uri();
	
	// No request body to read
	if (this->content_length == 0)
		return 0;
	
	if (this->method != ::server::protocol::http::method::POST)
		throw ::server::protocol::http::content_length_must_be_0{
			::server::protocol::http::method_to_str(this->method)
		};
	
	// Try to read request body
	this->body.resize(this->content_length);
	return this->process_stream_again(stream);
}


// Fills request body with bytes_transferred number of bytes
// Returns 0, if no additional data required, otherwise returns number of bytes to read
size_t
server::protocol::http::request::process_stream_again(std::istream &stream)
{
	size_t bytes_to_read = this->content_length - this->body_bytes_read_;
	if (bytes_to_read == 0)
		return 0;
	
	stream.readsome(this->body.data() + this->body_bytes_read_, bytes_to_read);
	this->body_bytes_read_ += stream.gcount();
	
	return this->content_length - this->body_bytes_read_;
}


// private
void
server::protocol::http::request::process_start_string(const std::string &str)
{
	static const std::regex
		regex{"^"
			  "([_[:alnum:]]+)"		"[[:space:]]+"				// Method	[1]
			  "([^[:space:]]+)"		"[[:space:]]+"				// URI		[2]
			  "([_[:alnum:]]+)/([[:digit:]]+\\.[[:digit:]]+)"	// Protocol	[3]/[4]
			  "[\r\n]*$"s,
			  std::regex::optimize};
	
	static const auto str_toupper =
		[](std::string str) -> std::string
		{
			for (auto &ch: str) ch = std::toupper(ch);
			return str;
		};
	
	
	std::smatch match;	// Match results
	
	
	if (!std::regex_match(str, match, regex) || match.size() != 5)
		throw ::server::protocol::http::incorrect_start_string{str};
	
	// Setting up method
	this->method = ::server::protocol::http::str_to_method(str_toupper(match[1]));
	
	// Protocol validating
	{
		auto protocol_name = str_toupper(match[3]);
		if (protocol_name != ::server::protocol::http::str::HTTP)
			throw ::server::protocol::http::incorrect_protocol{protocol_name};
	}
	
	// Protocol version validating
	{
		using ::server::protocol::http::version_to_str;
		using ::server::protocol::http::str_to_version;
		
		this->version = str_to_version(str_toupper(match[4]));
		if (this->version != ::server::protocol::http::version::v_1_1)
			throw ::server::protocol::http::unsupported_protocol_version{version_to_str(this->version)};
	}
	
	// Setting up the uri
	this->uri = match[2];
}


bool
server::protocol::http::request::process_header_string(const std::string &str)
{
	static const std::regex
		regex{"^"
			  "([^[:space:]]+)"		// Key [1]
			  ":[[:space:]]*"
			  "(.+)"				// Value [2]
			  "[\r\n]*$"s,
			  std::regex::optimize},
		regex_empty{"^"
					"[\r[:space:]]*"
					"$"s,
					std::regex::optimize};
	
	std::smatch match;
	
	if (!std::regex_match(str, match, regex) || match.size() != 3) {
		if (std::regex_match(str, regex_empty)) return false;
		
		throw ::server::protocol::http::incorrect_header_string{str};
	}
	
	std::string header = match[1],
				value  = match[2];
	
	auto it = process_header_fn_map.find(header);
	if (it == process_header_fn_map.end()) {
		this->unparsed_headers.emplace(std::move(header), std::move(value));
	} else {
		try {
			it->second(*this, header, value);
		} catch (...) {
			throw ::server::protocol::http::incorrect_header_string{str};
		}
		
		this->parsed_headers.emplace(std::move(header), std::move(value));
	}
	
	return true;
}


void
server::protocol::http::request::process_uri()
{
	static const std::regex
		base_regex{
			"([^?]+)(\\?(.*))?"s,				// [1], [4]: path and args
			std::regex::optimize
		},
		
		args_regex{
			"(\\&|\\?)?"						// [1]: '&' or '?'
			"("
				"([^\\&\\?]+)\\=([^\\&\\?]*)"	// [3], [4]: key, value
			"|"
				"([^\\&\\?]+)"					// [5]: key only
			")"s,
			std::regex::optimize
		};
	
	static const std::regex_iterator<std::string::const_iterator> end;
	
	
	std::smatch m;
	
	if (!std::regex_match(this->uri, m, base_regex))
		throw ::server::protocol::http::uri_parse_error{this->uri};
	
	
	this->path = m[1].str();
	
	auto args_sm = m[3];
	if (args_sm.length() != 0) {	// URI containg args => parsing too
		std::string args = args_sm;
		
		auto begin = std::regex_iterator<std::string::const_iterator>(args.begin(), args.end(), args_regex);
		std::for_each(begin, end,
			[&](const auto &m) {
				auto key = m[3];
				if (key.length() == 0) {
					this->args_set.emplace(m[5]);		// Key without value
				} else {
					this->args_map.emplace(key, m[4]);	// Key-Value pair
				}
			});
	}
}


std::ostream &
server::protocol::http::operator<<(std::ostream &stream, const ::server::protocol::http::request &request)
{
	stream
		<< "Client: "s << request.client_address << ": "s
		<< ::server::protocol::http::str::HTTP << ::server::protocol::http::str::slash
		<< ::server::protocol::http::version_to_str(request.version)
		<< ", "s << ::server::protocol::http::method_to_str(request.method)
		<< ", Requested URI: \""s << request.uri
		<< "\". Host: \""s << request.host << "\", port: "s << request.port << '.';
	
	if (!request.parsed_headers.empty()) {
		stream << " Parsed headers:"s;
		
		for (const auto &p: request.parsed_headers)
			stream << " ["s << p.first << ": "s << p.second << ']';
		
		stream << '.';
	}
	
	if (!request.unparsed_headers.empty()) {
		stream << " Unparsed headers:"s;
		
		for (const auto &p: request.unparsed_headers)
			stream << " ["s << p.first << ": "s << p.second << ']';
		
		stream << '.';
	}
	
	if (!request.cookies.empty()) {
		stream << " Cookies:"s;
		
		for (const auto &p: request.cookies)
			stream << " ["s << p.first << ": "s << p.second << ']';
		
		stream << '.';
	}
	
	if (request.content_length != 0)
		stream << " Content length: "s << request.content_length << '.';
	
	return stream;
}
