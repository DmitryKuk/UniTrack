// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <functional>

#include <cstdlib>

#include <server/server.h>
#include <server/acceptor.h>
#include <server/host/exceptions.h>


// Creates and runs new worker in separate process
// Returns correct child process object (only in parent process!)
// Throws: std::system_error, if it's impossible to create process (only in parent process!)
// NEVER returns in child process!

// NOTE: Host manager generator should return std::unique_ptr<server::host::manager> to correct
// and ready-to-response host manager. If it is impossible, please, throw!
// static
template<class HMGen>
system_::process
server::worker::run(::server::server &server,
					HMGen &&hm_gen)	// Host manager generator
{
	return system_::process{
		[&]
		{
			return ::server::worker{server, std::forward<HMGen>(hm_gen)}.run();
		}
	};
}


boost::asio::io_service &
server::worker::io_service() noexcept
{
	return this->io_service_;
}


inline
::server::host::manager &
server::worker::host_manager() noexcept
{
	return *this->host_manager_ptr_;
}


// Constructor
// NOTE: Host manager generator should return std::unique_ptr<server::host::manager> to correct
// and ready-to-response host manager. If it is impossible, please, throw!
template<class HMGen>
server::worker::worker(::server::server &server,
					   HMGen &&hm_gen):	// Host manager generator
	logger::enable_logger{server.logger().log_name() + std::string{"-worker"}},
	
	server_{server},
	
	status_{0},
	
	host_manager_ptr_{
		std::bind<
			std::unique_ptr<::server::host::manager>	// Host manager generator return type
		>(
			std::forward<HMGen>(hm_gen),				// Host manager generator
			std::placeholders::_1
		)(this->logger())
	},
	
	empty_work_{this->io_service_},
	signal_set_{this->io_service_, SIGTERM, SIGINT},
	
	server_name_generator_{std::random_device{}()}		// /dev/urandom used as seed generator
{
	if (this->host_manager_ptr_ == nullptr) {	// Idiot protection
		using namespace std::literals;
		this->logger().log_raw(logger::level::critical, "Worker: Host manager not constructed. Are you kidding?"s);
		throw ::server::host::host_manager_not_constructed{};
	}
	
	
	using namespace std::placeholders;
	this->signal_set_.async_wait(std::bind(&::server::worker::handle_signal, this, _1, _2));
	
	this->acceptors_.reserve(this->server_.parameters_.ports.size());
	for (auto port: this->server_.parameters_.ports)
		this->acceptors_.emplace_back(*this, port);
}
