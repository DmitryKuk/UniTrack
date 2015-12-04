// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol/exceptions.h>

using namespace std::literals;


server::protocol::error::error(const std::string &what_arg):
	::server::error{"Protocol: "s + what_arg}
{}
