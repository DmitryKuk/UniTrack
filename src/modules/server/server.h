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
#include <logger/async_logger.h>
#include <system_/process.h>
#include <server/types.h>
#include <server/host/manager.h>


namespace server {


class worker;


class server:
	protected logger::enable_logger_ref
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
	
	
	
	template<class HMGen, class... HMGenArgs>
	server(logger::logger &logger,
		   const parameters &parameters,
		   const logger::async_logger::parameters &async_logger_parameters,
		   HMGen &&hm_gen, HMGenArgs &&... hm_gen_args);	// Host manager generator with args
	
	
	// Non-copy/-move constructible/assignable. Use ptrs.
	server(const server &other) = delete;
	server(server &&other) = delete;
	
	server & operator=(const server &other) = delete;
	server & operator=(server &&other) = delete;
	
	
	// Stops the server
	void stop() noexcept;
	
	
	// inline bool joinable() const noexcept;	// Checks server's thread for joinable
	// inline void join();						// Joins server's thread
	// inline void detach();					// Detaches server's thread
	
	
	// Returns server names
	inline const std::vector<std::string> & names() const noexcept;
protected:
	friend class worker;
	
	// Data
	parameters parameters_;
private:
	// Data
	std::vector<system_::process> worker_processes_;
};	// class server


};	// namespace server


#include <server/server.hpp>

#endif	// SERVER_SERVER_H
