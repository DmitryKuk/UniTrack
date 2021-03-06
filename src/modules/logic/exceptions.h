// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_EXCEPTIONS_H
#define LOGIC_EXCEPTIONS_H

#include <string>
#include <stdexcept>


namespace logic {


class error: public std::logic_error
{
public:
	explicit error(const std::string &what_arg);
};


class global_instance_init_error: public error
{
public:
	explicit global_instance_init_error(const std::string &description);
};


class mongodb_incorrect_uri: public error
{
public:
	explicit mongodb_incorrect_uri(const std::string &description);
};


class mongodb_connection_error: public error
{
public:
	explicit mongodb_connection_error(const std::string &description);
};


class incorrect_cursor: public error
{
public:
	explicit incorrect_cursor();
};


class user_not_found: public error
{
public:
	explicit user_not_found(const std::string &description);
};


class duplicate_user_found: public error
{
public:
	explicit duplicate_user_found(const std::string &description);
};


class password_not_match: public error
{
public:
	explicit password_not_match(const std::string &user_id, const std::string &user_password);
};


class session_error: public error
{
public:
	explicit session_error(const std::string &session_id, const std::string &err_message);
};


class session_not_found: public session_error
{
public:
	explicit session_not_found(const std::string &session_id);
};


class cant_start_session: public error
{
public:
	explicit cant_start_session(const std::string &user_id);
};


class cant_create_user_ref: public error
{
public:
	explicit cant_create_user_ref(const std::string &user_id);
};


class incorrect_form: public error
{
public:
	explicit incorrect_form(const std::string &description);
};


};	// namespace logic


#endif	// LOGIC_EXCEPTIONS_H
