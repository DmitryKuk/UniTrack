// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/hierarchy.h>

#include <regex>

#include <base/json_utils.h>
#include <server/worker.h>
#include <host/utils.h>

using namespace std::literals;


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
	static const std::regex regex{	// Example: "/user/photo?params=values..."
		"^"
		"/"							//     Leading '/' symbol
		"([^/?]*)"					// [1] Leading section name: "user"
		"([/?].*)?"					// [2] Trailing path: "/photo?params=values..."
		"$"s,
		std::regex::optimize
	};
	
	
	// Parsing requested path
	std::smatch m;
	if (!std::regex_match(request.path, m, regex))
		throw server::host::path_not_found{request.path};
	
	
	// Searching for requested section
	std::string section_name = m[1];
	auto next_host_it = this->hosts_.find(section_name);
	if (next_host_it == this->hosts_.end()) {
		if (this->default_host_ != nullptr)
			return this->default_host_->response(worker, request);
		
		throw server::host::host_not_found{section_name};
	}
	
	
	// Generating correct path for next host
	{
		std::string trailling_path = m[2];
		if (trailling_path.empty() || trailling_path[0] != '/') {
			request.path = '/';
			request.path += trailling_path;
		} else {
			request.path = std::move(trailling_path);
		}
	}
	
	
	// Returning response from next host
	return next_host_it->second->response(worker, request);
}
