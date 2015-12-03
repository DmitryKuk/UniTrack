// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/worker.h>

#include <functional>

#include <logger/logger.h>
#include <server/server.h>
#include <server/session.h>

using namespace std::literals;


#define SIGNAL_REACTION(signal, reaction)	\
	std::make_pair(signal, std::make_pair(#signal, &::server::worker::reaction))

// Signal handlers map
// static
const std::unordered_map<int, std::pair<const char *, void (server::worker::*)(int, const char *)>>
	server::worker::signal_handlers_ =
		{
			SIGNAL_REACTION(SIGHUP,		ignore),
#ifdef SIGQUIT
			SIGNAL_REACTION(SIGQUIT,	exit),
#endif	// SIGQUIT
			SIGNAL_REACTION(SIGTERM,	exit),
			SIGNAL_REACTION(SIGINT,		exit)
		};

#undef SIGNAL_REACTION


// Returns server name (random!)
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
	
	
	logger::stream(logger::level::info)
		<< "Client accepted: "s << client_address << '.';
	
	
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
// Returns exit status for current process
int
server::worker::run() noexcept
{
	this->status_ = 0;
	
	logger::log(logger::level::info, "Worker started."s);
	
	
	try {
		this->io_service_.run();
	} catch (const std::exception &e) {
		logger::stream(logger::level::error)
			<< "Worker failed: "s << e.what();
		
		this->status_ = 1;
	}
	
	
	logger::log(logger::level::info, "Worker stopped."s);
	
	
	return this->status_;
}


// Handles signals and dispatch them to primitive handlers
void
server::worker::handle_signal(const boost::system::error_code &err, int signal) noexcept
{
	if (err) {
		logger::stream(logger::level::critical)
			<< "Worker process signal handle error: "s << err.message() << '.';
		
		this->status_ = 1;
		this->io_service_.stop();
	} else {
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
		<< "Worker: Recieved signal: "s << signal_str << " ("s << signal << "). Stopping..."s;
	
	this->io_service_.stop();
}


void
server::worker::ignore(int signal, const char *signal_str) noexcept
{
	logger::stream(logger::level::info)
		<< "Worker: Recieved signal: "s << signal_str << " ("s << signal << "). Ignore."s;
}
