// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/logic_exceptions.h>


logic::logic_error::logic_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


logic::global_instance_init_error::global_instance_init_error():
	logic::logic_error("Can\'t initialize logic global instance")
{}


logic::mongodb_connection_error::mongodb_connection_error(const std::string &str):
	logic::logic_error("Can\'t connect to MongoDB: \"" + str + "\"")
{}
