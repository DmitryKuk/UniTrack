// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include <memory>
#include <random>

#include <boost/asio/io_service.hpp>

#include <logger/logger.h>
#include <system_/process.h>
#include <server/types.h>


namespace server {


class server;
class acceptor;


namespace host {


class manager;


};	// namespace host


class worker:
	public logger::enable_logger
{
public:
	// Creates and runs new worker in separate process
	// Returns correct child process object (only in parent process!)
	// Throws: std::system_error, if it's impossible to create process (only in parent process!)
	// NEVER returns in child process!
	static system_::process run(logger::logger &logger, server &server);
	
	
	// Returns server name (random!)
	const std::string & server_name() const noexcept;
	
	// Returns server's host manager
	inline ::server::host::manager & host_manager() noexcept;
	
	// Returns thiw worker's io_service
	inline boost::asio::io_service & io_service() noexcept;
	
	// Adds new client to the worker
	void add_client(::server::socket &&socket) noexcept;
private:
	// Constructor
	worker(logger::logger &logger, server &server);
	
	
	// Returns exit status for current process
	int run() noexcept;
	
	
	// Data
	server &server_;
	
	
	boost::asio::io_service io_service_;
	boost::asio::io_service::work empty_work_;
	std::vector<::server::acceptor> acceptors_;
	
	
	mutable std::minstd_rand0 server_name_generator_;
};	// class worker


};	// namespace server


#include <server/worker.hpp>

#endif	// SERVER_WORKER_H
