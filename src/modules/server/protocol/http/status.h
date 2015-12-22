// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_HTTP_HTTP_H
#define SERVER_PROTOCOL_HTTP_HTTP_H

#include <string>


namespace server {
namespace protocol {
namespace http {


class status
{
public:
	explicit status(unsigned int code,
					const std::string &description,
					const std::string &start_string) noexcept;
	
	
	inline unsigned int code() const noexcept;
	inline const std::string & code_str() const noexcept;
	inline const std::string & description() const noexcept;
	inline const std::string & start_string() const noexcept;
private:
	const unsigned int code_;
	const std::string code_str_, description_, start_string_;
public:
	// Declaration statuses as static class members
	#define SERVER_PROTOCOL_HTTP_CLASS_STATUS
	#include <server/protocol/http/statuses.hpp>
	#undef SERVER_PROTOCOL_HTTP_CLASS_STATUS
};	// class status


};	// namespace http
};	// namespace protocol
};	// namespace server


#include <server/protocol/http/status.hpp>

#endif	// SERVER_PROTOCOL_HTTP_HTTP_H
