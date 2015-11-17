// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/file_host_handler_exceptions.h>


server::file_host_handler_error::file_host_handler_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}
