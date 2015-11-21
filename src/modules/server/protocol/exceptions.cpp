// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol/exceptions.h>


server::protocol::error::error(const std::string &what_arg):
	std::logic_error(what_arg)
{}
