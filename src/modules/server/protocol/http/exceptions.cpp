// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol/http/exceptions.h>

using namespace std::literals;


server::protocol::http::error::error(const std::string &what_arg):
	::server::protocol::error(what_arg)
{}


// Start string
server::protocol::http::incorrect_start_string::incorrect_start_string(const std::string &str):
	server::protocol::http::error("Incorrect start string: \""s + str + '\"')
{}

server::protocol::http::uri_parse_error::uri_parse_error(const std::string &uri):
	server::protocol::http::error("Can\'t parse URI: \""s + uri + '\"')
{}

server::protocol::http::unimplemented_method::unimplemented_method(const std::string &method_name):
	server::protocol::http::error("Unimplemented method: \""s + method_name + '\"')
{}

server::protocol::http::incorrect_protocol::incorrect_protocol(const std::string &protocol_name):
	server::protocol::http::error("Incorrect protocol: \""s + protocol_name + '\"')
{}

server::protocol::http::unsupported_protocol_version::unsupported_protocol_version(const std::string &version):
	server::protocol::http::error("Unsupported protocol version: \""s + version + '\"')
{}


// Headers
server::protocol::http::incorrect_header_string::incorrect_header_string(const std::string &str):
	server::protocol::http::error("Incorrect header string: \""s + str + '\"')
{}


server::protocol::http::empty_header_string::empty_header_string():
	server::protocol::http::error("Empty header string"s)
{}


server::protocol::http::header_required::header_required(const std::string &header_name):
	server::protocol::http::error("Header required: \""s + header_name + '\"')
{}


server::protocol::http::incorrect_host_header::incorrect_host_header(const std::string &host_str):
	server::protocol::http::error("Incorrect Host header: \""s + host_str + '\"')
{}


server::protocol::http::incorrect_port::incorrect_port(const std::string &port_str):
	server::protocol::http::error("Incorrect port: \""s + port_str + '\"')
{}
