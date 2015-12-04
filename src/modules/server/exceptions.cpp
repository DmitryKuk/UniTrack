// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/exceptions.h>

using namespace std::literals;


server::error::error(const std::string &what_arg):
	std::logic_error("Server: "s + what_arg)
{}
