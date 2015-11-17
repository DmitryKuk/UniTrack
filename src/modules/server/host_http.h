// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_HTTP_H
#define SERVER_HOST_HTTP_H

#include <string>
#include <deque>
#include <memory>
#include <random>

#include <logger/logger.h>
#include <base/buffer.h>

#include <server/protocol/http.h>
#include <server/host_http_parameters.h>


namespace server {


// Server knows about all existing hosts. If client needs host, that does exist,
// server dispatches request to the error_host.
class host_http:
	protected logger::enable_logger
{
public:
	typedef std::shared_ptr<host_http> ptr_t;
	
	
	host_http(logger::logger &logger,
		 const server::host_http_parameters &parameters);
	
	virtual ~host_http() = default;
	
	
	// Returns host's name (name does not include port information!).
	inline const std::string & name() const noexcept;
	
	
	// Returns true, if host_http can process requests on specified port, or false otherwise.
	bool port_allowed(server::port_t port) const noexcept;
	
	
	// Returns server name as string (random!).
	const std::string & server_name() const noexcept;
	
	
	// Prepares a correct response to the client.
	// NOTE: By default -- phony "404 Not Found". Redefine this function in child classes.
	virtual
	std::unique_ptr<server::protocol::http::response>
	response(std::unique_ptr<server::protocol::http::request> &&request);
	
	
	// Prepares a phony response to the client.
	// WARNING: Remember to save anywhere status too (standard statuses are already saved)!
	std::unique_ptr<server::protocol::http::response>
	phony_response(std::unique_ptr<server::protocol::http::request> &&request,
				   const server::protocol::http::status &status);
	
	
	// Returns reference to error host_http object, creating it, if does not exist.
	// If it didn't exist, it will be binded to logger of object's, whoose method was called.
	host_http & error_host() const;
	
	// Returns reference to error host_http object, creating it, if does not exist.
	// If it didn't exist, it will be binded to logger.
	static host_http & error_host(logger::logger &logger);
	
	// Creates error_host if it does not exist. You may call it once from server, if you want.
	static void create_error_host(logger::logger &logger);
protected:
	server::host_http_parameters host_http_parameters_;
private:
	static std::unique_ptr<host_http> error_host_;
	
	mutable std::minstd_rand0 server_name_generator_;
};	// class host_http


};	// namespace server


#include <server/host_http.hpp>

#endif	// SERVER_HOST_HTTP_H
