// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_H
#define SERVER_HOST_H

#include <string>
#include <vector>
#include <list>
#include <unordered_set>
#include <memory>
#include <random>

#include <boost/asio/buffer.hpp>

#include <base/buffer.h>

#include <logger/logger.h>

#include <server/protocol/http.h>
#include <server/types.h>
#include <server/host_parameters.h>


namespace server {


// Server knows about all existing hosts. If client needs host, that does exist,
// server dispatches request to the error_host.
class host:
	protected logger::enable_logger
{
public:
	host(logger::logger &logger,
		 const server::host_parameters &parameters);
	
	virtual ~host() = default;
	
	
	// Returns host's name (name does not include port information!).
	inline const std::string & name() const noexcept;
	
	
	// Returns true, if host can process requests on specified port, or false otherwise.
	bool port_allowed(server::port_t port) const noexcept;
	
	
	// Returns host name as string (random!).
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
	
	
	// Returns reference to error host object, creating it, if does not exist.
	// If it didn't exist, it will be binded to logger of object's, whoose method was called.
	host & error_host() const;
	
	// Returns reference to error host object, creating it, if does not exist.
	// If it didn't exist, it will be binded to logger.
	static host & error_host(logger::logger &logger);
	
	// Creates error_host if it does not exist. You may call it once from server, if you want.
	static void create_error_host(logger::logger &logger);
	
	
	static void validate_headers(const headers_t &headers);
protected:
	server::host_parameters host_parameters_;
private:
	static std::unique_ptr<host> error_host_;
	
	mutable std::minstd_rand0 server_name_generator_;
};	// class host


typedef std::shared_ptr<host> host_ptr_t;


};	// namespace server


#include <server/host.hpp>

#endif // SERVER_HOST_H
