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


// static
void
::server::client_manager::run(worker &worker, ::server::socket &&socket)
{
	auto this_ = std::make_shared<::server::client_manager>(worker, std::move(socket));
	client_manager::add_request_handler(this_);	// Starting reading headers
}


::server::client_manager::~client_manager()
{
	// Closing the socket
	boost::system::error_code err;
	this->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
	this->socket_.close();
}


// private
::server::client_manager::client_manager(worker &worker, ::server::socket &&socket):
	logger::enable_logger{worker.logger()},
	
	worker_{worker},
	
	socket_{std::move(socket)},
	
	client_address_{this->socket_.remote_endpoint().address()},
	server_port_{this->socket_.local_endpoint().port()}
{
	this->logger().stream(logger::level::info)
		<< "Client connected: " << this->client_address() << '.';
}


// Helpers
void
::server::client_manager::handle_error(const ::server::protocol::http::request &request,
									   const char *what,
									   const ::server::protocol::http::status &status)
{
	this->logger().stream(logger::level::error)
		<< "Client: " << request.client_address << ": " << what << " => " << status.code() << '.';
	
	this->send_phony(request, status);
}


void
::server::client_manager::send_response(std::unique_ptr<::server::protocol::http::response> &&response_ptr)
{
	this->responses_queue_.push(std::move(response_ptr));
	client_manager::add_response_handler(this->shared_from_this());
}


void
::server::client_manager::send_phony(const ::server::protocol::http::request &request,
								   const ::server::protocol::http::status &status)
{
	this->send_response(this->worker_.host_manager().error_host().phony_response(request, status));
}


void
log_request(const ::server::protocol::http::request &request)
{
	auto stream = this->logger().stream(logger::level::info);
	stream
		<< "Client: " << request.client_address
		<< ": HTTP/" << ::server::protocol::http::version_to_str(request.version)
		<< ", " << ::server::protocol::http::method_to_str(request.method)
		<< ", Requested URI: \"" << request.uri << "\". Headers:";
	
	for (const auto &p: request.headers)
		stream << " [" << p.first << ": " << p.second << ']';
	stream << '.';
}


// Helpers
void
::server::client_manager::process_request(const ::server::protocol::http::request &request) noexcept
{
	using ::server::protocol::http::status;
	
	
	try {
		request.process_buffer();
		this->log_request(*request);
		
		std::string host;
		::server::port_type port;
		
		try {
			std::tie(host, port) = request.host_and_port();
			
			if (port != this->server_port())
				throw ::server::protocol::http::incorrect_port(std::to_string(port));
		} catch (const ::server::protocol::http::incorrect_port &e) {
			this->logger().stream(logger::level::sec_warning)
				<< "Client: " << request.client_address << ": " << e.what() << '.';
			throw;
		}
		
		if (request.keep_alive)
			client_manager::add_request_handler(this->shared_from_this());
		
		auto host_ptr = this->host_manager_.host(host, port);
		this->send_response(host_ptr->response(this->worker_, request));
	}
	
	// Protocol errors
	catch (const ::server::protocol::http::unimplemented_method &e) {
		this->handle_error(request, e, not_implemented);
	}
	catch (const ::server::protocol::http::unsupported_protocol_version &e) {
		this->handle_error(request, e, http_version_not_supported);
	}
	catch (const ::server::protocol::http::error &e) {
		this->handle_error(request, e, bad_request);
	}
	
	// Host errors
	catch (const ::server::host::host_not_found &e) {
		this->handle_error(request, e, not_found);
	}
	catch (const ::server::host::port_not_allowed &e) {
		this->handle_error(request, e, not_found);
	}
	catch (const ::server::host::error &e) {
		this->handle_error(request, e, internal_server_error);
	}
	
	// Other errors
	catch (const std::exception &e) {
		this->handle_error(request, e, internal_server_error);
	}
	catch (...) {	// Impossible (maybe, logger?..)
		static const char e[] = "Unknown error";
		this->handle_error(request, e, internal_server_error);
	}
}



// WARNING: Implicit async execution flow!
// NOTE: client_manager object is alive, while exists std::shared_ptr to it. Methods below implicitly save
// those pointers, while operations go.

// Request
// static
void
::server::client_manager::add_request_handler(std::shared_ptr<::server::client_manager> this_)
{
	auto request_ptr = std::make_unique<::server::protocol::http::request>(this->client_address());
	auto &buffer = request_ptr->buffer;
	auto &socket = this_->socket_;
	
	using namespace std::placeholders;
	boost::asio::async_read_until(socket, buffer, "\r\n\r\n",
								  std::bind(&client_manager::request_handler, std::move(this_),
											std::move(request_ptr), _1, _2));
}


// static
void
::server::client_manager::request_handler(std::shared_ptr<::server::client_manager> &&this_,
										std::unique_ptr<::server::protocol::http::request> &&request_ptr,
										const boost::system::error_code &err,
										size_t bytes_transferred)
{
	// Normal
	if (!err) {
		this_->process_request(*request_ptr);
		return;
	}
	
	// Error
	if (err == boost::asio::error::misc_errors::eof) {
		this_->logger().stream(logger::level::info)
			<< "Client: " << request_ptr->client_address << ": Client disconnected.";
	} else {
		this_->logger().stream(logger::level::error)
			<< "Client: " << request_ptr->client_address << ": " << err.message() << '.';
	}
}



// Response
// static
void
::server::client_manager::add_response_handler(std::shared_ptr<::server::client_manager> this_, bool sending)
{
	if (sending || this_->responses_queue_.empty())
		return;
	
	auto &buffers = this_->responses_queue_.front()->buffers;
	auto &socket = this_->socket_;
	
	using namespace std::placeholders;
	socket.async_send(buffers, std::bind(&client_manager::response_handler, std::move(this_), _1, _2));
}


// static
void
::server::client_manager::response_handler(std::shared_ptr<::server::client_manager> &&this_,
										 const boost::system::error_code &err,
										 size_t bytes_transferred)
{
	this_->responses_queue_.pop();	// Deleting current (sent) response
	this_->add_response_handler(this_, false);
	
	
	if (err) {
		this_->logger().stream(logger::level::error)
			<< "Client: " << this_->client_address() << ": Error sending response: " << err.message() << '.';
	} else {
		this_->logger().stream(logger::level::info)
			<< "Client: " << this_->client_address() << ": Response sent.";
	}
}
