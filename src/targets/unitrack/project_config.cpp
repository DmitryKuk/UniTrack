// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <project_config.h>

#include <string>


namespace {

const std::string
	server_json_file	= "server.json",
	logic_json_file		= "logic.json";

};	// namespace


const boost::filesystem::path
	project_config::dir		= PATH_CONFIG,
	project_config::server	= project_config::dir / server_json_file,
	project_config::logic	= project_config::dir / logic_json_file;
