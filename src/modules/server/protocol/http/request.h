// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_HTTP_REQUEST_H
#define SERVER_PROTOCOL_HTTP_REQUEST_H

#include <string>
#include <unordered_map>

#include <boost/asio/ip/address.hpp>

#include <base/buffer.h>
#include <server/protocol/request.h>
#include <server/protocol/http/protocol.h>


namespace server {
namespace protocol {
namespace http {


// Request data
class request:
	public server::protocol::request
{
public:
	// Protocol info
	server::protocol::http::method				method  = unknown;
	server::protocol::http::version				version = unknown;
	
	// Non-parsed URI
	std::string									uri;
	
	// Parsed URI info
	std::string									path;
	server::protocol::http::uri_arguments_map_t	args_map;
	server::protocol::http::uri_arguments_set_t	args_set;
	
	// Headers
	server::protocol::http::headers_map_t		headers;
	
	
	request(const boost::asio::ip::address &client_address,
			base::streambuf &&headers_buf);
	
	request() = default;
	request(const request &other) = default;
	request(request &&other) = default;
	
	request & operator=(const request &other) = default;
	request & operator=(request &&other) = default;
private:
	// Helper functions
	void process_start_string(const std::string &str);
	void process_header_string(const std::string &str);
	void process_uri(const std::string &str);
};	// class request


};	// namespace http
};	// namespace protocol
};	// namespace server


#endif	// SERVER_PROTOCOL_HTTP_REQUEST_H
