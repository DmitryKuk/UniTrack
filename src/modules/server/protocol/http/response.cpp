// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol/http/response.h>


void
server::protocol::http::response::add_start_string(const server::protocol::http::status &status,
												   server::protocol::http::version version)
{
	using base::buffer;
	using namespace server::protocol::http;
	
	this->buffers.reserve(this->buffers.size() + 8);
	
	this->buffers.insert(
		this->buffers.end(),
		{
			buffer(str::HTTP),
			buffer(str::slash),
			buffer(version_to_str(version)),
			buffer(str::space),
			buffer(status.code_str()),
			buffer(str::space),
			buffer(status.description()),
			buffer(str::newline)
		}
	);
}


void
server::protocol::http::response::add_header(const std::string &name, const std::string &value)
{
	using base::buffer;
	using namespace server::protocol::http;
	
	this->buffers.reserve(this->buffers.size() + 4);
	
	this->buffers.insert(
		this->buffers.end(),
		{
			buffer(name),
			buffer(str::header_separator),
			buffer(value),
			buffer(str::newline)
		}
	);
}
