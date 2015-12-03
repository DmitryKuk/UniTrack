// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <string>
#include <vector>

#include <json.hpp>

#include <system_/process.h>
#include <server/types.h>
#include <server/host/manager.h>


namespace server {


class worker;


class server
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
	
	
	
	template<class HMGen>
	server(const parameters &parameters,
		   HMGen &&hm_gen);	// Host manager generator
	
	
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
