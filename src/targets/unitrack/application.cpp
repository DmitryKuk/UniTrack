// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <application.h>

#include <iostream>
#include <stdexcept>

#include <json.hpp>

#include <base/json_utils.h>
#include <templatizer/page.h>
#include <server/server.h>
#include <server/host/file.h>
#include <server/host/file/files_only.h>
#include <host/file/template_pages_only.h>
#include <host/file/files_and_template_pages.h>


application::application(int argc, char **argv):
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
		this->logic_ptr_ = std::make_unique<logic::global_instance>(
			this->logger(),
			logic::global_instance::parameters{
				base::json_utils::json_from_file(project_config::logic)
			}
		);
		
		
		// Config
		auto config = base::json_utils::json_from_file(server_config_path);
		
		
		// Server
		this->server_ptr_ = std::make_unique<server::server_http>(
			this->logger(),
			server::server::parameters{
				base::json_utils::at(config, "server")
			}
		);
		
		
		// Hosts
		for (const auto &host_config: base::json_utils::at(config, "hosts")) {
			server::host::file::parameters params{host_config};
			const auto type = base::json_utils::get<std::string>(host_config, "type");
			
			if (type == "files_only") {
				this->server_ptr_->host_manager().add_host(
					std::make_shared<server::file_host<server::files_only>>(
						this->logger(),
						params
					)
				);
			} else if (type == "template_pages_only") {
				this->server_ptr_->host_manager().add_host(
					std::make_shared<server::host::file<host::file::template_pages_only>>(
						this->logger(),
						params,
						host::file::template_pages_only{
							this->logic_global_instance_
						}
					)
				);
			} else if (type == "files_and_template_pages") {
				this->server_ptr_->host_manager().add_host(
					std::make_shared<server::host::file<host::file::files_and_template_pages>>(
						this->logger(),
						params,
						host::file::files_and_template_pages{
							this->logic_global_instance_,
							host::file::files_and_template_pages::parameters{host_config}
						}
					)
				);
			} else {
				throw std::logic_error("Incorrect config");
			}
		}
	} catch (const std::exception &e) {
		this->handle_error(e);
	}
}


// Run application in interactive mode (reading std::cid and waiting until it will be closed)
int
application::run_interactive() noexcept
{
	if (this->status_) return this->status();
	
	
	try {
		this->logger().stream(logger::level::info)
			<< "Running interactive mode. Press Ctrl+D to exit...";
		
		// Waiting for Ctrl+D
		while (std::cin) std::cin.get();
		
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
application::handle_error(const std::exception &e)
{
	this->status_ = 1;
	
	try {
		if (this->logger_ptr_ == nullptr)
			throw std::runtime_error("Logic not set");
		
		this->logger().stream(logger::level::critical)
			<< "Critical error occured: " << e.what() << '.';
	} catch (const std::exception &) {
		std::cerr << "Critical error occured: " << e.what() << std::endl;
	}
}
