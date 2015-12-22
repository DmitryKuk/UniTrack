// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_HTTP_REQUEST_H
#define SERVER_PROTOCOL_HTTP_REQUEST_H

#include <istream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <boost/asio/ip/address.hpp>

#include <base/buffer.h>
#include <server/protocol/request.h>
#include <server/protocol/http.h>


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
	::server::protocol::http::method					method  		= ::server::protocol::http::method::unknown;
	::server::protocol::http::version					version 		= ::server::protocol::http::version::unknown;
	
	// Non-parsed URI
	std::string											uri;
	
	// Parsed URI info
	std::string											path;
	std::unordered_multimap<std::string, std::string>	args_map;
	std::unordered_multiset<std::string>				args_set;
	
	// Headers
	std::string											host;
	::server::port_type									port			= ::server::protocol::http::default_port;
	
	size_t												content_length	= 0;
	
	std::unordered_map<std::string, std::string>		cookies;
	
	std::unordered_multimap<std::string, std::string>	unparsed_headers,
														parsed_headers;
	
	// Body
	std::vector<char>									body;
	
	
	
	// Constructor with client address
	using ::server::protocol::request::request;
	
	request() = default;
	request(request &&other) = default;
	request & operator=(request &&other) = default;
	
	request(const request &other) = delete;
	request & operator=(const request &other) = delete;
	
	
	// Prepares request for new data from same client
	// Resets buffer and keep_alive, saving client_address
	void reset() noexcept;
	
	
	// Fills request data from the stream
	// Returns 0, if no additional data required, otherwise returns number of bytes to read
	size_t process_stream(std::istream &stream);
	
	// Fills request body with bytes_transferred number of bytes
	// Returns 0, if no additional data required, otherwise returns number of bytes to read
	size_t process_stream_again(std::istream &stream);
private:
	// Helper functions
	void process_start_string(const std::string &str);
	bool process_header_string(const std::string &str);
	void process_uri();
	
	
	// Data
	size_t body_bytes_read_ = 0;	// Count of body bytes, that are already read
};	// class request


// Same as request.process_stream(stream)
inline std::istream & operator>>(std::istream &stream, request &request);

std::ostream & operator<<(std::ostream &stream, const ::server::protocol::http::request &request);


};	// namespace http
};	// namespace protocol
};	// namespace server


#include <server/protocol/http/request.hpp>

#endif	// SERVER_PROTOCOL_HTTP_REQUEST_H
