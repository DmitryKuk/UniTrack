// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/session.h>

#include <iostream>
#include <functional>
#include <utility>
#include <tuple>
#include <regex>
#include <stdexcept>

#include <boost/asio.hpp>

#include <server/worker.h>
#include <server/protocol/http/exceptions.h>
#include <server/host/exceptions.h>


// static
void
server::session::run(worker &worker, ::server::socket &&socket)
{
	auto this_ = std::make_shared<::server::session>(worker, std::move(socket));
	session::add_request_handler(std::move(this_));	// Starting reading headers
}


server::session::session(worker &worker, ::server::socket &&socket):
	logger::enable_logger{worker.logger()},
	
	worker_{worker},
	
	socket_{std::move(socket)},
	
	client_address_{this->socket_.remote_endpoint().address()},
	server_port_{this->socket_.local_endpoint().port()}
{
	this->logger().stream(logger::level::info)
		<< "Client connected: " << this->client_address() << '.';
}


server::session::~session()
{
	// Closing the socket
	boost::system::error_code err;
	this->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
	this->socket_.close();
}


// private
// Helpers
void
server::session::handle_error(const char *what, const ::server::protocol::http::status &status)
{
	this->logger().stream(logger::level::error)
		<< "Client: " << this->request_.client_address << ": " << what << " => " << status.code() << '.';
	
	this->send_phony(status);
}


void
server::session::send_response(std::unique_ptr<::server::protocol::http::response> &&response_ptr)
{
	this->responses_queue_.push(std::move(response_ptr));
	session::add_response_handler(this->shared_from_this());
}


void
server::session::send_phony(const ::server::protocol::http::status &status)
{
	auto &error_host = this->worker_.host_manager().error_host();
	this->send_response(error_host.phony_response(this->worker_, this->request_, status));
}



// Helpers
// The main function of this class
// Processes current request (it must be already got!) and adds response to the queue
void
server::session::process_request() noexcept
{
	using ::server::protocol::http::status;
	
	
	const auto log_request =
		[&]()
		{
			auto stream = this->logger().stream(logger::level::info);
			stream
				<< "Client: " << this->request_.client_address
				<< ": HTTP/" << ::server::protocol::http::version_to_str(this->request_.version)
				<< ", " << ::server::protocol::http::method_to_str(this->request_.method)
				<< ", Requested URI: \"" << this->request_.uri << "\". Headers:";
			
			for (const auto &p: this->request_.headers)
				stream << " [" << p.first << ": " << p.second << ']';
			stream << '.';
		};
	
	
	try {
		// Preparing and logging current request
		std::istream{&this->stream_buffer_} >> this->request_;
		log_request();
		
		
		std::string host;
		::server::port_type port;
		
		try {
			std::tie(host, port) = this->request_.host_and_port();
			
			if (port != this->server_port())
				throw ::server::protocol::http::incorrect_port{std::to_string(port)};
		} catch (const ::server::protocol::http::incorrect_port &e) {
			this->logger().stream(logger::level::sec_warning)
				<< "Client: " << this->request_.client_address << ": " << e.what() << '.';
			throw;
		}
		
		
		// NOTE: BEFORE get response and add it to the queue
		auto host_ptr = this->worker_.host_manager().host(host, port);
		this->send_response(host_ptr->response(this->worker_, this->request_));
		
		
		// NOTE: AFTER replace old request with the new one
		if (this->request_.keep_alive)
			session::add_request_handler(this->shared_from_this());
		
		// NOTE: Here old request does not exist, it is replaced by new request (if keep-alive enabled)
	}
	
	// Protocol errors
	catch (const ::server::protocol::http::unimplemented_method &e) {
		this->handle_error(e, status::not_implemented);
	}
	catch (const ::server::protocol::http::unsupported_protocol_version &e) {
		this->handle_error(e, status::http_version_not_supported);
	}
	catch (const ::server::protocol::http::error &e) {
		this->handle_error(e, status::bad_request);
	}
	
	// Host errors
	catch (const ::server::host::host_not_found &e) {
		this->handle_error(e, status::not_found);
	}
	catch (const ::server::host::port_not_allowed &e) {
		this->handle_error(e, status::not_found);
	}
	catch (const ::server::host::error &e) {
		this->handle_error(e, status::internal_server_error);
	}
	
	// Other errors
	catch (const std::exception &e) {
		this->handle_error(e, status::internal_server_error);
	}
	catch (...) {	// Impossible (maybe, logger?..)
		static const char e[] = "Unknown error";
		this->handle_error(e, status::internal_server_error);
	}
}



// WARNING: Implicit async execution flow!
// NOTE: session object is alive, while exists std::shared_ptr to it. Methods below implicitly save
// those pointers, while operations go.

// Request
// static
void
server::session::add_request_handler(std::shared_ptr<::server::session> this_)
{
	using namespace std::placeholders;
	boost::asio::async_read_until(this_->socket_,
								  this_->stream_buffer_,
								  ::server::protocol::http::str::header_body_separator,
								  std::bind(&session::request_handler, this_, _1, _2));
}


// static
void
server::session::request_handler(std::shared_ptr<::server::session> this_,
								 const boost::system::error_code &err,
								 size_t bytes_transferred)
{
	// Normal
	if (!err) {
		this_->process_request();
		return;
	}
	
	// Error
	if (err == boost::asio::error::misc_errors::eof) {
		this_->logger().stream(logger::level::info)
			<< "Client: " << this_->client_address() << ": Client disconnected.";
	} else {
		this_->logger().stream(logger::level::error)
			<< "Client: " << this_->client_address() << ": " << err.message() << '.';
	}
}



// Response
// static
void
server::session::add_response_handler(std::shared_ptr<::server::session> this_, bool sending)
{
	if (sending || this_->responses_queue_.empty())
		return;
	
	using namespace std::placeholders;
	this_->socket_.async_send(this_->responses_queue_.front()->buffers,
							  std::bind(&session::response_handler, this_, _1, _2));
}


// static
void
server::session::response_handler(std::shared_ptr<::server::session> this_,
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
