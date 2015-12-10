// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <application.h>

#include <json.hpp>

#include <base/json_utils.h>
#include <logger/logger.h>
#include <server/server.h>
#include <host/generator.h>

using namespace std::literals;


// class application
application::application(int argc, char **argv) noexcept:
	status_{0},
	
	logger_global_instance_{argv[0]}
{
	// std::ios::sync_with_stdio(false);
	
	try {
		// Server and hosts config
		auto config = base::json_utils::json_from_file(project_config::server);
		
		// Server
		this->server_ptr_ = std::make_unique<server::server>(
			base::json_utils::at(config, "server"s),
			
			host::manager_generator{
				base::json_utils::at(config, "hosts"s),
				base::json_utils::json_from_file(project_config::logic)
			}
		);
	} catch (const std::exception &e) {
		this->handle_error(e);
	}
}


// Run application in interactive mode (reading std::cin and waiting until it will be closed)
int
application::run_interactive() noexcept
{
	if (this->status_)
		return this->status();
	
	try {
		logger::log(logger::level::info, "Running interactive mode. Press Ctrl+D to exit..."s);
		
		// Waiting for Ctrl+D
		while (std::cin)
			std::cin.get();
		
		this->stop();
	} catch (const std::exception &e) {
		this->handle_error(e);
	}
	
	
	return this->status();
}


void
application::stop()
{
	logger::log(logger::level::info, "Stopping..."s);
	
	if (this->server_ptr_)
		this->server_ptr_->stop();
	
	logger::log(logger::level::info, "Stopped."s);
}


void
application::handle_error(const std::exception &e) noexcept
{
	this->status_ = 1;
	
	logger::log(logger::level::critical, e.what() + "."s);
}
