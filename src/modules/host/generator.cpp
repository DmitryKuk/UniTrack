// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/generator.h>

#include <string>

#include <base/json_utils.h>
#include <host/module.h>
#include <host/exceptions.h>

using namespace std::literals;


namespace {


class host_manager_with_logic:
	public server::host::manager,
	private logic::global_instance
{
public:
	host_manager_with_logic(const nlohmann::json &logic_config):
		logic::global_instance{logic_config}
	{}
	
	
	logic::global_instance & logic() noexcept
	{
		return static_cast<logic::global_instance &>(*this);
	}
};	// class host_manager_with_logic


};	// namespace



host::manager_generator::manager_generator(const nlohmann::json &hosts_config,
										   const nlohmann::json &logic_config):
	hosts_config_{hosts_config},
	logic_config_{logic_config}
{}


std::unique_ptr<server::host::manager>
host::manager_generator::operator()() const
{
	auto host_manager_ptr = std::make_unique<host_manager_with_logic>(this->logic_config_);
	
	for (const auto &host_config: this->hosts_config_) {
		// Generating new host
		auto host_ptr = host::generate_host(host_config, host_manager_ptr->logic());
		
		// And adding it
		host_manager_ptr->add_host(host_ptr);
	}
	
	return std::move(host_manager_ptr);
}



std::shared_ptr<server::host::base>
host::generate_host(const nlohmann::json &host_config,
					::logic::global_instance &logic)
{
	// Getting type of new host
	std::string type;
	if (!base::json_utils::extract(host_config, type, "type"s))
		throw server::host::incorrect_config{"Required key: \"type\" missed"s};
	
	
	// Searching for generator of host of given type
	auto host_module_it = host::default_module_registrar.map().find(type),
		 end = host::default_module_registrar.map().end();
	if (host_module_it != end)
		return (host_module_it->second)(host_config, logic);	// Generating and returning new host
	
	
	// Throwing exception with message
	std::string message = "Unknown host type: \""s + type + '\"';
	
	auto it = host::default_module_registrar.map().begin();
	if (it != end) {
		message += ", correct values are: "s;
		
		for (size_t i = 0, size = host::default_module_registrar.map().size(); it != end; ++it, ++i) {
			message += '\"';
			message += it->first;
			message += '\"';
			
			if (i < size - 1)
				message += ", "s;
		}
	}
	
	throw server::host::incorrect_config{message};
}
