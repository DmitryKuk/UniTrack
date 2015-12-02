// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_EXCEPTIONS_H
#define LOGGER_EXCEPTIONS_H

#include <stdexcept>
#include <string>


namespace logger {


class error: public std::logic_error
{
public:
	explicit error(const std::string &what_arg);
};


class incorrect_config: public error
{
public:
	explicit incorrect_config(const std::string &what_arg);
};


class cant_create_fifo: public error
{
public:
	explicit cant_create_fifo(const std::string &what_arg);
};


class cant_open_fifo: public error
{
public:
	explicit cant_open_fifo(const std::string &what_arg);
};


};	// namespace logger


#endif	// LOGGER_EXCEPTIONS_H
