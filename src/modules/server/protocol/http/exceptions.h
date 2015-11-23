// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_HTTP_EXCEPTIONS_H
#define SERVER_PROTOCOL_HTTP_EXCEPTIONS_H

#include <string>

#include <server/protocol/exceptions.h>


namespace server {
namespace protocol {
namespace http {


class error: public ::server::protocol::error
{
public:
	explicit error(const std::string &what_arg);
};


// Start string
class incorrect_start_string: public error
{
public:
	explicit incorrect_start_string(const std::string &str);
};


class uri_parse_error: public error
{
public:
	explicit uri_parse_error(const std::string &uri);
};


class unimplemented_method: public error
{
public:
	explicit unimplemented_method(const std::string &method_name);
};


class incorrect_protocol: public error
{
public:
	explicit incorrect_protocol(const std::string &protocol_name);
};


class unsupported_protocol_version: public error
{
public:
	explicit unsupported_protocol_version(const std::string &version);
};


// Headers
class incorrect_header_string: public error
{
public:
	explicit incorrect_header_string(const std::string &str);
};


class empty_header_string: public error
{
public:
	explicit empty_header_string();
};


class header_required: public error
{
public:
	explicit header_required(const std::string &header_name);
};


class incorrect_host_header: public error
{
public:
	explicit incorrect_host_header(const std::string &host_str);
};


class incorrect_port: public error
{
public:
	explicit incorrect_port(const std::string &port_str);
};


};	// namespace http
};	// namespace protocol
};	// namespace server


#endif	// SERVER_PROTOCOL_HTTP_EXCEPTIONS_H
