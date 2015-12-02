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
void
server::server::stop() noexcept
{
	this->logger().stream(logger::level::info)
		<< "Server: Stopping...";
	
	// Stopping running workers
	for (auto &process: this->worker_processes_) {
		try {
			process.kill();
		} catch (const std::exception &e) {
			this->logger().stream(logger::level::error)
				<< "Server: Worker stopping error: " << e.what();
		}
	}
	this->worker_processes_.clear();
	
	this->logger().stream(logger::level::info)
		<< "Server: Stopped.";
}
