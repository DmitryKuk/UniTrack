// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_UTILS_H
#define HOST_UTILS_H

#include <memory>

#include <json.hpp>

#include <server/host/base.h>
#include <logic/global_instance.h>


namespace host {


std::shared_ptr<server::host::base>
generate_host(const nlohmann::json &host_config,
			  logic::global_instance &logic);


};	// namespace host


#endif	// HOST_UTILS_H
