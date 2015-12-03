// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/server.h>

#include <functional>
#include <system_error>

#include <logger/logger.h>
#include <base/json_utils.h>
#include <server/worker.h>

using namespace std::literals;


// struct server::server::parameters
server::server::parameters::parameters(const nlohmann::json &config)
{
	base::json_utils::extract(config, this->ports,			"ports"s);
	base::json_utils::extract(config, this->workers,		"workers"s);
	base::json_utils::extract(config, this->server_names,	"server_names"s);
}


// class server::server
void
server::server::stop() noexcept
{
	logger::log(logger::level::info, "Server: Stopping..."s);
	
	// Stopping running workers
	for (auto &process: this->worker_processes_) {
		try {
			process.kill();
		} catch (const std::exception &e) {
			logger::log(logger::level::error, "Server: Worker stopping error: "s + e.what());
		}
	}
	this->worker_processes_.clear();
	
	logger::log(logger::level::info, "Server: Stopped."s);
}
