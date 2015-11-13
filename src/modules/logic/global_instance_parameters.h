// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_GLOBAL_INSTANCE_PARAMETERS_H
#define LOGIC_GLOBAL_INSTANCE_PARAMETERS_H

#include <string>

#include <json.hpp>

#include <base/json_utils.h>


namespace logic {


struct mongo_parameters
{
	std::string uri;
	
	
	explicit mongo_parameters() = default;
	explicit mongo_parameters(const nlohmann::json &config);
	
	mongo_parameters(const mongo_parameters &other) = default;
	mongo_parameters(mongo_parameters &&other) = default;
	
	mongo_parameters & operator=(const mongo_parameters &other) = default;
	mongo_parameters & operator=(mongo_parameters &&other) = default;
};	// struct mongo_parameters


struct global_instance_parameters
{
	mongo_parameters mongo;
	
	
	explicit global_instance_parameters() = default;
	explicit global_instance_parameters(const nlohmann::json &config);
	
	global_instance_parameters(const global_instance_parameters &other) = default;
	global_instance_parameters(global_instance_parameters &&other) = default;
	
	global_instance_parameters & operator=(const global_instance_parameters &other) = default;
	global_instance_parameters & operator=(global_instance_parameters &&other) = default;
};	// struct global_instance_parameters


};	// logic


#endif	// LOGIC_GLOBAL_INSTANCE_PARAMETERS_H
