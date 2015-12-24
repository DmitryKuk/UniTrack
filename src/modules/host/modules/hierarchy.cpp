// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/hierarchy.h>

#include <tuple>
#include <stdexcept>

#include <base/json_utils.h>
#include <server/worker.h>
#include <host/generator.h>
#include <host/module.h>
#include <host/path_utils.h>

using namespace std::literals;
using namespace std::placeholders;


namespace {


host::module<host::hierarchy> module{
	"hierarchy"s,
	[](const nlohmann::json &host_config, ::logic::global_instance &logic)
	{
		return std::make_shared<host::hierarchy>(host_config, logic);
	}
};


};	// namespace



host::hierarchy::hierarchy(const nlohmann::json &config,
						   ::logic::global_instance &logic):
	server::host::base{config},
	
	logic_{logic}
{
	// Host hierarchy
	{
		std::unordered_map<std::string, nlohmann::json> content;
		try {
			::base::json_utils::extract(config, content, "content"s);
		} catch (const std::logic_error &) {
			throw server::host::incorrect_config{"Incorrect type of: \"content\" value (in hierarchy host config), "
												 "it should be map: section name -> host config"s};
		}
		
		// Adding hosts
		for (auto &section: content) {
			section.second["name"] = this->name_;
			section.second["ports"] = this->ports_;
			
			this->hosts_.emplace(section.first, host::generate_host(section.second, logic));
		}
	}
	
	
	// Default host
	{
		nlohmann::json default_host_config;
		try {
			default_host_config = ::base::json_utils::at(config, "default"s);
		} catch (const std::out_of_range &) {
			return;
		}
		
		default_host_config["name"] = this->name_;
		default_host_config["ports"] = this->ports_;
		this->default_host_ = host::generate_host(default_host_config, this->logic_);
	}
}


// virtual
std::unique_ptr<server::protocol::http::response>
host::hierarchy::response(const server::worker &worker,
						  server::protocol::http::request &request) const
{
	std::string first_section, trailing_path;
	try {
		std::tie(first_section, trailing_path) = host::split_first_section(request.path);
	} catch (const std::logic_error &) {	// Incorrect path
		throw server::host::path_not_found{request.path};
	}
	
	
	// Searching for requested section
	auto handler_host_it = this->hosts_.find(first_section);
	if (handler_host_it == this->hosts_.end()) {
		if (this->default_host_ != nullptr)
			return this->default_host_->response(worker, request);
		
		throw server::host::host_not_found{first_section};
	}
	
	
	// Generating correct path for handler host
	if (trailing_path.empty() || trailing_path[0] != '/') {
		request.path = '/';
		request.path += trailing_path;
	} else {
		request.path = std::move(trailing_path);
	}
	
	
	// Returning response from handler host
	return handler_host_it->second->response(worker, request);
}


// virtual
void
host::hierarchy::clear_cache() noexcept
{
	for (auto &host: this->hosts_)
		host.second->clear_cache();
	this->default_host_->clear_cache();
	
	this->server::host::base::clear_cache();
}
