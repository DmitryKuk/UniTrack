// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MANAGER_GENERATOR_H
#define HOST_MANAGER_GENERATOR_H

#include <memory>

#include <json.hpp>

#include <server/host/manager.h>
#include <logic/global_instance.h>


namespace host {


class manager_generator
{
public:
	explicit manager_generator(const nlohmann::json &hosts_config,
							   const nlohmann::json &logic_config);
	
	
	std::unique_ptr<server::host::manager> operator()() const;
private:
	const nlohmann::json &hosts_config_;
	const nlohmann::json &logic_config_;
};	// class manager_generator



std::shared_ptr<server::host::base>
generate_host(const nlohmann::json &host_config,
			  logic::global_instance &logic);


};	// namespace host


#endif	// HOST_MANAGER_GENERATOR_H
