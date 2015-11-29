// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/worker.h>

#include <functional>

#include <server/server.h>
#include <server/acceptor.h>
#include <server/session.h>


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


// Creates and runs new worker in separate process
// Returns correct child process object (only in parent process!)
// Throws: std::system_error, if it's impossible to create process (only in parent process!)
// NEVER returns in child process!
// static
system_::process
server::worker::run(logger::logger &logger, ::server::server &server)
{
	return system_::process{[&] { return ::server::worker{logger, server}.run(); }};
}


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
	
	
	this->logger().stream(logger::level::info)
		<< "Client accepted: " << client_address << '.';
	
	
	try {
		::server::session::run(*this, std::move(socket));
	} catch (const std::exception &e) {
		this->logger().stream(logger::level::error)
			<< "Can\'t process client: " << client_address << ": " << e.what() << '.';
	} catch (...) {
		this->logger().stream(logger::level::error)
			<< "Can\'t process client: " << client_address << ": Unknown error.";
	}
}


// private
// Constructor
server::worker::worker(logger::logger &logger, ::server::server &server):
	logger::enable_logger{logger},
	
	server_{server},
	
	empty_work_{this->io_service_},
	signal_set_{this->io_service_, SIGTERM, SIGINT},
	
	server_name_generator_{std::random_device{}()}	// /dev/urandom used as seed generator
{
	using namespace std::placeholders;
	this->signal_set_.async_wait(std::bind(&::server::worker::handle_signal, this, _1, _2));
	
	this->acceptors_.reserve(this->server_.parameters_.ports.size());
	for (auto port: this->server_.parameters_.ports)
		this->acceptors_.emplace_back(*this, port);
}


// Returns exit status for current process
int
server::worker::run() noexcept
{
	this->status_ = 0;
	
	this->logger().stream(logger::level::info)
		<< "Worker started.";
	
	
	try {
		this->io_service_.run();
	} catch (const std::exception &e) {
		this->logger().stream(logger::level::error)
			<< "Worker failed: " << e.what();
		
		this->status_ = 1;
	}
	
	
	this->logger().stream(logger::level::info)
		<< "Worker stopped.";
	
	
	return this->status_;
}


// Handles signals and dispatch them to primitive handlers
void
server::worker::handle_signal(const boost::system::error_code &err, int signal) noexcept
{
	if (err) {
		this->logger().stream(logger::level::critical)
			<< "Worker process signal handle error: " << err.message() << '.';
		
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
	this->logger().stream(logger::level::info)
		<< "Worker: Recieved signal: " << signal_str << " (" << signal << "). Stopping...";
	
	this->io_service_.stop();
}


void
server::worker::ignore(int signal, const char *signal_str) noexcept
{
	this->logger().stream(logger::level::info)
		<< "Worker: Recieved signal: " << signal_str << " (" << signal << "). Ignore.";
}
