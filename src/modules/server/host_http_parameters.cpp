// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host_http_parameters.h>

#include <base/json_utils.h>


server::host_http_parameters::host_http_parameters(const nlohmann::json &config):
	name(base::json_utils::get<decltype(this->name)>(config, "name")),
	ports(base::json_utils::get<decltype(this->ports)>(config, "ports"))
{
	base::json_utils::extract(config, this->server_names, "server_names");
}
