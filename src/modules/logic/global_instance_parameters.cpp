// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/global_instance_parameters.h>

#include <logic/logic_exceptions.h>


logic::mongo_parameters::mongo_parameters(const nlohmann::json &config)
{
	if (!base::json_utils::extract(config, this->uri, "uri"))
		throw logic::parameters_init_error("Required key: \"uri\" missed (in mongo parameters)");
}


logic::global_instance_parameters::global_instance_parameters(const nlohmann::json &config)
{
	try {
		this->mongo = logic::mongo_parameters(base::json_utils::at(config, "mongo"));
	} catch (const std::out_of_range &) {
		throw logic::parameters_init_error("Required key: \"mongo\" missed");
	}
}
