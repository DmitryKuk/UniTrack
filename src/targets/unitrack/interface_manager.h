// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef INTERFACE_MANAGER_H
#define INTERFACE_MANAGER_H

#include <string>
#include <memory>

#include <boost/filesystem/path.hpp>

#include <project_config.h>
#include <logger/logger.h>
#include <logic/global_instance.h>
#include <server/server_http.h>


class interface_manager:
	protected logger::enable_logger
{
public:
	interface_manager(
		logger::logger &logger,
		logic::global_instance &logic_global_instance,
		const boost::filesystem::path &server_config_path
			= project_config::server
	);
	
	// Non-copy/move constructable/assignable.
	interface_manager(const interface_manager &other) = delete;
	interface_manager(interface_manager &&other);
	interface_manager & operator=(const interface_manager &other) = delete;
	interface_manager & operator=(interface_manager &&other);
	
	void interactive_run();
	void stop();
private:
	logic::global_instance &logic_global_instance_;
	
	std::unique_ptr<server::server_http> server_ptr_;
};	// class interface_manager


#endif // INTERFACE_MANAGER_H
