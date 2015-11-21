// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_HTTP_REQUEST_H
#define SERVER_PROTOCOL_HTTP_REQUEST_H

#include <string>
#include <memory>
#include <unordered_map>

#include <boost/asio/ip/address.hpp>

#include <base/buffer.h>
#include <server/protocol/request.h>
#include <server/protocol/http/http.h>


namespace server {
namespace protocol {
namespace http {


// Request data
class request:
	public server::protocol::request
{
public:
	typedef std::shared_ptr<request> ptr_type;
	
	
	// Data
	// Protocol info
	server::protocol::http::method					method  = server::protocol::http::method::unknown;
	server::protocol::http::version					version = server::protocol::http::version::unknown;
	
	// Non-parsed URI
	std::string										uri;
	
	// Parsed URI info
	std::string										path;
	server::protocol::http::uri_arguments_map_type	args_map;
	server::protocol::http::uri_arguments_set_type	args_set;
	
	// Headers
	server::protocol::http::headers_map_type		headers;
	
	
	// Constructor with client address
	using server::protocol::request::request;
	
	request() = default;
	request(const request &other) = default;
	request(request &&other) = default;
	
	request & operator=(const request &other) = default;
	request & operator=(request &&other) = default;
	
	
	// Call this method, when all client response saved in this->buffer
	void process_buffer();
	
	// Returns requested host and port pair. Used default port (80), if it was omitted
	std::pair<std::string, server::port_type> host_and_port();
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
