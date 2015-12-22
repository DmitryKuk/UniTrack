// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <string>
#include <vector>

#include <boost/asio/io_service.hpp>

#include <json.hpp>

#include <system_/process.h>
#include <server/types.h>
#include <server/host/manager.h>


namespace server {


class worker;


class server
{
public:
	template<class HMGen>
	server(const nlohmann::json &config,
		   HMGen &&hm_gen);	// Host manager generator
	
	
	// Non-copy/-move constructible/assignable. Use ptrs.
	server(const server &other) = delete;
	server(server &&other) = delete;
	
	server & operator=(const server &other) = delete;
	server & operator=(server &&other) = delete;
	
	
	// Stops the server
	void stop() noexcept;
	
	
	inline boost::asio::io_service & io_service() noexcept;
	
	
	// Returns server names
	inline const std::vector<std::string> & names() const noexcept;
protected:
	// Parameters
	::server::port_set_type		ports_;				// Optional
	unsigned int				workers_	= 1;	// Optional
	
	std::vector<std::string>	names_		=		// Optional
		{
#			include <server/default_server_names.hpp>
		};
private:
	// Data
	boost::asio::io_service io_service_;
	
	std::vector<system_::process> worker_processes_;
};	// class server


};	// namespace server


#include <server/server.hpp>

#endif	// SERVER_SERVER_H
