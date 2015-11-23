// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_REQUEST_H
#define SERVER_PROTOCOL_REQUEST_H

#include <boost/asio/ip/address.hpp>

#include <base/buffer.h>


namespace server {
namespace protocol {


// Base request class. Contents protocol-independent info about client and connection.
class request
{
public:
	// Client info
	boost::asio::ip::address	client_address;
	
	// Connection info
	bool						keep_alive = false;
	
	// Request buffer
	base::streambuf				buffer;
	
	
	inline request(const boost::asio::ip::address &client_address);
	
	request() = default;
	request(const request &other) = default;
	request(request &&other) = default;
	
	request & operator=(const request &other) = default;
	request & operator=(request &&other) = default;
};	// class request


};	// namespace protocol
};	// namespace server


#include <server/protocol/request.hpp>

#endif	// SERVER_PROTOCOL_REQUEST_H
