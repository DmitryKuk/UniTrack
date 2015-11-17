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
	protected logger::enable_logger
{
public:
	typedef std::shared_ptr<client_manager> ptr_type;
	typedef std::list<ptr_type> list_type;
	typedef list_type::const_iterator const_iterator_type;
	
	
	client_manager(logger::logger &logger,
				   worker &w,
				   const_iterator_type iterator,
				   server::socket_ptr_type socket_ptr,
				   server::host::manager &host_manager);
	~client_manager();
	
	
	// Non-copy/-move constructable/assignable.
	client_manager(const client_manager &other) = delete;
	client_manager(client_manager &&other) = delete;
	
	client_manager & operator=(const client_manager &other) = delete;
	client_manager & operator=(client_manager &&other) = delete;
	
	
	inline const boost::asio::ip::address &client_address() const noexcept;
	inline server::port_type server_port() const noexcept;
	
	inline bool keep_alive() const noexcept;
protected:
	inline void keep_alive(bool status) noexcept;
	
	
	// Mutex-like functions (for usage with std::unique_lock), but provides reference counting
	friend class std::unique_lock<client_manager>;
	typedef std::unique_lock<client_manager> unique_lock_t;
	
	// Tells current object does not destroy itself
	void lock() noexcept;
	
	// Removes manager from worker, if no works running
	void unlock() noexcept;
	
	
	void log_error(const char *what, const server::http::status &status);
	
	
	void handle_error(server::protocol::http::request::ptr_type request_ptr,
					  const char *what,
					  const server::http::status &status,
					  bool exit = true,
					  bool send_phony = true);
	
	inline
	void handle_error(server::protocol::http::request::ptr_type request_ptr,
					  const std::exception &e,
					  const server::http::status &status,
					  bool exit = true,
					  bool send_phony = true);
	
	
	void add_request_handler();
	
	void send_response(server::protocol::response::ptr_type response_ptr);
	
	void send_phony(server::protocol::http::request::ptr_type request_ptr,
					const server::http::status &status);
	
	
	void log_request(const server::protocol::http::request &request);
private:
	// Helpers
	void process_request(server::protocol::http::request::ptr_type request_ptr) noexcept;
	
	
	// Handlers
	void request_handler(server::protocol::http::request::ptr_type request_ptr,
						 const boost::system::error_code &err,
						 size_t bytes_transferred);
	
	
	void add_response_handler();
	
	void response_handler(server::host_cache::shared_ptr_t cache_ptr,
						  const boost::system::error_code &err,
						  size_t bytes_transferred);
	
	
	// Data
	server::host::manager &host_manager_;
	
	worker &worker_;
	const_iterator_type iterator_;
	
	unsigned int running_operations_;
	
	
	// Connection data
	server::socket_ptr_type		socket_ptr_;
	boost::asio::ip::address	client_address_;
	server::port_type			server_port_;
	bool						keep_alive_ = false;
	
	
	std::queue<server::protocol::response::ptr_type> responses_queue_;
	bool sending_;
};	// class client_manager


};	// namespace server


#include <server/client_manager.hpp>

#endif	// SERVER_CLIENT_MANAGER_H
