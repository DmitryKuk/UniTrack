// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_RESPONSE_H
#define SERVER_PROTOCOL_RESPONSE_H

#include <base/buffer.h>
#include <base/strings_cache.h>


namespace server {
namespace protocol {


// Base response class. Contains buffers to send and strings cache.
class response:
	public base::strings_cache
{
public:
	base::send_buffers_t buffers;
	
	
	response() = default;
	
	
	// Non-copy/move constructable/assignable (because of strings cache)
	response(const response &other) = delete;
	response(response &&other) = delete;
	
	response & operator=(const response &other) = delete;
	response & operator=(response &&other) = delete;
};	// class response


};	// namespace protocol
};	// namespace server


#include <server/protocol/response.hpp>

#endif	// SERVER_PROTOCOL_RESPONSE_H
