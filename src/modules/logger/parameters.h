// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_PARAMETERS_H
#define LOGGER_PARAMETERS_H

#include <boost/filesystem/path.hpp>

#include <json.hpp>


namespace logger {


struct parameters
{
	boost::filesystem::path root;
	
	size_t max_log_size		= 10000;
	bool colorize_output	= false;
	
	
	explicit parameters(const nlohmann::json &json);
	
	parameters(const parameters &other) = default;
	parameters(parameters &&other) = default;
	
	parameters & operator=(const parameters &other) = default;
	parameters & operator=(parameters &&other) = default;
};	// struct parameters


};	// namespace logger


#endif	// LOGGER_PARAMETERS_H
