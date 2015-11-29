// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SYSTEM__EXCEPTIONS_H
#define SYSTEM__EXCEPTIONS_H

#include <system_error>
#include <string>


namespace system_ {


class error: public std::system_error
{
public:
	error(const std::string &what_arg);
};


class incorrect_process_id: public error
{
public:
	incorrect_process_id(const std::string &message);
};


class invalid_signal: public error
{
public:
	invalid_signal(const std::string &message);
};


class permission_error: public error
{
public:
	permission_error();
};


class process_not_found: public error
{
public:
	process_not_found();
};


class unknown_error: public error
{
public:
	unknown_error();
};


};	// namespace system_


#endif	// SYSTEM__EXCEPTIONS_H
