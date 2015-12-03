// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/logger.h>
#include <server/worker.h>


template<class HMGen>
server::server::server(const ::server::server::parameters &parameters,
					   HMGen &&hm_gen):
	parameters_{parameters}
{
	using namespace std::literals;
	
	
	try {
		// Workers creation
		this->worker_processes_.reserve(this->parameters_.workers);
		
		auto workers_message = "Server: Workers created ("s + std::to_string(this->parameters_.workers) + "):"s;
		
		for (unsigned int i = 0; i < this->parameters_.workers; ++i) {
			this->worker_processes_.emplace_back(::server::worker::run(*this, std::forward<HMGen>(hm_gen)));
			
			workers_message += ' ';
			workers_message += std::to_string(this->worker_processes_.back().get_id());
		}
		
		workers_message += '.';
		logger::log(logger::level::info, workers_message);
	} catch (const std::exception &e) {
		logger::log(logger::level::critical, "Server: Caught exception: "s + e.what() + '.');
		
		// Stopping running workers
		this->stop();
		
		logger::log(logger::level::critical, "Server: NOT started."s);
		
		throw;
	}
}


// // Checks server's thread for joinable
// inline bool
// server::server::joinable() const noexcept
// {
// 	return this->server_thread_.joinable();
// }


// // Joins server's thread
// inline void
// server::server::join()
// {
// 	this->server_thread_.join();
// }


// // Detaches server's thread
// inline void
// server::server::detach()
// {
// 	this->server_thread_.detach();
// }


// Returns server names
inline
const std::vector<std::string> &
server::server::names() const noexcept
{
	return this->parameters_.server_names;
}
