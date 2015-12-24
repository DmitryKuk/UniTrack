// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/worker.h>

#include <functional>

#include <logger/logger.h>
#include <server/server.h>
#include <server/session.h>

using namespace std::literals;


// Signal, that means clear_cache command for worker.
// static
constexpr int ::server::worker::clear_cache_signal;


#define SIGNAL_REACTION(signal, reaction)	\
	std::make_pair(signal, std::make_pair(#signal, &::server::worker::reaction))

// Signal handlers map
// static
const std::unordered_map<int, std::pair<const char *, void (server::worker::*)(int, const char *)>>
	server::worker::signal_handlers_{
		{
			SIGNAL_REACTION(SIGHUP,								ignore		),
			
#ifdef SIGQUIT
			SIGNAL_REACTION(SIGQUIT,							exit		),
#endif	// SIGQUIT
			SIGNAL_REACTION(SIGTERM,							exit		),
			SIGNAL_REACTION(SIGINT,								exit		),
			
			SIGNAL_REACTION(worker::clear_cache_signal,	clear_cache_handler	)
		}
	};

#undef SIGNAL_REACTION


// Returns exit status for current process
int
server::worker::run() noexcept
{
	this->status_ = 0;
	
	logger::log(logger::level::info, "Started."s);
	
	
	try {
		this->io_service().run();
		
		logger::log(logger::level::info, "Stopped."s);
	} catch (const std::exception &e) {
		logger::stream(logger::level::error)
			<< "Failed: "s << e.what();
		
		this->status_ = 1;
	}
	
	
	return this->status_;
}


// Returns random server name
const std::string &
server::worker::server_name() const noexcept
{
	const auto &server_names = this->server_.names();
	
	if (server_names.empty()) {
		static const std::string empty_name;
		
		return empty_name;
	}
	
	size_t index = this->server_name_generator_() % server_names.size();
	return server_names[index];
}


// Adds new client to the worker
// Returns true, if added successfully
void
server::worker::add_client(::server::socket &&socket) noexcept
{
	auto client_address = socket.remote_endpoint().address();
	
	try {
		::server::session::run(*this, std::move(socket));
	} catch (const std::exception &e) {
		logger::stream(logger::level::error)
			<< "Can\'t process client: "s << client_address << ": "s << e.what() << '.';
	} catch (...) {
		logger::stream(logger::level::error)
			<< "Can\'t process client: "s << client_address << ": Unknown error."s;
	}
}


// private
void
server::worker::add_signal_handler() noexcept
{
	using namespace std::placeholders;
	this->signal_set_.async_wait(std::bind(&::server::worker::signal_handler, this, _1, _2));
}


// Handles signals and dispatch them to primitive handlers
void
server::worker::signal_handler(const boost::system::error_code &err, int signal) noexcept
{
	if (err) {
		logger::stream(logger::level::critical)
			<< "Signal handle error: "s << err.message() << '.';
		
		this->status_ = 1;
		this->io_service().stop();
	} else {
		this->add_signal_handler();	// Waiting for the next signal
		
		auto it = worker::signal_handlers_.find(signal);
		if (it == worker::signal_handlers_.end())
			this->exit(signal, "Unknown");
		else
			std::bind(it->second.second, this, it->first, it->second.first)();
	}
}


// Signal handlers
void
server::worker::exit(int signal, const char *signal_str) noexcept
{
	logger::stream(logger::level::info)
		<< "Recieved signal: "s << signal_str << " ("s << signal << "). Stopping..."s;
	
	this->io_service().stop();
}


void
server::worker::ignore(int signal, const char *signal_str) noexcept
{
	logger::stream(logger::level::info)
		<< "Recieved signal: "s << signal_str << " ("s << signal << "). Ignoring..."s;
}


void
server::worker::clear_cache_handler(int signal, const char *signal_str) noexcept
{
	logger::stream(logger::level::info)
		<< "Recieved signal: "s << signal_str << " ("s << signal << "). Cleaning cache..."s;
	
	this->clear_cache();
}
