// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/client_manager.h>

#include <iostream>
#include <functional>
#include <tuple>
#include <regex>
#include <stdexcept>

#include <server/worker.h>
#include <server/protocol/http/exceptions.h>
#include <server/host/exceptions.h>


server::client_manager::client_manager(logger::logger &logger,
									   server::worker &w,
									   const_iterator_t iterator,
									   server::socket_ptr_type socket_ptr,
									   server::host::manager &host_manager):
	logger::enable_logger(logger),
	
	host_manager_(host_manager),
	
	worker_(w),
	iterator_(iterator),
	
	running_operations_(0),
	
	socket_ptr_(socket_ptr),
	client_ip_address_(socket_ptr_->remote_endpoint().address().to_string()),
	server_port_(socket_ptr_->local_endpoint().port()),
	keep_alive_(false),
	
	sending_(false)
{
	unique_lock_t lock(*this);
	
	
	this->logger().stream(logger::level::info)
		<< "Client manager (worker " << this->worker_.id()
		<< "): Client connected: " << this->client_address() << '.';
	
	
	// Starting http-headers reading
	this->add_request_handler();
}


server::client_manager::~client_manager()
{
	// Closing the socket
	boost::system::error_code err;
	this->socket_ptr_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
	this->socket_ptr_->close();
}


// protected
// Mutex-like functions (for usage with std::unique_lock), but provides reference counting
// Tells current object does not destroy itself
void
server::client_manager::lock() noexcept
{
	++this->running_operations_;
}


// Removes manager from worker, if no works running
void
server::client_manager::unlock() noexcept
{
	--this->running_operations_;
	
	if (this->running_operations_ <= 0)
		this->worker_.erase_client(this->iterator_);
}


void
server::client_manager::handle_error(server::protocol::http::request::ptr_type request_ptr,
									 const char *what,
									 const server::protocol::http::status &status)
{
	this->logger().stream(logger::level::error)
		<< "Client manager (worker " << this->worker_.id()
		<< "): " << request_ptr->client_address() << ": " << what
		<< " => " << status.code() << '.';
	
	this->keep_alive(false);
	this->send_phony(std::move(request_ptr), status);
}


void
server::client_manager::add_request_handler()
{
	this->lock();
	
	auto request_ptr = std::make_shared<server::protocol::http::request>(this->client_address());
	
	using namespace std::placeholders;
	boost::asio::async_read_until(*this->socket_ptr_,
								  &request_ptr->buffer,
								  "\r\n\r\n",
								  std::bind(&client_manager::request_handler, this, request_ptr, _1, _2));
	
	this->logger().stream(logger::level::info)
		<< "Client manager (worker " << this->worker_.id()
		<< "): " << this->client_address()
		<< ": Waiting for headers...";
}


void
server::client_manager::send_response(server::protocol::response::ptr_type response_ptr)
{
	this->responses_queue_.push(std::move(response_ptr));
	this->add_response_handler();
}


void
server::client_manager::send_phony(server::protocol::http::request::ptr_type request_ptr,
								   const server::protocol::http::status &status)
{
	// Getting data for the phony...
	auto data = server::host::error_host().phony_response(request_ptr, status);
	
	// ...and sending it
	this->send_response(std::move(data));
}


void
log_request(const server::protocol::http::request &request)
{
	auto stream = this->logger().stream(logger::level::info);
	stream
		<< "Client manager (worker " << this->worker_.id()
		<< "): " << request.client_address
		<< ": HTTP/" << server::protocol::http::version_to_str(request.version)
		<< ", " << server::protocol::http::method_to_str(request.method)
		<< ", Requested URI: \"" << request.uri << "\". Headers:";
	
	for (const auto &p: request.headers)
		stream << " [" << p.first << ": " << p.second << ']';
	stream << '.';
}


// private
// Helpers
void
server::client_manager::process_request(server::protocol::http::request::ptr_type request_ptr) noexcept
{
	using server::protocol::http::status;
	
	
	try {
		request_ptr->process_buffer();
		this->log_request(*request_ptr);
		
		std::string host;
		server::port_type port;
		
		try {
			std::tie(host, port) = request_ptr->host_and_port();
			
			if (port != this->server_port())
				throw server::protocol::http::incorrect_port(std::to_string(port));
		} catch (const std::exception &e) {
			this->logger().stream(logger::level::sec_warning)
				<< "Client manager (worker " << this->worker_.id()
				<< "): " << this->client_address()
				<< ": " << e.what() << '.';
			throw;
		}
		
		if (request_ptr->keep_alive)
			this->add_request_handler();
		
		auto host_ptr = this->host_manager_.host(host, port);
		this->send_response(host_ptr->response(this->worker_, request_ptr));
	}
	
	// Protocol errors
	catch (const server::protocol::http::unimplemented_method &e) {
		this->handle_error(std::move(request_ptr), e,				 not_implemented);
	}
	catch (const server::protocol::http::unsupported_protocol_version &e) {
		this->handle_error(std::move(request_ptr), e,				 http_version_not_supported);
	}
	catch (const server::protocol::http::error &e) {
		// Also catches incorrect_protocol, incorrect_start_string, incorrect_header_string,
		// header_required, incorrect_host_header, incorrect_port
		this->handle_error(std::move(request_ptr), e,				 bad_request);
	}
	
	// Host errors
	catch (const server::host::host_not_found &e) {
		this->handle_error(std::move(request_ptr), e,				 not_found);
	}
	catch (const server::host::error &e) {
		// Also catches headers_has_content_length, duplicate_header
		this->handle_error(std::move(request_ptr), e,				 internal_server_error);
	}
	
	// Other errors
	catch (const std::exception &e) {
		this->handle_error(std::move(request_ptr), e,				 internal_server_error);
	}
	catch (...) {
		// Impossible (maybe, logger?..)
		this->handle_error(std::move(request_ptr), "Unknown error", internal_server_error);
	}
}


// Handlers
void
server::client_manager::request_handler(server::protocol::http::request::ptr_type request_ptr,
										const boost::system::error_code &err,
										size_t bytes_transferred)
{
	unique_lock_t lock(*this);
	this->unlock();
	
	// Normal
	if (!err) {
		this->process_request(std::move(request_ptr));
		return;
	}
	
	// Error
	if (err == boost::asio::error::misc_errors::eof) {
		this->logger().stream(logger::level::info)
			<< "Client manager (worker " << this->worker_.id()
			<< "): Client disconnected: " << request_ptr->client_address() << '.';
	} else {
		this->logger().stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << request_ptr->client_address()
			<< ": " << err.value() << " " << err.message() << '.';
	}
}


void
server::client_manager::add_response_handler()
{
	if (this->responses_queue_.empty()) return;
	
	if (!this->sending_) {
		this->sending_ = true;
		this->lock();
		
		auto response_ptr = std::move(this->responses_queue_.front());
		this->responses_queue_.pop();
		
		using namespace std::placeholders;
		this->socket_ptr_->async_send(response_ptr->buffers,
									  std::bind(&client_manager::response_handler, this, response_ptr, _1, _2));
	}
}


void
server::client_manager::response_handler(server::protocol::response::ptr_type response_ptr,
										 const boost::system::error_code &err,
										 size_t bytes_transferred)
{
	unique_lock_t lock(*this);
	this->unlock();
	this->sending_ = false;
	
	this->add_response_handler();
	
	
	if (err) {
		this->logger().stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_address()
			<< ": Error sending response: " << err.message() << '.';
	} else {
		this->logger().stream(logger::level::info)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_address()
			<< ": Response sent.";
	}
}
