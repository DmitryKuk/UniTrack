// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <iostream>
#include <vector>
#include <unordered_map>
#include <regex>

#include <templatizer/page.h>

#include <logger/logger.h>
#include <page_model.h>
#include <interface_manager.h>

#include <project_data.h>

#include <logic/global_instance.h>
#include <base/json_utils.h>


// #define LOGGER_JSON_FILE "logger.json"
#define SERVER_JSON_FILE "server.json"
#define LOGIC_JSON_FILE  "logic.json"


int main(int argc, char **argv)
{
	std::ios::sync_with_stdio(false);
	
	
	// mongo::mongoc_handler handler;
	
	// mongo::client client("mongodb://localhost:27017/");
	// mongo::collection collection = client.collection("test", "test");
	
	// bson_error_t error;
	
	// mongo::oid oid;
	// mongo::document doc;
	// doc.append()/*("_id", oid)*/("hello", "world");
	
	// std::cout << "Document: " << doc.json() << std::endl;
	
	// if (!collection.insert(doc, &error))
	// 	std::cerr << error.message << std::endl;
	
	// templatizer::page page("www/profile.html");
	
	
	logger::logger logger(std::clog, true);
	
	
	// Profile page
	// templatizer::page profile_page("www/profile.html");
	
	// page_model profile_model;
	// profile_model.emplace("USERNAME", "Dmitry Kukovinets");
	// profile_model.emplace("UNIVERSITY", "STANKIN");
	// profile_model.emplace("DEPARTMENT", "Inteh");
	// profile_model.emplace("TAGS", "Student");
	
	// std::cout << std::endl << "Generated page:" << std::endl
	// 		  << profile_page(profile_model) << std::endl;
	
	// interface_manager interface_manager(logger,
	// 									project_data::config / SERVER_JSON_FILE,
	// 									profile_model);
	
	// // Waiting for Ctrl+D
	// while (std::cin) std::cin.get();
	
	// interface_manager.stop();
	
	
	try {
		logic::global_instance logic(
			logger,
			logic::global_instance_parameters(
				base::json_utils::json_from_file(project_data::config / LOGIC_JSON_FILE)
			)
		);
	} catch (const std::exception &e) {
		logger.stream(logger::level::critical) << e.what() << '.';
	}
	
	
	// server::host_cache cache;
	// server::host::parse_uri("/index.html?param1=va1&param2=val2?param3&par4=&par5=&par6", cache);
	
	// std::cout << "Keys:" << std::endl;
	// for (const auto &key: cache.args_set)
	// 	std::cout << "    " << key << std::endl;
	
	// std::cout << "Pairs:" << std::endl;
	// for (const auto &p: cache.args_map)
	// 	std::cout << "    " << p.first << " = " << p.second << std::endl;
	
	return 0;
}
