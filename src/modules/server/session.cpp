// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/session.h>

#include <functional>
#include <utility>
#include <tuple>
#include <regex>
#include <stdexcept>

#include <boost/asio.hpp>

#include <server/worker.h>
#include <server/protocol/http/exceptions.h>
#include <server/host/exceptions.h>

using namespace std::literals;


// static
void
server::session::run(worker &worker, ::server::socket &&socket)
{
	auto this_ = std::make_shared<::server::session>(worker, std::move(socket));
	session::add_request_handler(std::move(this_));	// Starting reading headers
}


server::session::session(worker &worker, ::server::socket &&socket):
	worker_{worker},
	
	socket_{std::move(socket)},
	
	client_address_{this->socket_.remote_endpoint().address()},
	server_port_{this->socket_.local_endpoint().port()},
	
	request_{this->client_address_},
	
	sending_{false},
	
	processing_new_request_{true}
{
	logger::stream(logger::level::info)
		<< "Client: "s << this->client_address() << ": Connected."s;
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
	logger::stream(logger::level::error)
		<< "Client: "s << this->request_.client_address << ": "s << what
		<< " => "s << status.code_str() << ' ' << status.description() << '.';
	
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
	
	
	try {
		// Reading request (with body, if required)
		{
			std::istream stream{&this->stream_buffer_};
			size_t required_body_size = 0;
			
			
			if (this->processing_new_request_)	// New request
				required_body_size = this->request_.process_stream(stream);
			else								// Continue of old request (body required)
				required_body_size = this->request_.process_stream_again(stream);
			
			
			if (required_body_size == 0) {		// All request processed
				this->processing_new_request_ = true;
			} else {							// Continue of old request (body required)
				this->processing_new_request_ = false;
				
				using namespace std::placeholders;
				boost::asio::async_read(this->socket_,
										this->stream_buffer_,
										boost::asio::transfer_at_least(required_body_size),
										std::bind(&session::request_handler, this->shared_from_this(), _1, _2));
				return;
			}
		}
		// After this request is ready for work
		
		
		bool process_request_again = true;
		while (process_request_again) {	// To process again request, if need
			process_request_again = false;
			
			logger::stream(logger::level::info) << this->request_;
			
			
			try {
				if (this->request_.port != this->server_port())
					throw ::server::protocol::http::incorrect_port{std::to_string(this->request_.port)};
			} catch (const ::server::protocol::http::incorrect_port &e) {
				logger::stream(logger::level::sec_warning)
					<< "Client: "s << this->request_.client_address << ": "s << e.what() << '.';
				throw;
			}
			
			
			// NOTE: BEFORE get response and add it to the queue
			auto host_ptr = this->worker_.host_manager().host(this->worker_, this->request_.host, this->request_.port);
			auto response_ptr = host_ptr->response(this->worker_, this->request_);
			if (response_ptr == nullptr) {	// Request was rewrote => process it again
				process_request_again = true;
				continue;
			}
			
			
			// Continue normal processing
			this->send_response(std::move(response_ptr));
			
			// NOTE: AFTER replace old request with the new one
			if (this->request_.keep_alive)
				session::add_request_handler(this->shared_from_this());
			
			// NOTE: Here old request does not exist, it is replaced by new request (if keep-alive enabled)
		}
	}
	
	// Host errors
	catch (const ::server::host::host_not_found &e) {
		this->handle_error(e, status::not_found);
	}
	catch (const ::server::host::port_not_allowed &e) {
		this->handle_error(e, status::not_found);
	}
	catch (const ::server::host::error &e) {
		this->handle_error(e, status::bad_request);
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
	
	// Other errors
	catch (const std::exception &e) {
		this->handle_error(e, status::internal_server_error);
	}
	catch (...) {	// Impossible
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
								 size_t /* bytes_transferred */)
{
	// Normal
	if (!err) {
		this_->process_request();
		return;
	}
	
	// Error
	if (err == boost::asio::error::misc_errors::eof) {
		logger::stream(logger::level::info)
			<< "Client: "s << this_->client_address() << ": Disconnected."s;
	} else {
		logger::stream(logger::level::error)
			<< "Client: "s << this_->client_address() << ": "s << err.message() << '.';
	}
}



// Response
// static
void
server::session::add_response_handler(std::shared_ptr<::server::session> this_)
{
	if (this_->sending_ || this_->responses_queue_.empty())
		return;
	
	this_->sending_ = true;
	
	using namespace std::placeholders;
	boost::asio::async_write(this_->socket_,
							 this_->responses_queue_.front()->buffers(),
							 std::bind(&session::response_handler, this_, _1, _2));
}


// static
void
server::session::response_handler(std::shared_ptr<::server::session> this_,
								  const boost::system::error_code &err,
								  size_t bytes_transferred)
{
	this_->sending_ = false;
	this_->responses_queue_.pop();	// Deleting current (sent) response
	
	
	if (!err) {
		this_->add_response_handler(this_);
		return;
	}
	
	logger::stream(logger::level::error)
		<< "Client: "s << this_->client_address() << ": Error sending response: "s << err.message() << '.';
}
