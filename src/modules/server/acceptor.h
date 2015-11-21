// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_ACCEPTOR_H
#define SERVER_ACCEPTOR_H

#include <boost/asio.hpp>

#include <logger/logger.h>
#include <server/types.h>


namespace server {


class server;


class acceptor:
	protected logger::enable_logger
{
public:
	struct parameters {
		server::port_type port;
	};	// struct parameters
	
	
	acceptor(logger::logger &logger,
			 server::server &server,
			 const parameters &parameters,
			 boost::asio::io_service &acceptors_io_service,
			 boost::asio::io_service &workers_io_service);
	~acceptor();
private:
	// Handlers
	// Handles the accept event
	void accept_handler(server::socket_ptr_type socket_ptr,
						const boost::system::error_code &err) noexcept;
	
	// Add accept_handler to the io_service event loop
	void add_accept_handler() noexcept;
	
	
	// Data
	server::server &server_;
	
	parameters parameters_;
	
	boost::asio::io_service &acceptors_io_service_,
							&workers_io_service_;	// Binding new sockets to it
	boost::asio::ip::tcp::endpoint endpoint_;
	boost::asio::ip::tcp::acceptor acceptor_;
};


};	// namespace server


#endif // SERVER_ACCEPTOR_H
