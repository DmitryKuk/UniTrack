// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/logic_exceptions.h>


logic::logic_error::logic_error(const std::string &what_arg):
	std::logic_error("Logic: " + what_arg)
{}


logic::parameters_init_error::parameters_init_error(const std::string &description):
	logic::logic_error("Can\'t initialize global instance parameters: " + description)
{}


logic::global_instance_init_error::global_instance_init_error(const std::string &description):
	logic::logic_error("Can\'t initialize global instance: " + description)
{}


logic::mongodb_incorrect_uri::mongodb_incorrect_uri(const std::string &description):
	logic::logic_error("Incorrect MongoDB URI: " + description)
{}


logic::mongodb_connection_error::mongodb_connection_error(const std::string &description):
	logic::logic_error("Can\'t connect to MongoDB: " + description)
{}
