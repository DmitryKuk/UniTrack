// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/manager_generator.h>

#include <host/utils.h>


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
