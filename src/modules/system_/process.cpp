// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <system_/process.h>

#include <system_/exceptions.h>


void
system_::process::kill(int signal)
{
	if (this->id_ == 0)
		throw system_::incorrect_process_id{"Process id == 0"};
	
	switch (::kill(this->id_, signal)) {
		case 0:	// Normal
			return;
		case EINVAL:
			throw system_::invalid_signal{std::to_string(signal)};
		case EPERM:
			throw system_::permission_error{};
		case ESRCH:
			throw system_::process_not_found{};
		default:
			throw system_::unknown_error{};
	}
}
