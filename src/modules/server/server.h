// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <functional>
#include <thread>
#include <memory>
#include <vector>
#include <unordered_map>

#include <json.hpp>

#include <logger/logger.h>
#include <system_/process.h>
#include <server/types.h>
#include <server/host/manager.h>


namespace server {


class worker;


class server:
	protected logger::enable_logger
{
public:
	struct parameters
	{
		::server::port_set_type		ports			= {};
		unsigned int				workers			= 1;
		
		
		std::vector<std::string>	server_names	=	// Optional
			{
#				include <server/default_server_names.hpp>
			};
		
		
		explicit parameters() = default;
		explicit parameters(const nlohmann::json &config);
	};	// struct parameters
	
	
	
	server(logger::logger &logger,
		   ::server::host::manager &host_manager,
		   const parameters &parameters);
	
	
	// Non-copy/-move constructable/assignable. Use ptrs.
	server(const server &other) = delete;
	server(server &&other) = delete;
	
	server & operator=(const server &other) = delete;
	server & operator=(server &&other) = delete;
	
	
	// Stops the server (waiting for server thread and workers threads)
	void stop() noexcept;
	
	
	// inline bool joinable() const noexcept;	// Checks server's thread for joinable
	// inline void join();						// Joins server's thread
	// inline void detach();					// Detaches server's thread
	
	// Returns the hosts manager of this server
	inline ::server::host::manager & host_manager() noexcept;
	inline const ::server::host::manager & host_manager() const noexcept;
	
	// Returns server names
	inline const std::vector<std::string> & names() const noexcept;
protected:
	friend class worker;
	
	// Data
	parameters parameters_;
private:
	// Data
	::server::host::manager &host_manager_;
	
	std::vector<system_::process> worker_processes_;
};	// class server


};	// namespace server


#include <server/server.hpp>

#endif	// SERVER_SERVER_H
