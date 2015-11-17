// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host/manager.h>

#include <server/host_exceptions.h>


server::host::manager::manager(logger::logger &logger):
	logger::enable_logger(logger)
{}


// Adds the new host.
// Returns true, if host successfully addes, or false, if host with the
// same name is already managed by this manager.
// Thread-safety: no (it's write function).
bool
server::host::manager::add_host(server::host::base::ptr_type host_ptr) noexcept
{
	if (host_ptr == nullptr) {
		this->logger().stream(logger::level::error)
			<< "Host manager: A nonexistent host can not be added: Are you kidding?";
		return false;
	}
	
	auto p = this->hosts_.emplace(&host_ptr->name(), host_ptr);
	if (p.second) {
		this->logger().stream(logger::level::info)
			<< "Host manager: Host \"" << host_ptr->name() << "\" added.";
	} else {
		this->logger().stream(logger::level::error)
			<< "Host manager: Host \"" << host_ptr->name() << "\" not added: Host already exists.";
	}
	
	return p.second;
}


// Finds the host with specified name.
// Returns std::shared_ptr to it or throws server::host_not_found exception.
// Thread-sefety: yes (it's read function).
server::host::base::ptr_type
server::host::manager::host(const std::string &name, server::port_type port)
{
	auto it = this->hosts_.find(&name);
	if (it == this->hosts_.end()) {
		this->logger().stream(logger::level::sec_info)
			<< "Host manager: Requested access to nonexistent host: \"" << name << "\", port: " << port << '.';
		
		throw server::host_not_found(name);
	}
	
	auto host_ptr = it->second;
	if (!host_ptr->port_allowed(port)) {
		this->logger().stream(logger::level::sec_warning)
			<< "Host manager: Requested access to non-allowed port: host: \"" << name << "\", port: " << port << '.';
		
		throw server::host_not_found(name);
	}
	
	return host_ptr;
}


// Returns reference to error host object, creating it, if does not exist.
// If it didn't exist, it will be binded to logger of host_manager object, whoose method was called.
server::host::base & error_host() const
{
	server::host::base::error_host(this->logger());
}
