// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/worker.h>

#include <server/server.h>
#include <server/acceptor.h>
#include <server/session.h>


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
	
	server_name_generator_{std::random_device{}()}	// /dev/urandom used as seed generator
	// static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())
{
	this->acceptors_.reserve(this->server_.parameters_.ports.size());
	for (auto port: this->server_.parameters_.ports)
		this->acceptors_.emplace_back(*this, port);
}


// Returns exit status for current process
int
server::worker::run() noexcept
{
	this->logger().stream(logger::level::info)
		<< "Worker started.";
	
	
	try {
		this->io_service_.run();
	} catch (const std::exception &e) {
		this->logger().stream(logger::level::error)
			<< "Worker failed: " << e.what();
		
		return 1;
	}
	
	
	this->logger().stream(logger::level::info)
		<< "Worker stopped.";
	
	
	return 0;
}
