// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/client_manager.h>

#include <iostream>
#include <functional>

#include <server/worker.h>
#include <server/protocol_exceptions.h>
#include <server/host_exceptions.h>


server::client_manager::client_manager(logger::logger &logger,
									   worker &w,
									   const_iterator_t iterator,
									   socket_ptr_t socket_ptr):
	logger_(logger),
	
	worker_(w),
	iterator_(iterator),
	
	socket_ptr_(socket_ptr)
{
	// Getting client ip address
	try {
		auto endpoint = this->socket_ptr_->remote_endpoint();
		auto address = endpoint.address();
		this->client_ip_address_ = std::move(address.to_string());
	} catch (const boost::system::system_error &e) {
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): Unknown error with client: " << e.what() << '.';
		this->finish_work();
		return;
	}
	
	
	this->logger_.stream(logger::level::info)
		<< "Client manager (worker " << this->worker_.id()
		<< "): Client connected: " << this->client_ip_address() << '.';
	
	
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
void
server::client_manager::log_error(const char *what, const server::http::status &status)
{
	this->logger_.stream(logger::level::error)
		<< "Client manager (worker " << this->worker_.id()
		<< "): " << this->client_ip_address() << ": " << what
		<< " => " << status.code() << '.';
}


void
server::client_manager::handle_error(const char *what,
									 const server::http::status &status,
									 bool exit,
									 bool send_phony,
									 std::unordered_map<std::string, std::string> &&headers)
{
	this->log_error(what, status);
	
	if (exit) this->keep_alive(false);
	if (send_phony) this->send_phony(status, std::move(headers));
}


void
server::client_manager::add_request_handler()
{
	using namespace std::placeholders;
	
	boost::asio::async_read_until(*this->socket_ptr_,
								  this->cache_.headers_buf,
								  "\r\n\r\n",
								  std::bind(&client_manager::request_handler, this, _1, _2));
	
	this->logger_.stream(logger::level::info)
		<< "Client manager (worker " << this->worker_.id()
		<< "): " << this->client_ip_address()
		<< ": Waiting for headers...";
}


void
server::client_manager::send_response(server::host::send_buffers_t &&buffers)
{
	using namespace std::placeholders;
	
	this->socket_ptr_->async_send(buffers,
								  std::bind(&client_manager::response_handler, this, _1, _2));
}


void
server::client_manager::send_phony(const server::http::status &status,
								   std::unordered_map<std::string, std::string> &&headers)
{
	this->cache_.headers = std::move(headers);
	auto buffers = std::move(server::host::error_host(this->logger_).phony_page(
		this->connection_params_.version,
		status,
		this->cache_.host,
		this->cache_.headers));
	this->send_response(std::move(buffers));
}


void
server::client_manager::parse_headers()
{
	std::istream headers_stream(&this->cache_.headers_buf);
	std::string str;
	
	
	// Processing start string
	std::getline(headers_stream, str);
	
	{
		auto start_data = std::move(server::parse_start_string(str));
		
		this->connection_params_.method = start_data.method;
		this->connection_params_.version = start_data.version;
		this->connection_params_.uri = std::move(start_data.uri);
	}
	
	
	// Processing headers
	while (std::getline(headers_stream, str) && !str.empty()) {
		try {
			this->connection_params_.headers.insert(std::move(server::parse_header_string(str)));
		} catch (const server::empty_header_string &)
		{}	// It's normal for the last string
	}
	
	
	// Setting up keep-alive
	try {
		static const std::regex keep_alive_regex(".*keep-alive.*");
		
		this->keep_alive(false);
		
		if (regex_match(this->connection_params_.headers.at("Connection"), keep_alive_regex))
			this->keep_alive(true);
	} catch (...) {}
}


// private
// Closes the socket and removes manager from worker
void
server::client_manager::finish_work() noexcept
{
	this->worker_.erase_client(this->iterator_);
}


void
server::client_manager::request_handler(const boost::system::error_code &err,
										size_t bytes_transferred)
{
	if (err) {
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_ip_address()
			<< ": " << err.message() << '.';
		
		this->finish_work();
		return;
	}
	
	
	try {
		this->parse_headers();
		
		
		{
			auto stream = this->logger_.stream(logger::level::info);
			stream
				<< "Client manager (worker " << this->worker_.id()
				<< "): " << this->client_ip_address()
				<< ": HTTP/" << server::http::version_to_str(this->connection_params_.version)
				<< ", " << server::http::method_to_str(this->connection_params_.method)
				<< ", Requested URI: \"" << this->connection_params_.uri << "\". Headers:";
			
			for (const auto &p: this->connection_params_.headers)
				stream << " [" << p.first << ": " << p.second << ']';
			stream << '.';
		}
		
		
		// ok
		this->send_phony(server::http::status::ok);
	} catch (const server::unimplemented_method &e) {
		// not_implemented
		this->handle_error(
			e, server::http::status::not_implemented,
			true, true);
	} catch (const server::unsupported_protocol_version &e) {
		// http_version_not_supported
		this->handle_error(
			e, server::http::status::http_version_not_supported,
			true, true);
	} catch (const server::incorrect_header_string &e) {
		// unrecoverable_error
		this->handle_error(
			e, server::http::status::unrecoverable_error,
			true, true);
	} catch (const server::protocol_error &e) {
		// Catches incorrect_protocol and incorrect_start_string too
		
		// unrecoverable_error
		this->handle_error(
			e, server::http::status::unrecoverable_error,
			true, true);
	} catch (const server::host_error &e) {
		// internal_server_error
		this->handle_error(
			e, server::http::status::internal_server_error,
			true, true);
	} catch (...) {
		// internal_server_error
		this->handle_error(
			"Unknown error", server::http::status::internal_server_error,
			true, true);
	}
}


void
server::client_manager::response_handler(const boost::system::error_code &err,
										 size_t bytes_transferred)
{
	this->cache_.host.clear();
	
	bool need_finish = true;
	if (this->keep_alive()) {
		this->add_request_handler();
		need_finish = false;
	}
	
	
	if (err) {
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_ip_address()
			<< ": Error sending response: " << err.message() << '.';
	} else {
		this->logger_.stream(logger::level::info)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_ip_address()
			<< ": Response sent.";
	}
	
	
	if (need_finish) this->finish_work();
}
