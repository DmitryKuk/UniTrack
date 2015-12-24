// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include <memory>
#include <random>
#include <unordered_map>

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>

#include <system_/process.h>
#include <server/types.h>


namespace server {


class server;
class acceptor;


namespace host {


class manager;


};	// namespace host


class worker
{
public:
	// Signal, that means clear_cache command for worker.
	static constexpr int clear_cache_signal = SIGUSR1;
	
	
	// Constructor
	// NOTE: Host manager generator should return std::unique_ptr<server::host::manager> to correct
	// and ready-to-response host manager. If it is impossible, please, throw!
	template<class HMGen>
	worker(server &server,
		   std::vector<::server::acceptor> &acceptors,
		   HMGen &&hm_gen);	// Host manager generator
	
	
	// Returns exit status for current process
	int run() noexcept;
	
	
	const std::string & server_name() const noexcept;	// Returns random server name
	inline ::server::host::manager & host_manager() noexcept;
	inline boost::asio::io_service & io_service() noexcept;
	
	// Tells all hosts to clear cached data.
	inline void clear_cache() noexcept;
	
	// Adds new client to the worker
	void add_client(::server::socket &&socket) noexcept;
private:
	// Signal handlers
	void add_signal_handler() noexcept;
	
	// Handles signals and dispatch them to primitive handlers
	void signal_handler(const boost::system::error_code &err, int signal) noexcept;
	
	// Primitive signal handlers
	void exit(int signal, const char *signal_str) noexcept;
	void ignore(int signal, const char *signal_str) noexcept;
	void clear_cache_handler(int signal, const char *signal_str) noexcept;
	
	// Signal handlers map
	static
	const std::unordered_map<int, std::pair<const char *, void (::server::worker::*)(int, const char *)>>
		signal_handlers_;
	
	
	// Data
	server &server_;
	
	int status_ = 0;	// Execution status (returned by run())
	
	std::unique_ptr<::server::host::manager> host_manager_ptr_;
	
	boost::asio::io_service::work	empty_work_;	// Forces io_service do not stop, when no clients in process
	boost::asio::signal_set			signal_set_;	// Need for handle SIGTERM, SIGINT, SIGHUP, SIGQUIT
	std::vector<::server::acceptor>	&acceptors_;
	
	// Used to choose random server name number
	// /dev/urandom used as seed generator
	mutable std::minstd_rand0 server_name_generator_;
};	// class worker


};	// namespace server


#include <server/worker.hpp>

#endif	// SERVER_WORKER_H
