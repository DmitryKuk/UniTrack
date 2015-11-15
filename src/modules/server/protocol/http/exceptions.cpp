// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol/http/exceptions.h>


server::protocol::http::protocol_error::protocol_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


// Start string
server::protocol::http::incorrect_start_string::incorrect_start_string(const std::string &str):
	server::protocol::http::protocol_error("Incorrect start string: \"" + str + "\"")
{}

server::protocol::http::uri_parse_error::uri_parse_error(const std::string &method_name):
	server::protocol::http::protocol_error("Can\'t parse URI: \"" + uri + '\"')
{}

server::protocol::http::unimplemented_method::unimplemented_method(const std::string &method_name):
	server::protocol::http::protocol_error("Unimplemented method: \"" + method_name + '\"')
{}

server::protocol::http::incorrect_protocol::incorrect_protocol(const std::string &protocol_name):
	server::protocol::http::protocol_error("Incorrect protocol: \"" + protocol_name + '\"')
{}

server::protocol::http::unsupported_protocol_version::unsupported_protocol_version(const std::string &version):
	server::protocol::http::protocol_error("Unsupported protocol version: \"" + version + '\"')
{}


// Headers
server::protocol::http::incorrect_header_string::incorrect_header_string(const std::string &str):
	server::protocol::http::protocol_error("Incorrect header string: \"" + str + '\"')
{}


server::protocol::http::empty_header_string::empty_header_string():
	server::protocol::http::protocol_error("Empty header string")
{}


server::protocol::http::header_required::header_required(const std::string &header_name):
	server::protocol::http::protocol_error("Header required: \"" + header_name + '\"')
{}


server::protocol::http::incorrect_host_header::incorrect_host_header(const std::string &host_str):
	server::protocol::http::protocol_error("Incorrect Host header: \"" + host_str + '\"')
{}


server::protocol::http::incorrect_port::incorrect_port(const std::string &port_str):
	server::protocol::http::protocol_error("Incorrect port: \"" + port_str + '\"')
{}
