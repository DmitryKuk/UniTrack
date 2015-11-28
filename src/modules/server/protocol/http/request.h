// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_HTTP_REQUEST_H
#define SERVER_PROTOCOL_HTTP_REQUEST_H

#include <istream>
#include <string>
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
	public ::server::protocol::request
{
public:
	// Data
	// Protocol info
	::server::protocol::http::method					method  = ::server::protocol::http::method::unknown;
	::server::protocol::http::version					version = ::server::protocol::http::version::unknown;
	
	// Non-parsed URI
	std::string											uri;
	
	// Parsed URI info
	std::string											path;
	::server::protocol::http::uri_arguments_map_type	args_map;
	::server::protocol::http::uri_arguments_set_type	args_set;
	
	// Headers
	::server::protocol::http::headers_map_type			headers;
	
	
	// Constructor with client address
	using ::server::protocol::request::request;
	
	request() = default;
	request(request &&other) = default;
	request & operator=(request &&other) = default;
	
	request(const request &other) = delete;
	request & operator=(const request &other) = delete;
	
	
	// Returns requested host and port pair. Used default port (80), if it was omitted
	std::pair<std::string, ::server::port_type> host_and_port();
	
	
	// Prepares request for new data from same client
	// Resets buffer and keep_alive, saving client_address
	void reset() noexcept;
	
	
	void process_stream(std::istream &stream);
private:
	// Helper functions
	void process_start_string(const std::string &str);
	void process_header_string(const std::string &str);
	void process_uri();
};	// class request


// Same as request.process_stream(stream)
inline std::istream & operator>>(std::istream &stream, request &request);


};	// namespace http
};	// namespace protocol
};	// namespace server


#include <server/protocol/http/request.hpp>

#endif	// SERVER_PROTOCOL_HTTP_REQUEST_H
