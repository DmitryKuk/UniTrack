// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/logger.h>
#include <base/json_utils.h>
#include <server/acceptor.h>
#include <server/worker.h>


template<class HMGen>
server::server::server(const nlohmann::json &config,
					   HMGen &&hm_gen)
{
	using namespace std::literals;
	
	
	base::json_utils::extract(config, this->ports_,		"ports"s);
	base::json_utils::extract(config, this->workers_,	"workers"s);
	base::json_utils::extract(config, this->names_,		"server_names"s);
	
	
	try {
		// Creating acceptors
		std::vector<::server::acceptor>	acceptors;	// Need for accepting clients on ports
		
		acceptors.reserve(this->ports_.size());
		for (auto port: this->ports_)
			acceptors.emplace_back(this->io_service_, port);
		
		
		// Creating workers
		this->worker_processes_.reserve(this->workers_);
		
		auto workers_message = "Server: Workers created ("s + std::to_string(this->workers_) + "):"s;
		
		for (unsigned int i = 0; i < this->workers_; ++i) {
			this->io_service_.notify_fork(boost::asio::io_service::fork_prepare);
			this->worker_processes_.emplace_back(
				[&]() -> int
				{
					this->io_service_.notify_fork(boost::asio::io_service::fork_child);
					return ::server::worker{*this, acceptors, std::forward<HMGen>(hm_gen)}.run();
				}
			);
			this->io_service_.notify_fork(boost::asio::io_service::fork_parent);
			
			
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


// Returns server names
inline
const std::vector<std::string> &
server::server::names() const noexcept
{
	return this->names_;
}


inline
boost::asio::io_service &
server::server::io_service() noexcept
{
	return this->io_service_;
}
