// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/server_exceptions.h>


server::server_error::server_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


server::server_not_started::server_not_started():
	server::server_error("Server NOT started")
{}
