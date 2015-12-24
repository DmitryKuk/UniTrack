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
	
	
	// Non-copy/-move constructible/assignable. Use ptrs.
	base(const base &other) = delete;
	base(base &&other) = delete;
	
	base & operator=(const base &other) = delete;
	base & operator=(base &&other) = delete;
	
	
	// Returns host's name (name does not include port information!).
	inline const std::string & name() const noexcept;
	
	
	// Returns true, if host can process requests on specified port, or false otherwise.
	bool port_allowed(::server::port_type port) const noexcept;
	
	
	// Prepares a correct response.
	// NOTE: By default -- phony "404 Not Found". Redefine this function in child classes.
	// Should return non-nullptr for sending response.
	// If nullptr was returned, server will process request again.
	virtual
	std::unique_ptr<::server::protocol::http::response>
	response(const worker &worker,
			 ::server::protocol::http::request &request) const;
	
	
	// Prepares a phony response.
	std::unique_ptr<::server::protocol::http::response>
	phony_response(const worker &worker,
				   const ::server::protocol::http::request &request,
				   const ::server::protocol::http::status &status) const;
	
	
	// Prepares a response with body.
	template<class String>
	std::unique_ptr<::server::protocol::http::response>
	response_with_body(const worker &worker,
					   const ::server::protocol::http::request &request,
					   const ::server::protocol::http::status &status,
					   String &&response_body,
					   bool need_cache = true,
					   const std::string &content_type = "text/html") const;
	
	
	// Prepares a redirection response.
	std::unique_ptr<::server::protocol::http::response>
	redirect_response(const worker &worker,
					  ::server::protocol::http::request &request,
					  const std::string &location,
					  const ::server::protocol::http::status &status
						  = ::server::protocol::http::status::see_other) const;
	
	
	// Error logging
	template<class WhatArgs>
	inline void log_error(const ::server::worker &worker,
						  const ::server::protocol::http::request &request,
						  const WhatArgs &what) const;
	
	
	template<class WhatArgs>
	inline void log_error(const ::server::worker &worker,
						  const ::server::protocol::http::request &request,
						  const WhatArgs &what,
						  const ::server::protocol::http::status &status) const;	// With status
	
	
	// Does some logging and prepares phony response for client
	// What can be "const std::string &", "const char *" or "const std::exception &"
	std::unique_ptr<::server::protocol::http::response>
	handle_error(const worker &worker,
				 const ::server::protocol::http::request &request,
				 const std::string &what,
				 const ::server::protocol::http::status &status) const;
	
	std::unique_ptr<::server::protocol::http::response>
	handle_error(const worker &worker,
				 const ::server::protocol::http::request &request,
				 const char *what,
				 const ::server::protocol::http::status &status) const;
	
	inline
	std::unique_ptr<::server::protocol::http::response>
	handle_error(const worker &worker,
				 const ::server::protocol::http::request &request,
				 const std::exception &what,
				 const ::server::protocol::http::status &status) const;
	
	
	// Clears cached data. By default does nothing.
	virtual void clear_cache() noexcept;
	
	
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
