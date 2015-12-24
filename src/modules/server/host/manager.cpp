// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host/manager.h>

#include <server/worker.h>
#include <server/host/exceptions.h>


// Adds the new host.
// Throws host_exists (if host with duplicate name found)
// or host_not_exists (if host_ptr == nullptr).
// Thread-safety: no (it's write function).
void
server::host::manager::add_host(std::shared_ptr<::server::host::base> host_ptr)
{
	if (host_ptr == nullptr)
		throw ::server::host::host_not_exists{};
	
	auto inserted = this->hosts_.emplace(&host_ptr->name(), host_ptr).second;
	if (!inserted)
		throw ::server::host::host_exists{host_ptr->name()};
}


// Finds the host with specified name.
// Returns std::shared_ptr to it or throws server::host_not_found exception.
// Uses worker's logger for security messages.
// Thread-sefety: yes (it's read function).
std::shared_ptr<const ::server::host::base>
server::host::manager::host(const ::server::worker &worker,
							const std::string &name,
							::server::port_type port)
{
	using namespace std::literals;
	
	
	auto it = this->hosts_.find(&name);
	if (it == this->hosts_.end()) {
		logger::stream(logger::level::sec_info)
			<< "Host manager: Requested access to nonexistent host: \""s << name << "\", port: "s << port << '.';
		
		throw ::server::host::host_not_found{name};
	}
	
	
	auto &host_ptr = it->second;
	if (!host_ptr->port_allowed(port)) {
		logger::stream(logger::level::sec_warning)
			<< "Host manager: Requested access to non-allowed port: host: \""s << name << "\", port: "s << port << '.';
		
		throw ::server::host::port_not_allowed{name, port};
	}
	
	
	return host_ptr;
}


// Tells all hosts to clear cached data.
void
server::host::manager::clear_cache() noexcept
{
	for (auto &host: this->hosts_)
		host.second->clear_cache();
	this->error_host_.clear_cache();
}
