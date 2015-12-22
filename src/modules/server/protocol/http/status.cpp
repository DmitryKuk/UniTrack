// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol/http/status.h>

#include <server/protocol/http/statuses.hpp>


server::protocol::http::status::status(unsigned int code,
									   const std::string &description,
									   const std::string &start_string) noexcept:
	code_{code},
	code_str_{std::to_string(this->code_)},
	description_{description},
	start_string_{start_string}
{}
