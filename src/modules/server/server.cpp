// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/server.h>

#include <functional>
#include <system_error>

#include <logger/logger.h>
#include <base/json_utils.h>
#include <server/worker.h>

using namespace std::literals;


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
