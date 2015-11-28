// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/server.h>

#include <functional>
#include <system_error>

#include <base/json_utils.h>

#include <server/worker.h>


// struct server::server::parameters
server::server::parameters::parameters(const nlohmann::json &config)
{
	base::json_utils::extract(config, this->ports,			"ports");
	base::json_utils::extract(config, this->workers,		"workers");
	base::json_utils::extract(config, this->server_names,	"server_names");
}


// class server::server
server::server::server(logger::logger &logger,
					   ::server::host::manager &host_manager,
					   const ::server::server::parameters &parameters):
	logger::enable_logger{logger},
	
	parameters_{parameters},
	
	host_manager_{host_manager}
{
	try {
		// Workers creation
		this->worker_processes_.reserve(this->parameters_.workers);
		
		for (unsigned int i = 0; i < this->parameters_.workers; ++i)
			this->worker_processes_.emplace_back(::server::worker::run(this->logger(), *this));
		
		this->logger().stream(logger::level::info)
			<< "Server: " << this->parameters_.workers << " workers created.";
	} catch (const std::exception &e) {
		this->logger().stream(logger::level::critical)
			<< "Server: Caught exception: " << e.what() << '.';
		
		this->logger().stream(logger::level::critical)
			<< "Server: NOT started.";
		
		// Stopping running workers
		// for (auto &process: this->worker_processes_)
		// 	process.kill();
	}
}


void
server::server::stop() noexcept
{
	this->logger().stream(logger::level::info)
		<< "Server: Stopping...";
	
	// Stopping running workers
	// for (auto &process: this->worker_processes_)
	// 	process.kill();
	
	this->logger().stream(logger::level::info)
		<< "Server: Stopped.";
}
