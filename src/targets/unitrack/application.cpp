// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <application.h>

#include <iostream>

#include <json.hpp>

#include <base/json_utils.h>
#include <templatizer/page.h>
#include <server/server.h>
#include <server/host/file.h>
#include <server/host/file_handlers/files_only.h>
#include <host/file_handlers/template_pages_only.h>
#include <host/file_handlers/files_and_template_pages.h>


application::application(int argc, char **argv) noexcept:
	status_{0}
{
	using namespace std::literals;
	
	
	std::ios::sync_with_stdio(false);
	
	
	// Creating logger
	try {
		this->logger_ptr_ = std::make_unique<logger::logger>(std::clog, true);
	} catch (const std::exception &e) {
		this->handle_error(std::runtime_error{"Can\'t create logger: "s + e.what()});
		return;
	}
	
	
	try {
		// Logic and logic config
		this->logic_ptr_ = std::make_unique<logic::global_instance>(
			this->logger(),
			logic::global_instance::parameters{
				base::json_utils::json_from_file(project_config::logic)
			}
		);
		
		
		// Server and hosts config
		auto config = base::json_utils::json_from_file(project_config::server);
		
		
		// Hosts
		this->host_manager_ptr_ = std::make_unique<server::host::manager>(this->logger());
		for (const auto &host_config: base::json_utils::at(config, "hosts")) {
			server::host::file_parameters file_host_parameters{host_config};
			const auto type = base::json_utils::get<std::string>(host_config, "type");
			
			if (type == "files_only") {
				this->host_manager_ptr_->add_host(
					std::make_shared<server::host::file<server::host::file_handlers::files_only>>(
						this->logger(),
						file_host_parameters
					)
				);
			} else if (type == "template_pages_only") {
				this->host_manager_ptr_->add_host(
					std::make_shared<server::host::file<host::file_handlers::template_pages_only>>(
						this->logger(),
						file_host_parameters,
						host::file_handlers::template_pages_only{
							*this->logic_ptr_
						}
					)
				);
			} else if (type == "files_and_template_pages") {
				this->host_manager_ptr_->add_host(
					std::make_shared<server::host::file<host::file_handlers::files_and_template_pages>>(
						this->logger(),
						file_host_parameters,
						host::file_handlers::files_and_template_pages{
							*this->logic_ptr_,
							host::file_handlers::files_and_template_pages::parameters{host_config}
						}
					)
				);
			} else {
				throw server::host::incorrect_config{"Unknown host type: \"" + type
													 + "\", correct values are: \"files_only\", "
													   "\"template_pages_only\", \"files_and_template_pages\""};
			}
		}
		
		
		// Server
		this->server_ptr_ = std::make_unique<server::server>(
			this->logger(),
			*this->host_manager_ptr_,
			server::server::parameters{
				base::json_utils::at(config, "server")
			}
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
		this->logger().stream(logger::level::info)
			<< "Running interactive mode. Press Ctrl+D to exit...";
		
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
	this->logger().stream(logger::level::info)
		<< "Stopping...";
	
	if (this->server_ptr_)
		this->server_ptr_->stop();
	
	this->logger().stream(logger::level::info)
		<< "Stopped.";
}


void
application::handle_error(const std::exception &e) noexcept
{
	this->status_ = 1;
	
	if (this->logger_ptr_ == nullptr) {
		try {
			std::cerr << "Critical error occured: Logger not set.";
			std::cerr << "Critical error occured: " << e.what() << std::endl;
		} catch (...) {}
	} else {
		this->logger().stream(logger::level::critical)
			<< e.what() << '.';
	}
}
