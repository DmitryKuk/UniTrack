// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MANAGER_GENERATOR_H
#define HOST_MANAGER_GENERATOR_H

#include <memory>

#include <json.hpp>

#include <server/host/manager.h>


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


};	// namespace host


#endif	// HOST_MANAGER_GENERATOR_H
