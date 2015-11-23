// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_CLIENT_MANAGER_H
#define SERVER_CLIENT_MANAGER_H

#include <string>
#include <memory>
#include <list>
#include <queue>
#include <unordered_map>
#include <mutex>

#include <boost/asio/ip/address.hpp>

#include <logger/logger.h>
#include <server/types.h>
#include <server/protocol/http.h>
#include <server/host/base.h>
#include <server/host/manager.h>


namespace server {


class worker;


class client_manager:
	private std::enable_shared_from_this<client_manager>,
	protected logger::enable_logger
{
public:
	// Use this method for creating client_manager objects. You should NOT manage objects of this class -- they do it.
	static void run(worker &worker, server::socket &&socket);
	
	
	~client_manager();
private:
	client_manager(worker &w, server::socket &&socket);
	
	
	// Non-copy/-move constructable/assignable. Why? Because of implicit async execution flow (see below).
	client_manager(const client_manager &other) = delete;
	client_manager(client_manager &&other) = delete;
	
	client_manager & operator=(const client_manager &other) = delete;
	client_manager & operator=(client_manager &&other) = delete;
	
	
	// Client and server connection info
	inline const boost::asio::ip::address &client_address() const noexcept;
	inline server::port_type server_port() const noexcept;
	
	
	// Helpers
	void handle_error(const server::protocol::http::request &request,
					  const char *what,
					  const server::protocol::http::status &status);
	
	inline
	void handle_error(const server::protocol::http::request &request,
					  const std::exception &e,
					  const server::protocol::http::status &status);
	
	
	void send_response(std::unique_ptr<server::protocol::http::response> &&response_ptr);
	
	void send_phony(const server::protocol::http::request &request,
					const server::protocol::http::status &status);
	
	
	void log_request(const server::protocol::http::request &request);
	
	
	void process_request(const server::protocol::http::request &request) noexcept;
	
	
	// WARNING: Implicit async execution flow!
	// NOTE: client_manager object is alive, while exists std::shared_ptr to it. Methods below implicitly save
	// those pointers, while operations go.
	
	// Request
	static void add_request_handler(std::shared_ptr<client_manager> this_);
	
	static void request_handler(std::shared_ptr<client_manager> &&this_,
								std::unique_ptr<server::protocol::http::request> &&request_ptr,
								const boost::system::error_code &err,
								size_t bytes_transferred);
	
	
	// Response
	static void add_response_handler(std::shared_ptr<client_manager> this_, bool sending = true);
	
	static void response_handler(std::shared_ptr<client_manager> &&this_,
								 const boost::system::error_code &err,
								 size_t bytes_transferred);
	
	
	// Data
	worker &worker_;
	
	// Connection data
	server::socket				socket_;
	boost::asio::ip::address	client_address_;
	server::port_type			server_port_;
	
	
	std::queue<std::unique_ptr<server::protocol::http::response>> responses_queue_;
};	// class client_manager


};	// namespace server


#include <server/client_manager.hpp>

#endif	// SERVER_CLIENT_MANAGER_H
