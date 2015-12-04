// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/acceptor.h>

#include <functional>
#include <string>

#include <sys/socket.h>	// Need to enable SO_REUSEPORT socket option manually

#include <logger/logger.h>
#include <server/worker.h>

using namespace std::literals;


server::acceptor::acceptor(::server::worker &worker, ::server::port_type port):
	boost::asio::ip::tcp::acceptor{worker.io_service()},
	
	worker_ptr_{&worker},
	socket_{this->worker_ptr_->io_service()}
{
	// Configuring acceptor's socket manually
	{
		boost::asio::ip::tcp::endpoint local_endpoint{boost::asio::ip::tcp::v4(), port};
		
		this->open(local_endpoint.protocol());
		
		// Without this bind() will fail on all systems
		this->set_option(boost::asio::ip::tcp::acceptor::reuse_address{true});
		
#ifdef SO_REUSEPORT
		{
			// Without this bind() will fail on BSD-like systems
			int enable_option = 1;
			::setsockopt(this->native_handle(), SOL_SOCKET, SO_REUSEPORT, &enable_option, sizeof(enable_option));
		}
#endif	// SO_REUSEPORT
		
		this->bind(local_endpoint);
	}
	
	
	this->add_accept_handler();
	
	logger::stream(logger::level::info)
		<< "Accepting on port: "s << this->local_endpoint().port() << ": started."s;
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
		logger::stream(logger::level::error)
			<< "Accepting on port: "s << this->local_endpoint().port() << ": "s << err.message() << '.';
	} else {
		this->worker_ptr_->add_client(std::move(this->socket_));
		
		this->add_accept_handler();	// Continue accepting
	}
}
