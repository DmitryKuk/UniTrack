// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_MANAGER_H
#define SERVER_HOST_MANAGER_H

#include <unordered_map>
#include <memory>

#include <server/types.h>
#include <server/host/base.h>


namespace server {


class worker;


namespace host {


class manager
{
public:
	// Adds the new host.
	// Throws host_exists (if host with duplicate name found)
	// or host_not_exists (if host_ptr == nullptr).
	// Thread-safety: no (it's write function).
	void add_host(std::shared_ptr<::server::host::base> host_ptr);
	
	
	// Finds the host with specified name.
	// Returns std::shared_ptr to it or throws server::host_not_found exception.
	// Uses worker's logger for security messages.
	// Thread-sefety: yes (it's read function).
	std::shared_ptr<const ::server::host::base> host(const worker &worker,
													 const std::string &name,
													 ::server::port_type port);
	
	
	// Returns reference to error host object
	inline const ::server::host::base & error_host() const noexcept;
private:
	class equal_to:
		private std::equal_to<std::string>
	{
	public:
		inline bool operator()(const std::string *a, const std::string *b) const
		{
			return std::equal_to<std::string>::operator()(*a, *b);
		}
	};	// class equal_to
	
	class hash:
		private std::hash<std::string>
	{
	public:
		inline bool operator()(const std::string *s) const
		{
			return std::hash<std::string>::operator()(*s);
		}
	};	// class hash
	
	
	std::unordered_map<const std::string *, std::shared_ptr<::server::host::base>, hash, equal_to> hosts_;
	
	
	// Error host with default parameters
	::server::host::base error_host_;
};	// class manager


};	// namespace host
};	// namespace server


#include <server/host/manager.hpp>

#endif	// SERVER_HOST_MANAGER_H
