// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/acceptor.h>

#include <functional>

#include <server/worker.h>


server::acceptor::acceptor(::server::worker &worker, ::server::port_type port):
	boost::asio::ip::tcp::acceptor{worker.io_service(),
								   {boost::asio::ip::tcp::v4(), port},	// Local endpoint
								   true},								// Reuse address
	
	logger::enable_logger{worker.logger()},
	
	worker_{worker},
	socket_{this->worker_.io_service()}
{
	this->add_accept_handler();
	
	this->logger().stream(logger::level::info)
		<< "Accepting on port: " << this->local_endpoint().port() << ": started.";
}


server::acceptor::~acceptor()
{
	boost::system::error_code err;
	this->close(err);
}


// Add accept_handler to the io_service event loop
void
server::acceptor::add_accept_handler()
{
	using namespace std::placeholders;
	
	this->async_accept(this->socket_, std::bind(&acceptor::accept_handler, this, _1));
}


// Handles the accept event
void
server::acceptor::accept_handler(const boost::system::error_code &err)
{
	if (err) {
		this->logger().stream(logger::level::error)
			<< "Accepting on port: " << this->local_endpoint().port() << ": " << err.message() << '.';
	} else {
		this->worker_.add_client(std::move(this->socket_));
		
		this->add_accept_handler();	// Continue accepting
	}
}
