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
template<class HMGen, class... HMGenArgs>
system_::process
server::worker::run(::server::server &server,
					const logger::async_logger::parameters &logger_parameters,
					HMGen &&hm_gen, HMGenArgs &&... hm_gen_args)	// Host manager generator with args
{
	return system_::process{
		[&]
		{
			return ::server::worker{
				server,
				logger_parameters,
				std::forward<HMGen>(hm_gen),
				std::forward<HMGenArgs>(hm_gen_args)...
			}.run();
		}
	};
}


inline
logger::async_logger &
server::worker::logger() const noexcept
{
	return this->logger_;
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
template<class HMGen, class... HMGenArgs>
server::worker::worker(::server::server &server,
					   const logger::async_logger::parameters &logger_parameters,
					   HMGen &&hm_gen, HMGenArgs &&... hm_gen_args):	// Host manager generator with args
	server_{server},
	
	status_{0},
	
	logger_{this->io_service_, logger_parameters},
	
	host_manager_ptr_{
		std::bind<
			std::unique_ptr<::server::host::manager>	// Host manager generator return type
		>(
			std::forward<HMGen>(hm_gen),				// Host manager generator
			std::forward<HMGenArgs>(hm_gen_args)...,	// Args for it
			std::placeholders::_1
		)(this->logger_)
	},
	
	empty_work_{this->io_service_},
	signal_set_{this->io_service_, SIGTERM, SIGINT},
	
	server_name_generator_{std::random_device{}()}	// /dev/urandom used as seed generator
{
	if (this->host_manager_ptr_ == nullptr)	// Idiot protection
		throw ::server::host::host_manager_not_constructed{};
	
	using namespace std::placeholders;
	this->signal_set_.async_wait(std::bind(&::server::worker::handle_signal, this, _1, _2));
	
	this->acceptors_.reserve(this->server_.parameters_.ports.size());
	for (auto port: this->server_.parameters_.ports)
		this->acceptors_.emplace_back(*this, port);
}
