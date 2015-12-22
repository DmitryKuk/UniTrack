// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_HTTP_H
#define SERVER_PROTOCOL_HTTP_H

#include <string>

#include <server/types.h>


namespace server {
namespace protocol {
namespace http {


extern const ::server::port_type
	default_port;			// 80



namespace str {


extern const std::string
	default_port,			// "80"
	
	space,					// " "
	newline,				// "\r\n"
	HTTP,					// "HTTP"
	slash,					// "/"
	header_separator;		// ": "

extern const std::string
	header_body_separator;	// "\r\n\r\n"


};	// namespace str



namespace header {


extern const std::string
	content_length,
	server,
	allow,
	connection,
	host,
	cookie,
	set_cookie,
	location;


};	// namespace header



enum class method
{
	unknown	= 0,
	
	GET		= 1,
	HEAD	= 2,
	POST	= 3,
	DELETE	= 4
};	// enum class method

const std::string & method_to_str(method method_) noexcept;
method str_to_method(const std::string &str) noexcept;



enum class version
{
	unknown	= 0,
	
	v_1_1	= 1	// HTTP/1.1
};	// enum class version

const std::string & version_to_str(version version_) noexcept;
version str_to_version(const std::string &str) noexcept;


};	// namespace http
};	// namespace protocol
};	// namespace server


#include <server/protocol/http/status.h>
#include <server/protocol/http/utils.h>
#include <server/protocol/http/request.h>
#include <server/protocol/http/response.h>

#endif	// SERVER_PROTOCOL_HTTP_H
