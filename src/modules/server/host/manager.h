// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_MANAGER_H
#define SERVER_HOST_MANAGER_H

#include <unordered_map>

#include <logger/logger.h>
#include <server/host/base.h>
#include <server/types.h>


namespace server {


class manager:
	protected logger::enable_logger
{
public:
	manager(logger::logger &logger);
	
	
	// Adds the new host.
	// Returns true, if host successfully addes, or false, if host with the
	// same name is already managed by this manager.
	// Thread-safety: no (it's write function).
	bool add_host(server::host::base::ptr_type host_ptr) noexcept;
	
	
	// Finds the host with specified name.
	// Returns std::shared_ptr to it or throws server::host_not_found exception.
	// Thread-sefety: yes (it's read function).
	server::host::base::ptr_type host(const std::string &name, server::port_type port);
	
	
	// Returns reference to error host object, creating it, if does not exist.
	// If it didn't exist, it will be binded to logger of host_manager object, whoose method was called.
	server::host::base & error_host() const;
private:
	class equal_to: public std::equal_to<std::string>
	{
	public:
		inline bool operator()(const std::string *a, const std::string *b) const
		{
			return std::equal_to<std::string>::operator()(*a, *b);
		}
	};	// class equal_to
	
	std::unordered_map<const std::string *, server::host::base::ptr_type, equal_to> hosts_;
};	// class manager


};	// namespace server


#endif	// SERVER_HOST_MANAGER_H
