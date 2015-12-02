// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <istream>
#include <string>
#include <memory>
#include <list>
#include <queue>
#include <unordered_map>
#include <mutex>

#include <boost/asio/ip/address.hpp>
#include <boost/asio/streambuf.hpp>

#include <server/types.h>
#include <server/protocol/http.h>
#include <server/host/base.h>
#include <server/host/manager.h>


namespace server {


class worker;


class session:
	public std::enable_shared_from_this<session>
{
public:
	// Use this method for creating session objects. You should NOT manage objects of this class -- they do it.
	static void run(worker &worker, ::server::socket &&socket);
	
	
	session(worker &w, ::server::socket &&socket);
	~session();
	
	
	// Non-copy/-move constructible/assignable. Why? Because of implicit async execution flow (see below).
	session(const session &other) = delete;
	session(session &&other) = delete;
	
	session & operator=(const session &other) = delete;
	session & operator=(session &&other) = delete;
private:
	// Client and ::server connection info
	inline const boost::asio::ip::address &client_address() const noexcept;
	inline ::server::port_type server_port() const noexcept;
	
	
	// Helpers
	void handle_error(const char *what,
					  const ::server::protocol::http::status &status);
	
	inline
	void handle_error(const std::exception &e,
					  const ::server::protocol::http::status &status);
	
	
	void send_response(std::unique_ptr<::server::protocol::http::response> &&response_ptr);
	
	void send_phony(const ::server::protocol::http::status &status);
	
	
	// The main function of this class
	// Processes current request (it must be already got!) and adds response to the queue
	void process_request() noexcept;
	
	
	// WARNING: Implicit async execution flow!
	// NOTE: session object is alive, while exists std::shared_ptr to it. Methods below implicitly save
	// those pointers, while operations go.
	
	// Request
	static void add_request_handler(std::shared_ptr<session> this_);
	
	static void request_handler(std::shared_ptr<session> this_,
								const boost::system::error_code &err,
								size_t bytes_transferred);
	
	
	// Response
	static void add_response_handler(std::shared_ptr<session> this_, bool sending = true);
	
	static void response_handler(std::shared_ptr<session> this_,
								 const boost::system::error_code &err,
								 size_t bytes_transferred);
	
	
	// Data
	worker &worker_;
	
	// Connection data
	::server::socket					socket_;
	boost::asio::ip::address			client_address_;
	::server::port_type					server_port_;
	
	// Buffered recieved data
	boost::asio::streambuf				stream_buffer_;
	
	// Current client request
	::server::protocol::http::request	request_;
	
	// Responses for sending to client
	// NOTE: responses_queue_.front() points to response, that is sending now
	std::queue<std::unique_ptr<::server::protocol::http::response>> responses_queue_;
};	// class session


};	// namespace server


#include <server/session.hpp>

#endif	// SERVER_SESSION_H
