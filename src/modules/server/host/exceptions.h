// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_EXCEPTIONS_H
#define SERVER_HOST_EXCEPTIONS_H

#include <string>
#include <stdexcept>


namespace server {
namespace host {


class error: public std::logic_error
{
public:
	explicit error(const std::string &what_arg);
};


class headers_has_content_length: public error
{
public:
	explicit headers_has_content_length();
};


class duplicate_header: public error
{
public:
	explicit duplicate_header(const std::string &name);
};


class host_not_found: public error
{
public:
	explicit host_not_found(const std::string &name);
};


class denied_uri: public error
{
public:
	explicit denied_uri(const std::string &uri);
};


class method_not_allowed: public error
{
public:
	explicit method_not_allowed(const std::string &method_name);
};


class path_forbidden: public error
{
public:
	explicit path_forbidden(const std::string &path);
};


class path_not_found: public error
{
public:
	explicit path_not_found(const std::string &path);
};


class incorrect_uri: public error
{
public:
	explicit incorrect_uri(const std::string &uri);
};


};	// namespace host
};	// namespace server


#endif	// SERVER_HOST_EXCEPTIONS_H
