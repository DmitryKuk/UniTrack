// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <iostream>

#include <logger/logger.h>
#include <interface_manager.h>

#include <project_config.h>

#include <logic/global_instance.h>
#include <base/json_utils.h>


int main(int argc, char **argv)
{
	std::ios::sync_with_stdio(false);
	
	
	// server::host_cache cache;
	// server::host::parse_uri("/index.html?param1=va1&param2=val2?param3&par4=&par5=&par6", cache);
	
	// std::cout << "Keys:" << std::endl;
	// for (const auto &key: cache.args_set)
	// 	std::cout << "    " << key << std::endl;
	
	// std::cout << "Pairs:" << std::endl;
	// for (const auto &p: cache.args_map)
	// 	std::cout << "    " << p.first << " = " << p.second << std::endl;
	
	
	// templatizer::page page("www/profile.html");
	
	
	
	// Profile page
	// templatizer::page profile_page("www/profile.html");
	
	// page_model profile_model;
	// profile_model.emplace("USERNAME", "Dmitry Kukovinets");
	// profile_model.emplace("UNIVERSITY", "STANKIN");
	// profile_model.emplace("DEPARTMENT", "Inteh");
	// profile_model.emplace("TAGS", "Student");
	
	// std::cout << std::endl << "Generated page:" << std::endl
	// 		  << profile_page(profile_model) << std::endl;
	
	
	logger::logger logger(std::clog, true);
	
	try {
		logic::global_instance logic(
			logger,
			logic::global_instance_parameters(
				base::json_utils::json_from_file(project_config::logic)
			)
		);
		
		
		interface_manager interface_manager(logger, logic);
		
		// Waiting for Ctrl+D
		while (std::cin) std::cin.get();
		
		interface_manager.stop();
	} catch (const std::exception &e) {
		logger.stream(logger::level::critical) << e.what() << '.';
		logger.stream(logger::level::critical) << "Critical error occured. Stopping...";
		return 1;
	}
	
	
	return 0;
}
