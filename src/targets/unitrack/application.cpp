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
	argc_{argc},
	argv_{argv},
	
	status_{0}
{
	try {
		// Logger
		this->logger_global_instance_ptr_ = std::make_unique<logger::global_instance>(this->argv_[0]);
		
		
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


// Runs application in automatic mode (interactive or not).
// Returns status of execution.
int
application::run() noexcept
{
	return this->run_interactive();
}


// Runs application in interactive mode (reading std::cid and waiting until it will be closed).
// Returns status of execution.
int
application::run_interactive() noexcept
{
	if (this->status_)
		return this->status();
	
	try {
		logger::log(logger::level::info, "Running interactive mode. "
										 "Press Ctrl+D to exit. "
										 "Press Enter to clear cache..."s);
		
		// Waiting for Ctrl+D
		while (std::cin) {
			if (std::cin.get() == '\n')
				this->server_ptr_->clear_cache();
		}
		
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
