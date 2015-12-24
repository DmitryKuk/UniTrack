// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <functional>

#include <logger/logger.h>
#include <server/server.h>
#include <server/acceptor.h>
#include <server/host/exceptions.h>


// Constructor
// NOTE: Host manager generator should return std::unique_ptr<server::host::manager> to correct
// and ready-to-response host manager. If it is impossible, please, throw!
template<class HMGen>
server::worker::worker(::server::server &server,
					   std::vector<::server::acceptor> &acceptors,
					   HMGen &&hm_gen):	// Host manager generator
	server_{server},
	
	status_{0},
	
	host_manager_ptr_{
		std::bind<
			std::unique_ptr<::server::host::manager>	// Host manager generator return type
		>(
			std::forward<HMGen>(hm_gen)					// Host manager generator
		)()
	},
	
	empty_work_{this->server_.io_service()},
	signal_set_{this->server_.io_service()},
	
	acceptors_{acceptors},
	
	server_name_generator_{std::random_device{}()}		// /dev/urandom used as seed generator
{
	if (this->host_manager_ptr_ == nullptr)	// Idiot protection
		throw ::server::host::host_manager_not_constructed{};
	
	// Setting up signal handlers
	{
		for (const auto &signal: ::server::worker::signal_handlers_)
			this->signal_set_.add(signal.first);
		
		this->add_signal_handler();
	}
	
	// Start accepting
	for (auto &acceptor: this->acceptors_)
		acceptor.start_accepting(*this);
}


inline
::server::host::manager &
server::worker::host_manager() noexcept
{
	return *this->host_manager_ptr_;
}


inline
boost::asio::io_service &
server::worker::io_service() noexcept
{
	return this->server_.io_service();
}


// Tells all hosts to clear cached data.
inline
void
server::worker::clear_cache() noexcept
{
	this->host_manager().clear_cache();
}
