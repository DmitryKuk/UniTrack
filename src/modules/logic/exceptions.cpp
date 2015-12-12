// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/exceptions.h>

using namespace std::literals;


logic::error::error(const std::string &what_arg):
	std::logic_error("Logic: "s + what_arg)
{}


logic::global_instance_init_error::global_instance_init_error(const std::string &description):
	logic::error{"Can\'t initialize global instance: "s + description}
{}


logic::mongodb_incorrect_uri::mongodb_incorrect_uri(const std::string &description):
	logic::error{"Incorrect MongoDB URI: "s + description}
{}


logic::mongodb_connection_error::mongodb_connection_error(const std::string &description):
	logic::error{"Can\'t connect to MongoDB: "s + description}
{}


logic::cant_create_cursor::cant_create_cursor():
	logic::error{"Can\'t create mongo cursor"s}
{}


logic::login_not_found::login_not_found(const std::string &user_login, const std::string &user_password):
	logic::error{"No BSON objects found for login: \""s + user_login + "\" and password: \""s + user_password + '\"'}
{}


logic::password_not_match::password_not_match(const std::string &user_login, const std::string &user_password):
	logic::error{"Password not match for login: \""s + user_login + "\" and password: \""s + user_password + '\"'}
{}


logic::session_not_found::session_not_found(const std::string &session_id):
	logic::error{"Session not found: \""s + session_id + '\"'}
{}


logic::incorrect_form::incorrect_form(const std::string &description):
	logic::error{"Incorrect form: " + description}
{}
