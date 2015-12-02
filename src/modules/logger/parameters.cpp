// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/parameters.h>

#include <base/json_utils.h>
#include <logger/exceptions.h>


// explicit
logger::parameters::parameters(const nlohmann::json &json)
{
	try {
		this->root = base::json_utils::get<std::string>(json, "root");
	} catch (const std::out_of_range &e) {
		throw ::logger::incorrect_config{"Required key missed: \"root\" in global logger config"};
	}
	
	base::json_utils::extract(json, this->max_log_size, "max_log_size");
	base::json_utils::extract(json, this->colorize_output, "colorize_output");
}
