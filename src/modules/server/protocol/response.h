// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_RESPONSE_H
#define SERVER_PROTOCOL_RESPONSE_H

#include <memory>

#include <base/buffer.h>
#include <base/strings_cache.h>


namespace server {
namespace protocol {


// Base response class. Contains buffers to send and strings cache.
class response:
	public base::strings_cache
{
public:
	typedef std::shared_ptr<response> ptr_type;
	
	
	// Buffers to send
	base::send_buffers_type buffers;
	
	
	response() = default;
	
	
	// Non-copy/move constructable/assignable (because of strings cache)
	response(const response &other) = delete;
	response(response &&other) = delete;
	
	response & operator=(const response &other) = delete;
	response & operator=(response &&other) = delete;
protected:
	using base::strings_cache::operator();	// Allow only cache() method
};	// class response


};	// namespace protocol
};	// namespace server


#endif	// SERVER_PROTOCOL_RESPONSE_H
