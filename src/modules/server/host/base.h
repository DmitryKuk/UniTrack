// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_BASE_H
#define SERVER_HOST_BASE_H

#include <string>
#include <memory>

#include <json.hpp>

#include <base/buffer.h>
#include <server/types.h>
#include <server/protocol/http.h>


namespace server {


class worker;


namespace host {


// Server knows about all existing hosts. If client needs host, that does exist,
// server dispatches request to the error_host.
class base
{
public:
	base(const nlohmann::json &config);
	base() = default;
	
	virtual ~base() = default;
	
	
	// Returns host's name (name does not include port information!).
	inline const std::string & name() const noexcept;
	
	
	// Returns true, if host can process requests on specified port, or false otherwise.
	bool port_allowed(::server::port_type port) const noexcept;
	
	
	// Prepares a correct response to the client.
	// NOTE: By default -- phony "404 Not Found". Redefine this function in child classes.
	virtual
	std::unique_ptr<::server::protocol::http::response>
	response(const worker &worker,
			 ::server::protocol::http::request &request) const;
	
	
	// Prepares a phony response to the client.
	// WARNING: Remember to save anywhere status too (standard statuses are already saved)!
	std::unique_ptr<::server::protocol::http::response>
	phony_response(const worker &worker,
				   const ::server::protocol::http::request &request,
				   const ::server::protocol::http::status &status) const;
	
	
	// Gets server name from worker and adds it to headers in response.
	// Returns pointer to used server name string and true, if name was added.
	// Otherwise, returns pointer to empty string and false (if returned server name is empty).
	static
	std::pair<const std::string *, bool>
	add_server_name(const worker &worker, ::server::protocol::http::response &response);
protected:
	// Parameters
	std::string				name_;	// Required
	::server::port_set_type	ports_;	// Required
};	// class base


};	// namespace host
};	// namespace server


#include <server/host/base.hpp>

#endif	// SERVER_HOST_BASE_H
