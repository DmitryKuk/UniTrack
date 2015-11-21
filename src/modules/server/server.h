// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_SERVER_HTTP_H
#define SERVER_SERVER_HTTP_H

#include <thread>
#include <memory>
#include <vector>
#include <unordered_map>

#include <logger/logger.h>
#include <server/acceptor.h>
#include <server/worker.h>
#include <server/host_manager.h>
#include <server/types.h>


namespace server {


class server:
	protected logger::enable_logger
{
public:
	struct parameters
	{
		server::port_set_type		ports			= {};
		unsigned int				workers			= 1;
		
		
		std::vector<std::string>	server_names	=	// Optional
			{
#				include <server/default_server_names.hpp>
			};
		
		
		explicit parameters() = default;
		explicit parameters(const nlohmann::json &config);
	};	// struct parameters
	
	
	
	server(logger::logger &logger,
		   const parameters &parameters);
	
	
	// Non-copy/-move constructable/assignable. Use ptrs.
	server(const server &other) = delete;
	server(server &&other) = delete;
	
	server & operator=(const server &other) = delete;
	server & operator=(server &&other) = delete;
	
	
	// Stops the server (waiting for server thread and workers threads)
	void stop() noexcept;
	
	
	inline bool joinable() const noexcept;	// Checks server's thread for joinable
	inline void join();						// Joins server's thread
	inline void detach();					// Detaches server's thread
	
	// Returns the hosts manager of this server
	inline server::host_manager & host_manager() noexcept;
	
	// Returns server names
	inline const std::vector<std::string> & names() const noexcept;
protected:
	friend class server::acceptor;
	
	// Dispatches client pointed by socket_ptr to one of worker threads
	// Only acceptor calls this!
	void dispatch_client(server::socket_ptr_t socket_ptr) noexcept;
private:
	// Dispatches client to one of workers
	// WARNING: this method calls from one of workers' threads, NOT from server thread!
	// Only dispatch_client() calls this!
	void dispatch_client_worker_thread(server::socket_ptr_t socket_ptr) noexcept;
	
	
	void run() noexcept;
	
	
	// Data
	parameters parameters_;
	
	server::host_manager host_manager_;
	
	
	boost::asio::io_service acceptors_io_service_;	// Only for acceptors (server runs it in separate thread)!
	std::vector<std::unique_ptr<server::acceptor>> acceptor_ptrs_;
	boost::asio::io_service::work acceptor_empty_work_;	// If no acceptors required
	
	
	boost::asio::io_service workers_io_service_;	// Only for workers (each worker runs it in separate thread)!
	std::vector<std::unique_ptr<server::worker>> worker_ptrs_;
	std::unordered_map<std::thread::id, worker_id_t> workers_dispatch_table_;
	
	
	std::thread server_thread_;
};	// class server


};	// namespace server


#include <server/server.hpp>

#endif // SERVER_SERVER_HTTP_H
