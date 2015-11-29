// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <system_/exceptions.h>

#include <cerrno>
#include <cstring>

using namespace std::literals;


system_::error::error(const std::string &what_arg):
	std::system_error{errno, std::system_category(), what_arg}
{}


system_::incorrect_process_id::incorrect_process_id(const std::string &message):
	system_::error{"Incorrect process id: "s + message}
{}


system_::invalid_signal::invalid_signal(const std::string &message):
	system_::error{"Invalid signal: "s + message + ": "s + std::strerror(errno)}
{}


system_::permission_error::permission_error():
	system_::error{"Permission error: "s + std::strerror(errno)}
{}


system_::process_not_found::process_not_found():
	system_::error{"Process not found: "s + std::strerror(errno)}
{}


system_::unknown_error::unknown_error():
	system_::error{"Unknown error: "s + std::strerror(errno)}
{}
