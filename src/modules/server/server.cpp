// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/server.h>

#include <functional>
#include <system_error>

#include <logger/logger.h>
#include <base/json_utils.h>
#include <server/worker.h>

using namespace std::literals;


server::server::~server()
{
	this->stop();
}


// Detaches all workers: they will NOT be killed, when server object destroyed.
void
server::server::detach_workers() noexcept
{
	this->worker_processes_.clear();
}


// Stops all workers.
void
server::server::stop() noexcept
{
	for (auto &process: this->worker_processes_) {
		try {
			process.kill();
		} catch (const std::exception &e) {
			logger::log(logger::level::error, "Server: Worker stopping error: "s + e.what());
		}
	}
	
	this->detach_workers();	// Simple clears workers
}


// Tells all workers to clear cached data.
void
server::server::clear_cache() noexcept
{
	for (auto &process: this->worker_processes_) {
		try {
			process.kill(::server::worker::clear_cache_signal);
		} catch (const std::exception &e) {
			logger::log(logger::level::error, "Server: Worker cleaning cache error: "s + e.what());
		}
	}
}
