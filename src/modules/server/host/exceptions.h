// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_EXCEPTIONS_H
#define SERVER_HOST_EXCEPTIONS_H

#include <string>

#include <server/exceptions.h>
#include <server/types.h>


namespace server {
namespace host {


class error: public ::server::error
{
public:
	explicit error(const std::string &what_arg);
};


class incorrect_config: public error
{
public:
	explicit incorrect_config(const std::string &what_arg);
};


class host_exists: public error
{
public:
	explicit host_exists(const std::string &name);
};


class host_not_exists: public error
{
public:
	explicit host_not_exists();
};


class host_not_found: public error
{
public:
	explicit host_not_found(const std::string &name);
};


class host_manager_not_constructed: public error
{
public:
	explicit host_manager_not_constructed();
};


class port_not_allowed: public error
{
public:
	explicit port_not_allowed(const std::string &host, ::server::port_type port);
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
	explicit path_forbidden(const std::string &path, const std::string &message);
};


class path_not_found: public error
{
public:
	explicit path_not_found(const std::string &path);
};


class path_is_directory: public error
{
public:
	explicit path_is_directory(const std::string &path);
};


class incorrect_uri: public error
{
public:
	explicit incorrect_uri(const std::string &uri);
};


};	// namespace host
};	// namespace server


#endif	// SERVER_HOST_EXCEPTIONS_H
