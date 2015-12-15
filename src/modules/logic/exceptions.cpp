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


logic::incorrect_cursor::incorrect_cursor():
	logic::error{"Incorrect mongo cursor"s}
{}


logic::user_not_found::user_not_found(const std::string &description):
	logic::error{"User not found: "s + description}
{}


logic::id_not_found::id_not_found(const std::string &user_id, const std::string &user_password):
	logic::user_not_found{"ID: \""s + user_id + "\"; password: \""s + user_password + '\"'}
{}


logic::login_not_found::login_not_found(const std::string &user_login, const std::string &user_password):
	logic::user_not_found{"Login: \""s + user_login + "\"; password: \""s + user_password + '\"'}
{}


logic::email_not_found::email_not_found(const std::string &user_email, const std::string &user_password):
	logic::user_not_found{"Email: \""s + user_email + "\"; password: \""s + user_password + '\"'}
{}


logic::password_not_match::password_not_match(const std::string &user_id, const std::string &user_password):
	logic::error{"Password not match for id: \""s + user_id + "\" and password: \""s + user_password + '\"'}
{}


logic::session_not_found::session_not_found(const std::string &session_id):
	logic::error{"Session not found: \""s + session_id + '\"'}
{}


logic::session_error::session_error(const std::string &session_id, const std::string &err_message):
	logic::error{"Session error: \""s + session_id + "\": " + err_message}
{}


logic::cant_start_session::cant_start_session(const std::string &user_id):
	logic::error{"Can\'t start session for user id: \""s + user_id + '\"'}
{}


logic::incorrect_form::incorrect_form(const std::string &description):
	logic::error{"Incorrect form: " + description}
{}
