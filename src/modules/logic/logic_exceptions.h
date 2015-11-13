// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_LOGIC_EXCEPTIONS_H
#define LOGIC_LOGIC_EXCEPTIONS_H

#include <string>
#include <stdexcept>


namespace logic {


class logic_error: public std::logic_error
{
public:
	explicit logic_error(const std::string &what_arg);
};


class parameters_init_error: public logic_error
{
public:
	explicit parameters_init_error(const std::string &description);
};


class global_instance_init_error: public logic_error
{
public:
	explicit global_instance_init_error(const std::string &description);
};


class mongodb_incorrect_uri: public logic_error
{
public:
	explicit mongodb_incorrect_uri(const std::string &description);
};


class mongodb_connection_error: public logic_error
{
public:
	explicit mongodb_connection_error(const std::string &description);
};


};	// namespace logic


#endif	// LOGIC_LOGIC_EXCEPTIONS_H
