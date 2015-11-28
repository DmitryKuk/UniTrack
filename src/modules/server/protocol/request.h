// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_REQUEST_H
#define SERVER_PROTOCOL_REQUEST_H

#include <boost/asio/ip/address.hpp>


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
	
	
	inline request(const boost::asio::ip::address &client_address);
	
	request() = default;
	request(request &&other) = default;
	request & operator=(request &&other) = default;
	
	request(const request &other) = delete;
	request & operator=(const request &other) = delete;
	
	
	// Prepares request for new data from same client
	// Resets buffer and keep_alive, saving client_address
	inline void reset() noexcept;
};	// class request


};	// namespace protocol
};	// namespace server


#include <server/protocol/request.hpp>

#endif	// SERVER_PROTOCOL_REQUEST_H
