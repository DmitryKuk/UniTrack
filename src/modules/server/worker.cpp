// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/worker.h>

#include <chrono>

#include <server/server.h>


server::worker::worker(logger::logger &logger,
					   const server::worker::parameters &parameters,
					   server::server &server):
	logger::enable_logger(logger),
	parameters_(parameters),
	
	server_(server),
	
	work_(io_service_),
	
	worker_thread_(std::bind(&worker::run, this)),
	
	server_name_generator_(std::chrono::system_clock::now().time_since_epoch().count())
{}


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
bool
server::worker::add_client(server::socket_ptr_type socket_ptr) noexcept
{
	this->logger().stream(logger::level::info)
		<< "Worker: Adding client to worker " << this->id() << '.';
	
	
	try {
		auto it = this->client_managers_.emplace(this->client_managers_.end(), nullptr);
		*it = std::make_shared<server::client_manager>(this->logger(),
													   *this,
													   it,
													   socket_ptr,
													   this->server_.host_manager());
	} catch (...) {
		this->logger().stream(logger::level::error)
			<< "Worker: Worker " << this->id() << ": Client not added.";
		return false;
	}
	
	
	this->logger().stream(logger::level::info)
		<< "Worker: Worker " << this->id() << ": Client added.";
	return true;
}


// Erases client by iterator. Client manager uses this.
void
server::worker::erase_client(server::client_manager::const_iterator_type iterator) noexcept
{
	this->client_managers_.erase(iterator);
}


// private
// Must be called in worker_thread_ thread
// NOTE: Constructor calls this automatically. Do NOT call it manually!
void
server::worker::run() noexcept
{
	this->logger().stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " started.";
	
	this->io_service_.run();	// Stops when server stops this service
	this->stop();
}


// Stops the worker
// NOTE: Do NOT call this manually! Worker's run() does it.
void
server::worker::stop() noexcept
{
	this->logger().stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " stopping...";
	
	
	
	this->logger().stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " stopped.";
}
