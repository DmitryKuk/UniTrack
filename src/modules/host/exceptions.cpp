// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/exceptions.h>


host::incorrect_config::incorrect_config(const std::string &message):
	server::host::error("Incorrect config: " + message)
{}
