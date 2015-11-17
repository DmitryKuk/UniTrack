// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_HTTP_MANAGER_H
#define SERVER_HOST_HTTP_MANAGER_H

#include <unordered_map>

#include <logger/logger.h>
#include <server/host_http.h>


namespace server {


class host_http_manager:
	protected logger::enable_logger
{
public:
	host_http_manager(logger::logger &logger);
	
	
	// Adds the new host.
	// Returns true, if host successfully addes, or false, if host with the
	// same name is already managed by this manager.
	// Thread-safety: no (it's write function).
	bool add_host(server::host_http::ptr_t host_ptr) noexcept;
	
	
	// Finds the host with specified name.
	// Returns std::shared_ptr to it or throws server::host_not_found exception.
	// Thread-sefety: yes (it's read function).
	server::host_http::ptr_t host(const std::string &name, server::port_t port);
private:
	class hosts_http_equal: public std::equal_to<std::string>
	{
	public:
		inline bool operator()(const std::string *a, const std::string *b) const
		{
			return std::equal_to<std::string>::operator()(*a, *b);
		}
	};	// class hosts_http_equal
	
	std::unordered_map<const std::string *, server::host_http::ptr_t, hosts_http_equal> hosts_;
};	// class host_http_manager


};	// namespace server


#endif	// SERVER_HOST_HTTP_MANAGER_H
