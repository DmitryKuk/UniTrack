// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/worker.h>


template<class HMGen>
server::server::server(logger::logger &logger,
					   const ::server::server::parameters &parameters,
					   HMGen &&hm_gen):
	logger::enable_logger_ref{logger},
	
	parameters_{parameters}
{
	try {
		// Workers creation
		this->worker_processes_.reserve(this->parameters_.workers);
		
		for (unsigned int i = 0; i < this->parameters_.workers; ++i) {
			this->logger().stream(logger::level::info)
				<< "Creating worker...";
			
			this->worker_processes_.emplace_back(::server::worker::run(*this, std::forward<HMGen>(hm_gen)));
			
			this->logger().stream(logger::level::info)
				<< "Worker created: " << this->worker_processes_.back().get_id() << "...";
		}
		
		this->logger().stream(logger::level::info)
			<< "Server: Workers created: " << this->parameters_.workers << '.';
	} catch (const std::exception &e) {
		this->logger().stream(logger::level::critical)
			<< "Server: Caught exception: " << e.what() << '.';
		
		this->logger().stream(logger::level::critical)
			<< "Server: NOT started.";
		
		// Stopping running workers
		this->stop();
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
