// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <interface_manager.h>

#include <fstream>
#include <stdexcept>

#include <json.hpp>
#include <base/json_utils.h>

#include <templatizer/page.h>

#include <server/server_http.h>
#include <server/file_host.h>
#include <server/file_host_files_only.h>

#include <file_host_template_pages_only.h>
#include <file_host_files_and_template_pages.h>


interface_manager::interface_manager(logger::logger &logger,
									 logic::global_instance &logic_global_instance,
									 const boost::filesystem::path &server_config_path):
	logger::enable_logger(logger),
	
	logic_global_instance_(logic_global_instance)
{
	try {
		this->logger().stream(logger::level::info)
			<< "Interface manager: Reading config: " << server_config_path << "...";
		
		nlohmann::json config = base::json_utils::json_from_file(server_config_path);
		
		
		// Server parameters
		{
			const auto &server = base::json_utils::at(config, "server_http");
			
			server::server_http_parameters params;
			params.ports = base::json_utils::get<decltype(params.ports)>(server, "ports");
			params.workers = base::json_utils::get<decltype(params.workers)>(server, "workers");
			
			this->server_ptr_ = std::make_unique<server::server_http>(logger, params);
		}
		
		
		// Hosts parameters
		{
			const auto &host_list_config = base::json_utils::at(config, "file_hosts");
			
			for (const auto &host_config: host_list_config) {
				server::file_host_parameters params(host_config);
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
						std::make_shared<server::file_host<template_pages_only>>(
							this->logger(),
							params,
							this->logic_global_instance_
						)
					);
				} else if (type == "files_and_template_pages") {
					files_and_template_pages_parameters additional_params(host_config);
					
					this->server_ptr_->host_manager().add_host(
						std::make_shared<server::file_host<files_and_template_pages>>(
							this->logger(),
							params,
							files_and_template_pages(
								this->logic_global_instance_,
								additional_params
							)
						)
					);
				} else {
					throw std::logic_error("Incorrect config");
				}
			}
		}
	} catch (const std::exception &e) {
		this->logger().stream(logger::level::error)
			<< "Interface manager: " << e.what() << '.';
	}
}


void
interface_manager::stop()
{
	if (this->server_ptr_)
		this->server_ptr_->stop();
}
