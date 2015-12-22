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


// Usage:
// 1. Construct:			response r{status, version};
// 2. Add headers: repeat:	r.add_header(name, value);
// 3. Add body:    repeat:	r.add_body(data);
// After these steps you have response ready to send. You may skip step 2, if you need.
// NOTE: After any action response object is valid for sending. So, you can call add_header and add_body in any order.

class response:
	public ::server::protocol::response,
	public base::strings_cache
{
public:
	inline response(const ::server::protocol::http::status &status,
					::server::protocol::http::version version = ::server::protocol::http::version::v_1_1);
	
	
	// Non-copy/move constructible/assignable
	response(const response &other) = delete;
	response(response &&other) = delete;
	
	response & operator=(const response &other) = delete;
	response & operator=(response &&other) = delete;
	
	
	// Helper functions
	template<class String1, class String2>
	inline void add_header(String1 &&name, String2 &&value);
	
	
	inline void add_body(const ::base::send_buffer_type &buffer);
protected:
	using base::strings_cache::operator();	// Allow only cache() method
private:
	inline void prepare_update_headers_buffer() noexcept;
	inline void update_headers_buffer() noexcept;
	
	// Data
	std::string headers_;
};	// class response


};	// namespace http
};	// namespace protocol
};	// namespace server


#include <server/protocol/http/response.hpp>

#endif	// SERVER_PROTOCOL_HTTP_RESPONSE_H
