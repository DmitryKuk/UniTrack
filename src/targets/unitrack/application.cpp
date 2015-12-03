// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <application.h>

#include <json.hpp>

#include <base/json_utils.h>
#include <logger/logger.h>
#include <templatizer/page.h>
#include <server/server.h>
#include <server/host/file.h>
#include <server/host/file_handlers/files_only.h>
#include <host/file_handlers/template_pages_only.h>
#include <host/file_handlers/files_and_template_pages.h>

using namespace std::literals;


namespace {


std::unique_ptr<server::host::manager>
generate_host_manager(const nlohmann::json &config, logger::logger &logger)
{
	// Logic and logic config
	auto logic_ptr = std::make_unique<logic::global_instance>(
		logger,
		logic::global_instance::parameters{
			base::json_utils::json_from_file(project_config::logic)
		}
	);
	
	
	// Hosts
	auto host_manager_ptr = std::make_unique<server::host::manager>();
	for (const auto &host_config: base::json_utils::at(config, "hosts"s)) {
		server::host::file_parameters file_host_parameters{host_config};
		
		const auto type = base::json_utils::get<std::string>(host_config, "type"s);
		
		if (type == "files_only"s) {
			host_manager_ptr->add_host(
				std::make_shared<server::host::file<server::host::file_handlers::files_only>>(
					file_host_parameters
				)
			);
		} else if (type == "template_pages_only"s) {
			host_manager_ptr->add_host(
				std::make_shared<server::host::file<host::file_handlers::template_pages_only>>(
					file_host_parameters,
					host::file_handlers::template_pages_only{
						*logic_ptr
					}
				)
			);
		} else if (type == "files_and_template_pages"s) {
			host_manager_ptr->add_host(
				std::make_shared<server::host::file<host::file_handlers::files_and_template_pages>>(
					file_host_parameters,
					host::file_handlers::files_and_template_pages{
						*logic_ptr,
						host::file_handlers::files_and_template_pages::parameters{host_config}
					}
				)
			);
		} else {
			throw server::host::incorrect_config{"Unknown host type: \""s + type
												 + "\", correct values are: \"files_only\", "
												   "\"template_pages_only\", \"files_and_template_pages\""s};
		}
	}
	
	
	return host_manager_ptr;
};


};	// namespace



// class application
application::application(int argc, char **argv) noexcept:
	logger::enable_logger{std::string{argv[0]}},
	
	status_{0}
{
	// std::ios::sync_with_stdio(false);
	
	
	try {
		// Server and hosts config
		auto config = base::json_utils::json_from_file(project_config::server);
		
		
		// Server
		this->server_ptr_ = std::make_unique<server::server>(
			this->logger(),
			server::server::parameters{
				base::json_utils::at(config, "server"s)
			},
			
			// Host manager generator
			std::bind<
				std::unique_ptr<server::host::manager>
			>(
				generate_host_manager,
				std::ref(config),
				std::placeholders::_1
			)
		);
	} catch (const std::exception &e) {
		this->handle_error(e);
	}
}


// Run application in interactive mode (reading std::cid and waiting until it will be closed)
int
application::run_interactive() noexcept
{
	if (this->status_)
		return this->status();
	
	
	try {
		this->logger().log_raw(logger::level::info, "Running interactive mode. Press Ctrl+D to exit..."s);
		
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
	this->logger().log_raw(logger::level::info, "Stopping..."s);
	
	if (this->server_ptr_)
		this->server_ptr_->stop();
	
	this->logger().log_raw(logger::level::info, "Stopped."s);
}


void
application::handle_error(const std::exception &e) noexcept
{
	this->status_ = 1;
	
	this->logger().stream(logger::level::critical)
		<< e.what() << '.';
}
