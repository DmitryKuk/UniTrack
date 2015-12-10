// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_RESPONSE_H
#define SERVER_PROTOCOL_RESPONSE_H

#include <base/buffer.h>


namespace server {
namespace protocol {


// Base response class. Contains buffers to send and strings cache.
class response
{
public:
	// Constructs empty response.
	response() = default;
	
	// Constructs buffers_ with given size. Don't forget to fill!
	inline response(size_t buffers_size);
	
	
	inline const base::send_buffers_type & buffers() const noexcept;
protected:
	inline base::send_buffers_type & buffers() noexcept;
private:
	// Buffers to send
	base::send_buffers_type buffers_;
};	// class response


};	// namespace protocol
};	// namespace server


#include <server/protocol/response.hpp>

#endif	// SERVER_PROTOCOL_RESPONSE_H
