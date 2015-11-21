// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_BASE_H
#define SERVER_HOST_BASE_H

#include <string>
#include <vector>
#include <deque>
#include <memory>

#include <json.hpp>

#include <logger/logger.h>
#include <base/buffer.h>
#include <server/protocol/http.h>


namespace server {


class worker;


namespace host {


// Server knows about all existing hosts. If client needs host, that does exist,
// server dispatches request to the error_host.
class base:
	protected logger::enable_logger
{
public:
	typedef std::shared_ptr<base> ptr_type;
	
	
	// Host parameters. Need for every host.
	// This is default parameters for error_host.
	struct parameters
	{
		std::string				name;	// Required
		server::port_set_type	ports;	// Required
		
		
		explicit parameters() = default;
		explicit parameters(const nlohmann::json &config);
	};	// struct parameters
	
	
	
	base(logger::logger &logger,
		 const parameters &parameters);
	
	virtual ~base() = default;
	
	
	// Returns host's name (name does not include port information!).
	inline const std::string & name() const noexcept;
	
	
	// Returns true, if host can process requests on specified port, or false otherwise.
	bool port_allowed(server::port_type port) const noexcept;
	
	
	// Prepares a correct response to the client.
	// NOTE: By default -- phony "404 Not Found". Redefine this function in child classes.
	virtual
	server::protocol::http::response::ptr_type
	response(const server::worker &worker,
			 server::protocol::http::request::ptr_type request_ptr);
	
	
	// Prepares a phony response to the client.
	// WARNING: Remember to save anywhere status too (standard statuses are already saved)!
	server::protocol::http::response::ptr_type
	phony_response(const server::worker &worker,
				   server::protocol::http::request::ptr_type request_ptr,
				   const server::protocol::http::status &status);
	
	
	// Returns reference to error host object, creating it, if does not exist.
	// If it didn't exist, it will be binded to logger of object, whoose method was called.
	base & error_host() const;
	
	// Returns reference to error host object, creating it, if does not exist.
	// If it didn't exist, it will be binded to logger.
	static base & error_host(logger::logger &logger);
	
	// Creates error_host if it does not exist. You may call it once from server, if you want.
	static void create_error_host(logger::logger &logger);
	
	
	// Gets server name from worker and adds it to headers in response.
	// Returns true, if name was added. Otherwise, false (if returned server name is empty).
	static
	bool add_server_name(const server::worker &worker,
						 const server::protocol::http::response &response);
protected:
	parameters parameters_;
private:
	static std::unique_ptr<base> error_host_ptr_;
};	// class base


};	// namespace host
};	// namespace server


#include <server/host/base.hpp>

#endif	// SERVER_HOST_BASE_H
