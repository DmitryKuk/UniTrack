// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_HTTP_RESPONSE_H
#define SERVER_PROTOCOL_HTTP_RESPONSE_H

#include <string>

#include <base/buffer.h>
#include <base/strings_cache.h>
#include <server/protocol/response.h>


namespace server {
namespace protocol {
namespace http {


class response:
	public server::protocol::response
{
public:
	inline response(const server::protocol::http::status &status,
					server::protocol::http::version version = v_1_1);
	
	response() = default;
	
	
	// Non-copy/move constructable/assignable
	response(const response &other) = delete;
	response(response &&other) = delete;
	
	response & operator=(const response &other) = delete;
	response & operator=(response &&other) = delete;
	
	
	// Helper functions
	// Usage:
	// 1. Construct:			response r{status, version};
	//    or:					response r; r.add_start_string(status, version);
	// 2. Add headers: repeat:	r.add_header(name, value);
	//    or/and:	   repeat:	r.add_headers(std::map<std::string, std::string>{ { "header1", "value1" }, ... });
	// 3. Finish headers:		r.finish_headers();
	// 4. Add body:    repeat:	r.add_body(data);
	// After these steps you have response ready to send. You may skip steps 2 and 4, if you need.
	
	void add_start_string(const server::protocol::http::status &status,
						  server::protocol::http::version version = v_1_1);
	
	void add_header(const std::string &name, const std::string &value);
	
	
	// Headers should be iterable container. Each element should have first and second members,
	// both convertible to std::string. Usage example:
	// response.add_headers
	template<class Headers>
	void add_headers(const Headers &headers);
	
	inline void finish_headers();
	
	
	template<class T>
	inline void add_body(const T &data);
	
	template<>
	inline void add_body(const base::send_buffer_t &buffer);
};	// class response


};	// namespace http
};	// namespace protocol
};	// namespace server


#include <server/protocol/http/response.hpp>

#endif	// SERVER_PROTOCOL_HTTP_RESPONSE_H
